#pragma once

#include "Board.h"

#ifdef NDEBUG
    #define debug(x)
#else
    #define debug(x) x
#endif

namespace Debug {
    void CheckBoardValid(Board& board);
    bool CheckBoardValidQuiet(Board& board);
}