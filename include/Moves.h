#pragma once

#include <array>

#define MAX_MOVES 256

struct MoveList {
    std::array<int, MAX_MOVES> moves;
    int length = 0;
};

// constexpr int FromSquareMask = 0x3F;
// constexpr int ToSquareMask = 0xFC0;
// constexpr int PieceMask = 0xF000;
// constexpr int CapturedPieceMask = 0xF0000;
// constexpr int PromotedPieceMask = 0xF00000;
// constexpr int CaptureFlag = 0x1000000;
// constexpr int PawnStartFlag = 0x2000000;
// constexpr int CastlingFlag = 0x4000000;
// constexpr int EnPassantFlag = 0x8000000;

enum MoveFlags {
    CaptureFlag = 0x1000000,
    PawnStartFlag = 0x2000000,
    CastlingFlag = 0x4000000,
    EnPassantFlag = 0x8000000
};

// MOVE REPRESENTATION
// 000000  000000  0000   0000      0000      0           0              0            0
// from    to      piece  captured  promoted  is capture  is pawn start  is castling  is en passant
inline int EncodeMove(int fromSquare, int toSquare, int moved, int captured, int promoted, int flag) {
    return (fromSquare) | (toSquare << 6) | (moved << 12) | (captured << 16) | (promoted << 20) | flag;
}

inline void AddMove(MoveList& list, int move) {
    list.moves[list.length++] = move;
}

inline int GetFromSquare(int move) {
    return move & 0x3F;
}

inline int GetToSquare(int move) {
    return (move & 0xFC0) >> 6;
}

inline int GetMovedPiece(int move) {
    return (move & 0xF000) >> 12;
}

inline int GetCapturedPiece(int move) {
    return (move & 0xF0000) >> 16;
}

inline int GetPromotedPiece(int move) {
    return (move & 0xF00000) >> 20;
}

inline int GetFlag(int move) {
    return move & 0xF000000;
}

// inline int IsCapture(int move) {
//     return move & CaptureFlag;
// }

// inline int IsPawnStart(int move) {
//     return move & PawnStartFlag;
// }

// inline int IsCastling(int move) {
//     return move & CastlingFlag;
// }

// inline int IsEnPassant(int move) {
//     return move & EnPassantFlag;
// }