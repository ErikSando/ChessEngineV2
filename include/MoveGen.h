#pragma once

#include "Board.h"

constexpr int MAX_MOVES = 256;

struct Move {
    int move;
    int score;
};

struct MoveList {
    Move moves[MAX_MOVES];
    int length = 0;
};

// MOVE REPRESENTATION
// 000000  000000  0000   0000      0000      0           0              0            0
// from    to      piece  captured  promoted  is capture  is pawn start  is castling  is en passant
inline int EncodeMove(int fromSquare, int toSquare, int moved, int captured, int promoted, int flag) {
    return (fromSquare) | (toSquare << 6) | (moved << 12) | (captured << 16) | (promoted << 20) | flag;
}

inline void AddMove(MoveList& list, int score, int move) {
    list.moves[list.length].move = move;
    list.moves[list.length++].score = score;
}

namespace MoveGen {
    void GenerateMoves(const Board& board, MoveList& list);
    void GenerateCaptures(const Board& board, MoveList& list);
}