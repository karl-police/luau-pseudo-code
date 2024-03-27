
#include "BytecodeParser.h"
#include "BytecodePreparation.h"

void BytecodeParser::parseInstruction(uint32_t code) {
    return;
}



// TODO: Maybe create a class that is a value that holds the declarer or something...

/*
    Creates a variable
    @params strVal - The code string used for writing out.
*/
void BytecodeParser::temp_createVariable(int regStoreId, std::string strVal, bool b_upVal) {
    auto found = this->test_registers.find(regStoreId);

    bool noWriteOut = false;

    // To determine whether to create an upvalue or not.
    /*if ((*this->p_currentProtoId) == this->mainProtoID) {
        b_upVal = true;
    }*/

    // If there's gonna ever be issues with Upvalues
    // Then we need to memorize upvalues similar to
    // how test_registers does it

    

    // If we find one, instead of writing "local v_" we do just "v_"
    if (found != this->test_registers.end()) {
        auto declareVar = this->test_registers[regStoreId];

        // If declared var is not empty
        if (declareVar != "") {
            this->temp_out.append(declareVar);
        }
        else {
            // If the declare name var is empty, then we assume indexing.
            /*this->temp_out.append("[");
            this->temp_out.append(strVal);
            this->temp_out.append("]");*/

            noWriteOut = true;
        }

        
    }
    else {
        auto declareCount = this->declaredVars;
        std::string varPrefix = "v";

        // Upvalues Override
        // Not really the cleanest way to write code.
        if (b_upVal == true) {
            declareCount = this->declaredUpvals;
            varPrefix = "u";
        }


        this->temp_out.append("local " + varPrefix + std::to_string(declareCount));
        this->test_registers[regStoreId] = varPrefix + std::to_string(declareCount);


        // increment declare counts of defined var type
        if (b_upVal == true) {
            //this->test_upval[declaredUpvals] = varPrefix + std::to_string(declareCount);
            this->declaredUpvals++;
        } else {
            this->test_upval[declaredVars] = varPrefix + std::to_string(declareCount);

            this->declaredVars++;
        }
    }

    // Store represented raw value as string
    this->test_registerValues[regStoreId] = strVal;


    if (noWriteOut == true) {
        return;
    }

    this->temp_out.append(" = ");
    this->temp_out.append(strVal);
    this->temp_out.append("\n");
};



std::string BytecodeParser::convertTValue_ToString(TValue constant) {
    switch (constant.tt) {
    case(LUA_TNIL):
        return "nil";
        break;
    case(LUA_TNUMBER):
        return std::to_string(constant.value.n);
        break;
    case(LUA_TBOOLEAN): {
        bool boolVal = (bool)constant.value.b;
        std::string str = "false";

        if (boolVal == true)
            str = "true";

        return "str";
        break;

    }
    case(LUA_TSTRING): {
        std::string str = "\"";
        str.append(constant.value.str);
        str.append("\"");

        return str;
        break;
    }
    default:
        printf("Conversion skipped: %d\n", constant.tt);
        break;
    }
};



