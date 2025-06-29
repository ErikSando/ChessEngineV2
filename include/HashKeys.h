#pragma once

#include "Board.h"
#include "Globals.h"

namespace ErikEngine {
    namespace HashKeys {
        extern U64 PieceKeys[12][64];
        extern U64 CastlingPermKeys[16];
        extern U64 EnPassantKeys[64];
        extern U64 SideKey;

        void Init();
        
        U64 GenerateHashKey(Board* board);
    }
}