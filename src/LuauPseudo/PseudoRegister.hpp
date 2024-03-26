#pragma once

#pragma once

#include <string>
#include "../Luau/Bytecode.h"
#include "../LuauPseudo/PseudoProto.h"


class BaseTemplateValue {
	virtual std::string ToString();
};

class TableValue : BaseTemplateValue {

};


class PseudoRegisterValue {
public:

	// The holding declarer is the variable that declared the value
	std::string holdingDeclarer;

};


class Temp_TableValue;

class Temp_PseudoRegisterValue {
public:

	// The holding declarer is the variable that declared the value
	std::string* holdingDeclarer = nullptr;

	std::string* strValue = nullptr;
	int* intValue = nullptr;
	Temp_TableValue* tableValue = nullptr;
};

class Temp_TableValue {
public:
	std::map<int, Temp_PseudoRegisterValue> integerIndexed;

};









class PseudoRegister {
public:

	PseudoRegisterValue RegisterValue; // The current value of a register.

	// The current string indentifyable value of a register.
	const char* strValue;


	// Constructor
	PseudoRegister() {};
};