// Parses a Proto or aka. Function or something
void BytecodeParser::parseProto(Proto proto) {
    this->p_currentProtoId = reinterpret_cast<uint32_t*>(&proto.bytecodeid);

    for (int i = 0; i < proto.sizecode; i++) {
        uint32_t code = proto.code[i];

        uint8_t OpCode = code & 0xff;

        char* str = nullptr;

        PseudoInstruction insn(code);

        LuauOpcode enumOpCode = static_cast<LuauOpcode>(OpCode);

        switch (enumOpCode) {
        case LOP_NOP:
        case LOP_BREAK:
            break;

        case LOP_LOADNIL:
            insn.SetRegisterFlags(INSN_RegisterFlags::A);

            BytecodeParser::temp_createVariable(insn.A, "nil");

            break;

            // Load Boolean
        case LOP_LOADB: {
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);

            bool boolVal = (bool)insn.B;
            std::string str = "false";

            if (boolVal == true)
                str = "true";

            // C is used for something specific here, dunno know

            BytecodeParser::temp_createVariable(insn.A, str);

            break;
        }

                      // Load Number
        case LOP_LOADN: {
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);

            int32_t numValue = insn.D;

            BytecodeParser::temp_createVariable(insn.A, std::to_string(numValue));

            break;
        }

        case LOP_LOADK: {
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);

            int32_t constantIndex = insn.D;

            auto constant = proto.k[constantIndex];

            auto representation = BytecodeParser::convertTValue_ToString(constant);

            BytecodeParser::temp_createVariable(insn.A, representation);

            break;
        }

        case LOP_MOVE: {
            insn.SetRegisterFlags(INSN_RegisterFlags::AB);

            auto targetValue = this->test_registers[insn.A];
            auto targetDeclarer = this->test_registers[insn.B];

            this->temp_out.append(targetDeclarer);
            this->temp_out.append(" = ");
            this->temp_out.append(targetValue);
            this->temp_out.append("\n");

            break;
        }

        case LOP_GETGLOBAL:
            insn.SetRegisterFlags(INSN_RegisterFlags::AC, true);
            printf("SKIPPED LOP_GETGLOBAL");
            break;

        case LOP_SETGLOBAL:
            insn.SetRegisterFlags(INSN_RegisterFlags::AC, true);

            printf("SKIPPED LOP_SETGLOBAL");
            break;

        case LOP_GETUPVAL: {
            insn.SetRegisterFlags(INSN_RegisterFlags::AB);

            auto upValueDeclareName = this->test_upval[insn.B];

            BytecodeParser::temp_createVariable(insn.A, upValueDeclareName, true);

            break;
        }

        case LOP_SETUPVAL: {
            insn.SetRegisterFlags(INSN_RegisterFlags::AB);
            // Verify this

            auto value = this->test_upval[insn.A];

            BytecodeParser::temp_createVariable(insn.B, value, true);

            break;
        }

        /*case LOP_CLOSEUPVALS:
            insn.SetRegisterFlags(INSN_RegisterFlags::A);
            break;*/

        case LOP_GETIMPORT: {
            insn.SetRegisterFlags(INSN_RegisterFlags::AD, true);
            // This has aux. AUX gets skipped after the switch case.
            insn.aux = proto.code[i + 1];

            auto iid = (uint32_t)insn.aux;

            unsigned int count = iid >> 30;

            std::string path;

            if (count != 0) {
                for (int i = 0; i < count; i++) {
                    int constId = int(iid >> (20 - i * 10)) & 1023;

                    auto constant = proto.k[constId];

                    path.append(constant.value.str);

                    if (i != count - 1) {
                        path.append(".");
                    }
                }
            }
            else {
                path = proto.k[insn.D].value.str;
            }

            //auto representation = BytecodeParser::convertTValue_ToString(constant);
            auto representation = path;

            BytecodeParser::temp_createVariable(insn.A, representation);

            break;
        }

        /*case LOP_GETTABLE:
        case LOP_SETTABLE:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);
            break;*/

        case LOP_GETTABLEKS: {
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC, true);
            insn.aux = proto.code[i + 1];

            std::string path;

            //auto targetTableToIndex = proto.k[insn.B];
            auto tblIndex = proto.k[insn.aux];

            bool newVar = false;

            /*
                This is how it's going to work.
                If the previous variable existed, we write to it
            */
            auto declaredTable = this->test_registers[insn.B];

            if (declaredTable != "") {
                if (this->temp_out.back() == '\n') {
                    this->temp_out.pop_back();
                }
            }
            else {
                auto idk = this->test_registers[insn.B];
                path.append(idk);
            }

            path.append(".");

            if (tblIndex.tt == LUA_TSTRING) {
                path.append(tblIndex.value.str);
            }
            else {
                path.append(BytecodeParser::convertTValue_ToString(tblIndex));
            }

            path.append("\n");


            this->temp_out.append(path);

            // the next part is that we need to merge the values to the register
            //this->test_registerValues[insn.A] = declaredTable;

            BytecodeParser::temp_createVariable(insn.A, declaredTable);

            break;
        }
        case LOP_SETTABLEKS: {
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC, true);
            insn.aux = proto.code[i + 1];

            std::string output;

            std::string valueToSet = this->test_registers[insn.A];
            std::string tableToAccess = this->test_registers[insn.B];

            auto tableIndex = proto.k[insn.aux].value.str;

            output.append(tableToAccess);
            output.append(".");
            output.append(tableIndex);

            output.append(" = ");
            output.append(valueToSet);
            output.append("\n");


            // Output
            this->temp_out.append(output);

            break;
        }

        /*case LOP_GETTABLEN:
        case LOP_SETTABLEN:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);
            break;

        case LOP_NEWCLOSURE:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);
            break;*/

        case LOP_NAMECALL: {
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC, true);
            insn.aux = proto.code[i + 1];

            auto registerToSaveTo = this->test_registers[insn.A];
            auto targetRegister = this->test_registers[insn.B];

            std::string strToCall = proto.k[insn.aux].value.str;

            std::string output;

            output.append(targetRegister);
            output.append(":" + strToCall);

            // If we're calling on an available table
            // merge it into the same line
            if (registerToSaveTo == targetRegister) {
                this->temp_out.append(registerToSaveTo);
                this->temp_out.append(" = ");
                this->temp_out.append(output);
            }

            break;
        }
        case LOP_CALL: {
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);
            bool isSelf = false;


            auto prevInsn = this->prevInsn;

            std::string callOut;

            auto test = this->test_registers[insn.A];

            auto argCount = (insn.B - 1);
            auto resCount = (insn.C - 1);

            if (test == "")
                test = this->test_registerValues[insn.A];

            // If the previous one was a NAMECALL, we don't need the name
            if (prevInsn.opCode == LOP_NAMECALL) {
                // NAMECALL's are self
                isSelf = true;
            }
            else {
                callOut.append(test);
            }
            callOut.append("(");

            auto startRegIndx = insn.A+1;

            for (int i = 0; i < argCount; i++) {
                std::string argStr = this->test_registers[startRegIndx + i];

                if (isSelf == true && i == 0) {
                    continue; // SKIP
                }

                if (argStr == "")
                    argStr = this->test_registerValues[startRegIndx + i];

                callOut.append(argStr);

                if (i != argCount-1)
                    callOut.append(",");
            }

            callOut.append(")\n"); // Close call bracket


            // When a call gets made, it seems to clear some stuff out...
            auto memorizedRegisterSize = this->test_registers.size();
            for (int i = 0; i < memorizedRegisterSize; i++) {
                //this->test_registers.erase(i);
            }

            /*int selfOffset = isSelf ? 1 : 0;
            for (int i = 1 + selfOffset; i < argCount; i++) {
                this->test_registers.erase(insn.A + i);
            }*/

            this->temp_out.append(callOut);

            break;
        }

        /*case LOP_RETURN: {
            insn.SetRegisterFlags(INSN_RegisterFlags::AB);
            // TODO fix this

            auto returnVal = this->test_registers[insn.B];

            this->temp_out.append("return ");
            this->temp_out.append(returnVal);

            break;
        }*/

        /*case LOP_JUMP:
        case LOP_JUMPBACK:
            insn.SetRegisterFlags(INSN_RegisterFlags::D);
            break;

        case LOP_JUMPIF:
        case LOP_JUMPIFNOT:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);
            break;*/

        case LOP_JUMPIFEQ:
        case LOP_JUMPIFLE:
        case LOP_JUMPIFLT:
        case LOP_JUMPIFNOTEQ:
        case LOP_JUMPIFNOTLE:
        case LOP_JUMPIFNOTLT:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD, true);

            printf("SKIPPED LOP_JUMPIFEQ AND A LOT\n");
            break;

        /*case LOP_ADD:
        case LOP_SUB:
        case LOP_MUL:
        case LOP_DIV:
        case LOP_MOD:
        case LOP_POW:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);
            break;

        case LOP_ADDK:
        case LOP_SUBK:
        case LOP_MULK:
        case LOP_DIVK:
        case LOP_MODK:
        case LOP_POWK:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);
            break;

        case LOP_AND:
        case LOP_OR:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);
            break;

        case LOP_ANDK:
        case LOP_ORK:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);
            break;

        case LOP_CONCAT:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);
            break;

        case LOP_NOT:
        case LOP_MINUS:
        case LOP_LENGTH:
            insn.SetRegisterFlags(INSN_RegisterFlags::AB);
            break;*/

        case LOP_NEWTABLE: {
            insn.SetRegisterFlags(INSN_RegisterFlags::AB, true);
            insn.aux = proto.code[i + 1];

            auto tableSize = insn.B;
            auto auxArraySize = insn.aux;

            BytecodeParser::temp_createVariable(insn.A, "{}");

            break;
        }

        case LOP_DUPTABLE:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);

            BytecodeParser::temp_createVariable(insn.A, "{}");

            break;

        case LOP_SETLIST:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC, true);

            printf("SKIPPED LOP_SETLIST\n");

            break;

        /*case LOP_FORNLOOP:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);
            break;*/

        case LOP_FORGLOOP:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD, true);

            printf("SKIPPED LOP_FORGLOOP\n");

            break;

        /*case LOP_FORGPREP_INEXT:
            insn.SetRegisterFlags(INSN_RegisterFlags::A);
            break;

        case LOP_DEP_FORGLOOP_INEXT:
            // Removed in v3
            break;

        case LOP_FORGPREP_NEXT:
            insn.SetRegisterFlags(INSN_RegisterFlags::A);
            break;

        case LOP_NATIVECALL:
            // Pseudo instruction
            break;

        case LOP_GETVARARGS:
            insn.SetRegisterFlags(INSN_RegisterFlags::AB);
            break;*/

        case LOP_DUPCLOSURE: {
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);
            
            auto targetClosure = proto.k[insn.D];
            
            auto closureProtoId = targetClosure.value.pseudoClosure->pseudoFuncId;
            auto protoClosure = protos[closureProtoId];

            std::string funcName = "func";
            funcName.append(std::to_string(closureProtoId));

            this->temp_out.append("function " + funcName + "()\n");
            
            //BytecodeParser::parseProto(protos[closureProtoId]);

            this->temp_out.append("end\n");


            BytecodeParser::temp_createVariable(insn.A, funcName);

            break;
        }

        case LOP_PREPVARARGS:
            insn.SetRegisterFlags(INSN_RegisterFlags::A);

            // A is the Target Register
            // Apparently this is unused

            printf("PREPVARARGS is unused apparently.\n");

            break;

        case LOP_LOADKX:
            insn.SetRegisterFlags(INSN_RegisterFlags::A, true);

            printf("SKIPPED LOP_LOADKX\n");

            break;

        /*case LOP_JUMPX:
            insn.SetRegisterFlags(INSN_RegisterFlags::E);
            break;

        case LOP_FASTCALL:
            insn.SetRegisterFlags(INSN_RegisterFlags::AC);
            break;

        case LOP_COVERAGE:
            insn.SetRegisterFlags(INSN_RegisterFlags::E);
            break;

        case LOP_CAPTURE:
            insn.SetRegisterFlags(INSN_RegisterFlags::AB);
            break;

        case LOP_SUBRK:
        case LOP_DIVRK:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);
            break;

        case LOP_FASTCALL1:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);
            break;*/

        case LOP_FASTCALL2:
        case LOP_FASTCALL2K:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC, true);

            printf("SKIPPED LOP_FASTCALL2 LOP_FASTCALL2K\n");

            break;

        /*case LOP_FORGPREP:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);
            break;*/

        case LOP_JUMPXEQKNIL:
        case LOP_JUMPXEQKB:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD, true);

            printf("SKIPPED LOP_JUMPXEQKNIL LOP_JUMPXEQKB\n");

            break;

        case LOP_JUMPXEQKN:
        case LOP_JUMPXEQKS:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD, true);

            printf("SKIPPED LOP_JUMPXEQKN LOP_JUMPXEQKS\n");

            break;

        /*case LOP_IDIV:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);
            break;

        case LOP_IDIVK:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);
            break;*/

        case LOP__COUNT:
            throw("What in the bytecode is this...");
            break;


        default:
            printf("Skipped: %s\n", PseudoUtils::getOpCodeName(enumOpCode));
            break;
        }


        // AUX handling
        if (insn.hasAUX == true) {
            insn.aux = proto.code[i + 1];

            // Skip next instruction
            // Because AUX was read, and AUX lays in the next instruction
            i++;
        }


        // Dump out the instruction
        std::cout << insn.GetDumpString() << std::endl;

        this->prevInsn = insn;
    }
}



/*
    The function that starts the parsing process.
*/
void BytecodeParser::parseBytecode() {
    //BytecodePreparation::init(); // Prepares stuff


	// This is where the parsing begins.

	// We start from the Main Proto

	auto mainId = this->mainProtoID;

	Proto mainProto = protos[mainId];

    BytecodeParser::parseProto(mainProto);

    for (int i = 0; i < protos.size(); i++) {
        if (i == mainId) {
            continue; // Skip
        }

        BytecodeParser::parseProto(protos[i]);
    }
}