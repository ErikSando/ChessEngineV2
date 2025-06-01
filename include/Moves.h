#pragma once

#include <array>

#define MAX_MOVES 256

struct MoveList {
    std::array<int, MAX_MOVES> moves;
    int length = 0;
};

constexpr int FromSquareMask = 0x3F;
constexpr int ToSquareMask = 0xFC0;
constexpr int PieceMask = 0xF000;
constexpr int CapturedPieceMask = 0xF0000;
constexpr int PromotedPieceMask = 0xF00000;
constexpr int PawnStartFlag = 0x1000000;
constexpr int PromotionFlag = 0x2000000;
constexpr int CastlingFlag = 0x4000000;
constexpr int EnPassantFlag = 0x8000000;

// MOVE REPRESENTATION
// 000000  000000  0000   0000      0000      0              0             0            0
// from    to      piece  captured  promoted  is pawn start  is promotion  is castling  is en passant
inline int EncodeMove(int fromSquare, int toSquare, int piece, int captured, int promoted, int flags) {
    return (fromSquare) | (toSquare << 6) | (piece << 12) | (captured << 16) | (promoted << 20) | flags;
}

inline void AddMove(MoveList& list, int move) {
    list.moves[list.length++] = move;
}

inline int GetFromSquare(int move) {
    return move & FromSquareMask;
}

inline int GetToSquare(int move) {
    return (move & ToSquareMask) >> 6;
}

inline int GetPiece(int move) {
    return (move & PieceMask) >> 12;
}

inline int GetCapturedPiece(int move) {
    return (move & CapturedPieceMask) >> 16;
}

inline int GetPromotedPiece(int move) {
    return (move & PromotedPieceMask) >> 20;
}

inline int IsPawnStart(int move) {
    return (move & PawnStartFlag) >> 24;
}

inline int IsPromotion(int move) {
    return (move & PromotionFlag) >> 25;
}

inline int IsCastling(int move) {
    return (move & CastlingFlag) >> 26;
}

inline int IsEnPassant(int move) {
    return (move & EnPassantFlag) >> 27;
}