#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>

#include "../Luau/Bytecode.h"
#include "../LuauPseudo/pseudo_lobject.h"

#include "../LuauPseudo/PseudoProto.h"

#include "../LuauPseudo/BytecodeParser.h"

template<typename T>
static T read(const char* data, size_t& offset)
{
    T result;
    memcpy(&result, data + offset, sizeof(T));
    offset += sizeof(T);

    return result;
}

static unsigned int readVarInt(const char* data, size_t& offset)
{
    unsigned int result = 0;
    unsigned int shift = 0;

    uint8_t byte;

    do
    {
        byte = read<uint8_t>(data, offset);
        result |= (byte & 127) << shift;
        shift += 7;
    } while (byte & 128);

    return result;
}

static char* readString(std::vector<char*> strings, const char* data, size_t& offset)
{
    unsigned int id = readVarInt(data, offset);

    return id == 0 ? NULL : strings[id - 1];
}


// Dumps Hexadecimal File Content of the current buffer location
// Useful to figure out, where the reader is currently at, within a file for debugging.
void readerDumpCurrentLocation(char* buffer, size_t offset, int deepness) {


    for (int i = 0; i < deepness; i++) {
        printf("%02X ", static_cast<unsigned char>(*(buffer + offset + i)));
    }
    printf("\n");

    return;
}

TValue CreateEmptyTValue() {
    TValue newValue;

    newValue.value.gc = nullptr;
    newValue.value.tbl = nullptr;
    newValue.value.str = nullptr;
    newValue.value.p = nullptr;
    newValue.value.tbl = nullptr;

    return newValue;
}


