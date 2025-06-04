#pragma once

#include "Board.h"

// Based on https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function

namespace Evaluation {
    void Init();
    int Evaluate(Board& board);

    inline int MirrorSquare(int square) {
        return square ^ 56;
    }

    constexpr int PhaseVal[6] = { 0, 1, 1, 2, 4, 0 };
    constexpr int TotalPhase = 2 * (PhaseVal[0] * 8 + 2 * (PhaseVal[1] + PhaseVal[2] + PhaseVal[3]) + PhaseVal[4]);

    extern int PieceMgTables[2][64];
    extern int PieceEgTables[2][64];

    constexpr int PawnMgTable[64] = {

    };

    constexpr int PawnEgTable[64] = {
        
    };

    constexpr int KnightMgTable[64] = {

    };

    constexpr int KnightEgTable[64] = {
        
    };

    constexpr int BishopMgTable[64] = {

    };

    constexpr int BishopEgTable[64] = {
        
    };

    constexpr int RookMgTable[64] = {

    };

    constexpr int RookEgTable[64] = {
        
    };

    constexpr int QueenMgTable[64] = {

    };

    constexpr int QueenEgTable[64] = {
        
    };

    constexpr int KingMgTable[64] = {

    };

    constexpr int KingEgTable[64] = {
        
    };
}