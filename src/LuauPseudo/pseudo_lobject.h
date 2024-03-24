#pragma once

#define CommonHeader \
     uint8_t tt; uint8_t marked; uint8_t memcat;

typedef uint32_t Instruction;

/*
** Union of all collectible objects
*/
typedef union GCObject GCObject;

/*
** Union of all Lua values
*/
typedef union
{
    GCObject* gc;
    char* str; // I created this to subsitude GCObject
    void* p;
    double n;
    int b;
    float v[2]; // v[0], v[1] live here; v[2] lives in TValue::extra
    struct Table* tbl;
} Value;

typedef struct lua_TValue
{
    Value value;
    //int extra[LUA_EXTRA_SIZE];
    int extra;
    int tt;
} TValue;

typedef struct TString
{
    CommonHeader;
    // 1 byte padding

    int16_t atom;

    // 2 byte padding

    TString* next; // next string in the hash table bucket

    unsigned int hash;
    unsigned int len;

    char data[1]; // string data is allocated right after the header
} TString;

typedef struct Table
{
    CommonHeader;


    uint8_t tmcache;    // 1<<p means tagmethod(p) is not present
    uint8_t readonly;   // sandboxing feature to prohibit writes to table
    uint8_t safeenv;    // environment doesn't share globals with other scripts
    uint8_t lsizenode;  // log2 of size of `node' array
    uint8_t nodemask8; // (1<<lsizenode)-1, truncated to 8 bits

    int sizearray; // size of `array' array
    union
    {
        int lastfree;  // any free position is before this position
        int aboundary; // negated 'boundary' of `array' array; iff aboundary < 0
    };


    struct Table* metatable;
    TValue* array;  // array part
    //LuaNode* node;
    GCObject* gclist;
} Table;

typedef struct LocVar
{
    TString* varname;
    int startpc; // first point where variable is active
    int endpc;   // first point where variable is dead
    uint8_t reg; // register slot, relative to base, where variable is stored
} LocVar;

/*
** Function Prototypes
*/
typedef struct Proto
{
    CommonHeader;


    uint8_t nups; // number of upvalues
    uint8_t numparams;
    uint8_t is_vararg;
    uint8_t maxstacksize;
    uint8_t flags;


    std::vector<TValue> k;              // constants used by the function
    std::vector<uint32_t> code;      // function bytecode
    //struct Proto** p;       // functions defined inside the function
    std::vector<struct Proto*> p;
    const Instruction* codeentry;

    void* execdata;
    uintptr_t exectarget;


    std::vector<uint8_t> lineinfo;      // for each instruction, line number as a delta from baseline
    std::vector<int> abslineinfo;       // baseline line info, one entry for each 1<<linegaplog2 instructions; allocated after lineinfo
    std::vector<struct LocVar> locvars; // information about local variables
    TString** upvalues;     // upvalue names
    TString* source;

    TString* debugname;
    uint8_t* debuginsn; // a copy of code[] array with just opcodes

    uint8_t* typeinfo;

    void* userdata;

    GCObject* gclist;


    int sizecode;
    int sizep;
    int sizelocvars;
    int sizeupvalues;
    int sizek;
    int sizelineinfo;
    int linegaplog2;
    int linedefined;
    int bytecodeid;
} Proto;