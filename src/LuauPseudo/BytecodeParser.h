#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>

#include "../Luau/Bytecode.h"
#include "../LuauPseudo/PseudoProto.h"
#include "PseudoUtils.hpp"
#include "../LuauPseudo/pseudo_lobject.h"

#include "PseudoInstruction.hpp"


// This class stores the values and everything needed, for one file.
class BytecodeParser {
public:
	uint8_t version; // Luau Version

	std::vector<char*> strings; // Storage of strings
	std::vector<Proto> protos; // Storage of Protos

	uint32_t mainProtoID; // Main Proto ID


	BytecodeParser() {}

	BytecodeParser(uint8_t version, std::vector<char*> strings, std::vector<Proto> protos, uint32_t mainProtoID)
		: version(version), strings(strings), protos(protos), mainProtoID(mainProtoID)
	{
	}




	// Methods
	// This begins the process of parsing the Bytecode.
	void parseBytecode();

	void parseProto(Proto proto);

	/*
	*	This prepares a PseudoInstruction Object based on the instruction
	*	@param opCode - The OpCode value.
	*/
	void parseInstruction(uint32_t code);
};