int main()
{
    //std::ifstream file("../tests/abc.lua.luac", std::ios::binary);
    std::ifstream file("AnimationClipEditor.luac", std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    // Determine the file size
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Allocate buffer to hold entire file content
    char* fileBuffer = new char[fileSize];

    // Read the entire file into buffer
    file.read(fileBuffer, fileSize);

    if (!file) {
        std::cerr << "Failed to read from file." << std::endl;
        delete[] fileBuffer; // Clean up allocated memory
        return 1;
    }

    // Close the file
    file.close();

    size_t offset = 0;
    uint8_t version = read<uint8_t>(fileBuffer, offset);

    printf("Luau Bytecode Version: %d\n", (int)version);


    // Collecting the strings
    //
    // Not sure if stringCount can ever go higher than 1, and what it would mean
    unsigned int stringCount = readVarInt(fileBuffer, offset);

    std::vector<char*> strings; // We will store strings in here;

    /*
        The string block has the following data:
        First there's the length, that readVarInt reads through.
        This "lenght" defines how many strings there actually are.


        Every string is separated by a byte, this byte is at the front of the string
        and defines the length (in-bytes) of the string.
    */
    for (unsigned int i = 0; i < stringCount; ++i)
    {
        unsigned int length = readVarInt(fileBuffer, offset);

        char* strData = fileBuffer + offset;

        // new offset
        size_t strOffset = 0;
        int remainingLength = length; // store length into a new variable


        int stringIndex = 0;

        while (remainingLength > 0) {
            uint8_t strLength = read<uint8_t>(strData, strOffset);

            char* str = new char[strLength];
            memcpy(str, strData + strOffset, strLength);
            str[strLength] = '\0'; // NULL Terminate the string

            strOffset += strLength; // Move offset

            // We have read one string
            // remove it from the remaining ones to read
            remainingLength -= 1;


            strings.push_back(str); // Store string
            //std::cout << strings[stringIndex] << std::endl;

            ++stringIndex; // Increment


            // Move the main offset here as well
            offset += strLength;
        }

        // Move the main offset
        offset += length;
    }

    

    // Protos
    std::vector<Proto> protos;

    unsigned int protoCount = readVarInt(fileBuffer, offset);

    printf("Proto Count: %d\n", protoCount);
    for (unsigned int i = 0; i < protoCount; ++i)
    {
        //Proto* p_proto = (Proto*)malloc(sizeof(Proto));

        Proto proto;
        proto.bytecodeid = int(i);
        //std::cout << proto.bytecodeid << std::endl;

        proto.maxstacksize = read<uint8_t>(fileBuffer, offset);
        proto.numparams = read<uint8_t>(fileBuffer, offset);
        proto.nups = read<uint8_t>(fileBuffer, offset);
        proto.is_vararg = read<uint8_t>(fileBuffer, offset);

        uint8_t typesversion = 0;
        if (version >= 4)
        {
            proto.flags = read<uint8_t>(fileBuffer, offset);

            uint32_t typesize = readVarInt(fileBuffer, offset);

            if (typesize && typesversion == LBC_TYPE_VERSION)
            {
                uint8_t* types = (uint8_t*)fileBuffer + offset;

                _ASSERT(typesize == unsigned(2 + proto.numparams));
                _ASSERT(types[0] == LBC_TYPE_FUNCTION);
                _ASSERT(types[1] == proto.numparams);

                // I don't have luaM_newarray yet.
                //p_proto->typeinfo = luaM_newarray(L, typesize, uint8_t, p_proto->memcat);
                //memcpy(p_proto->typeinfo, types, typesize);
            }

            // Move offset
            offset += typesize;
        }

        // This is the part with the instructions.
        proto.sizecode = readVarInt(fileBuffer, offset);

        for (int j = 0; j < proto.sizecode; ++j) {
            //proto.code[j] = read<uint32_t>(fileBuffer, offset);
            // The entire instruction is being stored in this area.
            proto.code.push_back(read<uint32_t>(fileBuffer, offset));
        }

        // Some Constants stuff
        proto.sizek = readVarInt(fileBuffer, offset);

        for (int j = 0; j < proto.sizek; ++j)
        {
            auto constantType = read<uint8_t>(fileBuffer, offset);

            switch (constantType)
            {
            case LBC_CONSTANT_NIL:
                /*if (!FFlag::LuauLoadExceptionSafe)
                {
                    setnilvalue(&p->k[j]);
                }*/

                TValue nilV;
                nilV.tt = LUA_TNIL; // won't work but we need the push_back
                proto.k.push_back(nilV);
                break;

            case LBC_CONSTANT_BOOLEAN:
            {
                uint8_t v = read<uint8_t>(fileBuffer, offset);
                //setbvalue(&p->k[j], v);

                TValue boolV;
                boolV.tt = LUA_TBOOLEAN;
                boolV.value.b = (v);

                proto.k.push_back(boolV);
                break;
            }

            case LBC_CONSTANT_NUMBER:
            {
                double v = read<double>(fileBuffer, offset);
                //setnvalue(&p->k[j], v);

                TValue numV;
                numV.tt = LUA_TNUMBER;
                numV.value.n = (v);

                proto.k.push_back(numV);
                break;
            }

            case LBC_CONSTANT_VECTOR:
            {
                float x = read<float>(fileBuffer, offset);
                float y = read<float>(fileBuffer, offset);
                float z = read<float>(fileBuffer, offset);
                float w = read<float>(fileBuffer, offset);
                (void)w;
                //setvvalue(&p->k[j], x, y, z, w);

                TValue vecV;
                vecV.tt = LUA_TVECTOR;
                float* i_v = vecV.value.v;
                i_v[0] = (x);
                i_v[1] = (y);
                i_v[2] = (z);
                i_v[3] = (w);

                proto.k.push_back(vecV);
                break;
            }

            case LBC_CONSTANT_STRING:
            {
                char* v = readString(strings, fileBuffer, offset);
                //printf("%s\n", v);
                //setsvalue(L, &p->k[j], v);

                TValue strV;
                strV.tt = LUA_TSTRING;
                strV.value.str = v;

                proto.k.push_back(strV);
                break;
            }

            case LBC_CONSTANT_IMPORT:
            {
                uint32_t iid = read<uint32_t>(fileBuffer, offset);
                //resolveImportSafe(L, envt, p->k, iid);
                //setobj(L, &p->k[j], L->top - 1);
                //L->top--;

                TValue* tbl;

                int count = iid >> 30;

                int id0 = int(iid >> 20) & 1023;
                int id1 = int(iid >> 10) & 1023;
                int id2 = int(iid) & 1023;

                proto.k.push_back(proto.k[id0]); //proto.k[id0];

                if (count < 2)
                    break;
                proto.k.push_back(proto.k[id1]);

                if (count < 3)
                    break;
                proto.k.push_back(proto.k[id2]);

                break;
            }

            case LBC_CONSTANT_TABLE:
            {
                int keys = readVarInt(fileBuffer, offset);
                //Table* h = luaH_new(L, 0, keys);

                Table h;
                h.metatable = NULL;
                h.array = NULL;
                h.sizearray = 0;
                h.lastfree = 0;
                h.lsizenode = 0;
                h.readonly = 0;
                h.safeenv = 0;
                h.nodemask8 = 0;
                

                for (int i = 0; i < keys; ++i)
                {
                    int key = readVarInt(fileBuffer, offset);
                    //TValue* val = luaH_set(L, h, &p->k[key]);
                    //setnvalue(val, 0.0);

                    // The thing seems to determine what's in the table
                    // Maybe we don't need this
                    //proto.k[key];
                }
                //sethvalue(L, &p->k[j], h);

                TValue tblV;
                tblV.tt = LUA_TTABLE;
                tblV.value.tbl = &h;

                proto.k.push_back(tblV);
                break;
            }

            case LBC_CONSTANT_CLOSURE:
            {
                uint32_t fid = readVarInt(fileBuffer, offset);
                //Closure* cl = luaF_newLclosure(L, protos[fid]->nups, envt, protos[fid]);
                //cl->preload = (cl->nupvalues > 0);
                //setclvalue(L, &p->k[j], cl);

                TValue i_o;
                i_o.tt = LUA_TFUNCTION;

                Closure c;
                c.isC = 0;
                c.env = nullptr;
                c.nupvalues = protos[fid].nups;
                c.stacksize = proto.maxstacksize;
                c.preload = 0;
                c.l.p = &proto;

                c.pseudoFuncId = fid; // For Pseudo

                //i_o.value.gc = c;
                i_o.value.pseudoClosure = &c;

                proto.k.push_back(i_o);
                break;
            }

            default:
                //readerDumpCurrentLocation(fileBuffer, offset, 10);
                _ASSERT(!"Unexpected constant kind");
            }
        }


        // This is for subfunctions within protos.
        proto.sizep = readVarInt(fileBuffer, offset);

        for (int j = 0; j < proto.sizep; ++j)
        {
            uint32_t fid = readVarInt(fileBuffer, offset);
            //p->p[j] = protos[fid];
            //proto.p.push_back(&protos[fid]);
        }

        proto.linedefined = readVarInt(fileBuffer, offset);
        proto.debugname = (TString*)readString(strings, fileBuffer, offset);

        uint8_t lineinfo = read<uint8_t>(fileBuffer, offset);

        if (lineinfo)
        {
            int intervals = ((proto.sizecode - 1) >> proto.linegaplog2) + 1;
            int absoffset = (proto.sizecode + 3) & ~3;

            proto.linegaplog2 = read<uint8_t>(fileBuffer, offset);

            uint8_t lastoffset = 0;
            for (int j = 0; j < proto.sizecode; ++j)
            {
                lastoffset += read<uint8_t>(fileBuffer, offset);
                //p->lineinfo[j] = lastoffset;
                proto.lineinfo.push_back(lastoffset);
            }

            int lastline = 0;
            for (int j = 0; j < intervals; ++j)
            {
                lastline += read<int32_t>(fileBuffer, offset);
                //p->abslineinfo[j] = lastline;
                proto.abslineinfo.push_back(lastline);
            }
        }



        uint8_t debuginfo = read<uint8_t>(fileBuffer, offset);

        if (debuginfo)
        {
            // This is going to throw an error INCASE there is debuginfo
            // But just in case, I've exposed the reading functions, so the offset moves.

            proto.sizelocvars = readVarInt(fileBuffer, offset);

            for (int j = 0; j < proto.sizelocvars; ++j)
            {
                readString(strings, fileBuffer, offset);
                readVarInt(fileBuffer, offset);
                readVarInt(fileBuffer, offset);
                read<uint8_t>(fileBuffer, offset);
                /*proto.locvars[j].varname = readString(strings, fileBuffer, offset);
                proto.locvars[j].startpc = readVarInt(fileBuffer, offset);
                proto.locvars[j].endpc = readVarInt(fileBuffer, offset);
                proto.locvars[j].reg = read<uint8_t>(fileBuffer, offset);*/
            }

            

            proto.sizeupvalues = readVarInt(fileBuffer, offset);

            for (int j = 0; j < proto.sizeupvalues; ++j)
            {
                readString(strings, fileBuffer, offset);
                //proto.upvalues[j] = readString(strings, fileBuffer, offset);
            }
        }


        // Store the proto
        protos.push_back(proto);
    }



    // "main" proto is pushed to Lua stack
    uint32_t mainid = readVarInt(fileBuffer, offset);
    printf("Main Proto ID: %d\n", mainid);

    /* 
        Instructions

        The first byte of an instruction is the OpCode
    */

    Proto mainProto = protos[mainid];


    BytecodeParser parser;
    parser.version = version;
    parser.strings = strings;
    parser.protos = protos;
    parser.mainProtoID = mainid;

    // Begin parsing
    parser.parseBytecode();


    std::cout << parser.temp_out << std::endl;

    system("pause");
    return 0;
}