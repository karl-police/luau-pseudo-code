#pragma once

#include <map>

class PseudoOpCodeInfo {
public:
	const char* opCodeName = nullptr;


	PseudoOpCodeInfo() {}

	PseudoOpCodeInfo(const char* opCodeName) : opCodeName(opCodeName) {
		
	}
};


std::map<LuauOpcode, PseudoOpCodeInfo> LuauOpcodeMap;


namespace BytecodePreparation {
	void init() {
		auto Start = LOP_NOP;
		auto Last = LOP__COUNT; // This enum here, is actually used for this purpose

		for (int i = Start; i != Last; i++)
		{
			LuauOpcode enumValue = static_cast<LuauOpcode>(i);

			switch (enumValue) {
			case LOP_NOP:
				LuauOpcodeMap[LOP_NOP] = PseudoOpCodeInfo("NOP");
				break;
			case LOP_BREAK:
				LuauOpcodeMap[LOP_BREAK] = PseudoOpCodeInfo("BREAK");

			case LOP_LOADNIL:
				LuauOpcodeMap[LOP_LOADNIL] = PseudoOpCodeInfo("LOADNIL");
				break;
			}
		}
	}
}