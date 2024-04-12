#pragma once

#include <string>
#include "../Luau/Bytecode.h"
#include "../LuauPseudo/PseudoProto.h"

// This is used to identify what args a instruction has
namespace INSN_RegisterFlags {
	const int A = 1;
	const int B = (1 << 1);
	const int C = (1 << 2);
	const int D = (1 << 3);
	const int E = (1 << 4);

	const int ABC = (A | B | C);
	const int AB = (A | B);
	const int AC = (A | C);
	const int AD = (A | D);
	const int AE = (A | E);
};


class PseudoInstruction {
public:
	uint8_t opCode; // The op code
	std::string opCodeName; // The name of the op code.

	// The full instruction data
	uint32_t code;


	int RegisterFlags = 0; // Flags that define what the instruction uses.
	uint8_t A, B, C;
	int32_t D, E;


	// Whether this Instruction has AUX
	// This is important for skipping an instruction
	bool hasAUX;

	// AUX value itself
	int32_t aux;


	// Default Constructor for initialization
	PseudoInstruction() {
		opCode = NULL;
		A, B, C, D, E = NULL;

		hasAUX = false;
		aux = NULL;
	}


	PseudoInstruction(uint32_t code) : PseudoInstruction() {
		this->code = code;

		this->opCode = code & 0xff;

		this->A = LUAU_INSN_A(code);
		this->B = LUAU_INSN_B(code);
		this->C = LUAU_INSN_C(code);
		this->D = LUAU_INSN_D(code);
		this->E = LUAU_INSN_E(code);

		LuauOpcode enumOpCode = static_cast<LuauOpcode>(opCode);
		this->opCodeName.assign(PseudoUtils::getOpCodeName(enumOpCode));
	}


	// Method to set hasAUX bool
	void SetHasAUX(bool b) {
		this->hasAUX = b;
	}

	// Method to set register flags
	void SetRegisterFlags(int flag, bool hasAux = false) {
		this->RegisterFlags = flag;

		if (hasAux == true) {
			this->SetHasAUX(hasAux);
		}
	}


	// Generates a string used to print out the instruction
	std::string GetDumpString(Proto* protoRef = nullptr) {
		std::string outStr = "";
		int RegisterFlags = this->RegisterFlags; // Check Flags

		outStr.append(this->opCodeName);
		outStr.append(" ");

		if (RegisterFlags & INSN_RegisterFlags::A) {
			outStr.append("A" + std::to_string(this->A) + " ");
		}
		if (RegisterFlags & INSN_RegisterFlags::B) {
			outStr.append("B" + std::to_string(this->B) + " ");
		}
		if (RegisterFlags & INSN_RegisterFlags::C) {
			outStr.append("C" + std::to_string(this->C) + " ");
		}
		if (RegisterFlags & INSN_RegisterFlags::D) {
			outStr.append("D" + std::to_string(this->D) + " ");
		}
		if (RegisterFlags & INSN_RegisterFlags::E) {
			outStr.append("E" + std::to_string(this->E) + " ");
		}

		if (this->hasAUX == true) {
			outStr.append("\n\tAUX " + std::to_string(this->aux));

			// TODO: Do this differently without func arg?
			if (protoRef != nullptr) {
				//auto value = protoRef->k[this->aux].value.str;
				//outStr.append(value);
			}
		}

		return outStr;
	}
};