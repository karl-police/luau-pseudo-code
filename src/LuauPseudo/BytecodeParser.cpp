
#include "BytecodeParser.h"
#include "BytecodePreparation.h"

void BytecodeParser::parseInstruction(uint32_t code) {
    return;
}


/*
    Creates a variable
*/
void BytecodeParser::temp_createVariable(int regStoreId, std::string strVal) {
    auto found = this->test_registers.find(regStoreId);

    if (found != this->test_registers.end()) {
        this->temp_out.append(this->test_registers[regStoreId]);
    }
    else {
        // Create it now
        this->temp_out.append("local v" + std::to_string(this->declaredVars++));
        this->test_registers[regStoreId] = "v" + std::to_string(this->declaredVars);
    }

    this->temp_out.append(" = ");
    this->temp_out.append(strVal);
    this->temp_out.append("\n");
};


// Parses a Proto or aka. Function or something
void BytecodeParser::parseProto(Proto proto) {
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

        case LOP_LOADB: {
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);

            bool boolVal = (bool)insn.B;
            std::string str = "false";

            if (boolVal == true)
                this->temp_out.append("true");

            // C is used for something specific here, dunno know

            BytecodeParser::temp_createVariable(insn.A, str);

            break;
        }

        /*case LOP_LOADN:
        case LOP_LOADK:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);
            break;


        case LOP_MOVE:
            insn.SetRegisterFlags(INSN_RegisterFlags::AB);
            break;

        case LOP_GETGLOBAL:
        case LOP_SETGLOBAL:
            insn.SetRegisterFlags(INSN_RegisterFlags::AC);
            insn.SetHasAUX(true);
            break;

        case LOP_GETUPVAL:
        case LOP_SETUPVAL:
            insn.SetRegisterFlags(INSN_RegisterFlags::AB);
            break;

        case LOP_CLOSEUPVALS:
            insn.SetRegisterFlags(INSN_RegisterFlags::A);
            break;

        case LOP_GETIMPORT:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);
            insn.SetHasAUX(true);
            break;

        case LOP_GETTABLE:
        case LOP_SETTABLE:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);
            break;

        case LOP_GETTABLEKS:
        case LOP_SETTABLEKS:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC, true);
            break;

        case LOP_GETTABLEN:
        case LOP_SETTABLEN:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);
            break;

        case LOP_NEWCLOSURE:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);
            break;

        case LOP_NAMECALL:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC, true);
            break;

        case LOP_CALL:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC);
            break;

        case LOP_RETURN:
            insn.SetRegisterFlags(INSN_RegisterFlags::AB);
            break;

        case LOP_JUMP:
        case LOP_JUMPBACK:
            insn.SetRegisterFlags(INSN_RegisterFlags::D);
            break;

        case LOP_JUMPIF:
        case LOP_JUMPIFNOT:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);
            break;

        case LOP_JUMPIFEQ:
        case LOP_JUMPIFLE:
        case LOP_JUMPIFLT:
        case LOP_JUMPIFNOTEQ:
        case LOP_JUMPIFNOTLE:
        case LOP_JUMPIFNOTLT:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD, true);
            break;

        case LOP_ADD:
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
            break;

        case LOP_NEWTABLE:
            insn.SetRegisterFlags(INSN_RegisterFlags::AB, true);
            break;

        case LOP_DUPTABLE:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);
            break;

        case LOP_SETLIST:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC, true);
            break;

        case LOP_FORNLOOP:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);
            break;

        case LOP_FORGLOOP:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD, true);
            break;

        case LOP_FORGPREP_INEXT:
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
            break;

        case LOP_DUPCLOSURE:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);
            break;*/

        case LOP_PREPVARARGS:
            insn.SetRegisterFlags(INSN_RegisterFlags::A);

            // A is the Target Register
            // Apparently this is unused

            printf("PREPVARARGS is unused apparently.\n");

            break;

        /*case LOP_LOADKX:
            insn.SetRegisterFlags(INSN_RegisterFlags::A, true);
            break;

        case LOP_JUMPX:
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
            break;

        case LOP_FASTCALL2:
        case LOP_FASTCALL2K:
            insn.SetRegisterFlags(INSN_RegisterFlags::ABC, true);
            break;

        case LOP_FORGPREP:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD);
            break;

        case LOP_JUMPXEQKNIL:
        case LOP_JUMPXEQKB:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD, true);
            break;

        case LOP_JUMPXEQKN:
        case LOP_JUMPXEQKS:
            insn.SetRegisterFlags(INSN_RegisterFlags::AD, true);
            break;

        case LOP_IDIV:
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
    }
}



/*
    The function that starts the parsing process.
*/
void BytecodeParser::parseBytecode() {
    //BytecodePreparation::init(); // Prepares stuff


	// This is where the parsing begins.

	// We start from the Main Proto

	auto mainId = mainProtoID;

	Proto mainProto = protos[mainId];

    BytecodeParser::parseProto(mainProto);

    for (int i = 0; i < protos.size(); i++) {
        if (i == mainId) {
            continue; // Skip
        }

        BytecodeParser::parseProto(protos[i]);
    }
}