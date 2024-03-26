#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string.h>

#include "../Luau/Bytecode.h"
#include "../LuauPseudo/PseudoProto.h"
#include "PseudoUtils.hpp"
#include "../LuauPseudo/pseudo_lobject.h"

#include "PseudoInstruction.hpp"
#include "../LuauPseudo/PseudoRegister.hpp"


// This class stores the values and everything needed, for one file.
class BytecodeParser {
public:
	uint8_t version; // Luau Version

	std::vector<char*> strings; // Storage of strings
	std::vector<Proto> protos; // Storage of Protos

	uint32_t mainProtoID; // Main Proto ID


	std::map<int, PseudoRegister> registers;
	std::map<int, std::string> test_registers;
	std::map<int, std::string> test_registerValues;


	std::map<int, std::string> test_upval;
	std::map<int, std::string> test_upvalValues;


	int declaredVars = 0;
	int declaredUpvals = 0;
	int declaredFuncs = 0;

	uint32_t* p_currentProtoId = nullptr;
	PseudoInstruction prevInsn;

	std::string temp_out; // A test.

	std::map<int, std::string> temp_protoOuts;



	// Constructors
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


	std::string convertTValue_ToString(TValue value);

	// Temp
	void temp_createVariable(int regStoreId, std::string strVal, bool b_upVal = false);
};