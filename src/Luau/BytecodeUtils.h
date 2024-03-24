#pragma once

#include "Bytecode.h"


namespace Luau
{

    inline int getOpLength(LuauOpcode op)
    {
        switch (op)
        {
        case LOP_GETGLOBAL:
        case LOP_SETGLOBAL:
        case LOP_GETIMPORT:
        case LOP_GETTABLEKS:
        case LOP_SETTABLEKS:
        case LOP_NAMECALL:
        case LOP_JUMPIFEQ:
        case LOP_JUMPIFLE:
        case LOP_JUMPIFLT:
        case LOP_JUMPIFNOTEQ:
        case LOP_JUMPIFNOTLE:
        case LOP_JUMPIFNOTLT:
        case LOP_NEWTABLE:
        case LOP_SETLIST:
        case LOP_FORGLOOP:
        case LOP_LOADKX:
        case LOP_FASTCALL2:
        case LOP_FASTCALL2K:
        case LOP_JUMPXEQKNIL:
        case LOP_JUMPXEQKB:
        case LOP_JUMPXEQKN:
        case LOP_JUMPXEQKS:
            return 2;

        default:
            return 1;
        }
    }

} // namespace Luau