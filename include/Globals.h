#pragma once

constexpr const char* NAME = "ErikEngine";
constexpr const char* VERSION = "2.0";

typedef unsigned long long U64;

inline bool IsBitSet(U64 bitboard, int bit) {
    return bitboard & (1ULL << bit);
}

inline int GetBit(U64 bitboard, int bit) {
    return (bitboard >> bit) & 1ULL;
}

inline void SetBit(U64& bitboard, int bit) {
    bitboard |= (1ULL << bit);
}

inline void ClearBit(U64& bitboard, int bit) {
    bitboard &= ~(1ULL << bit);
}

inline int GetSquare(int file, int rank) {
    return file + rank * 8;
}

inline int GetFile(int square) {
    return square & 7;
}

inline int GetRank(int square) {
    return square >> 3;
}

extern int ToSquare(const char* square);
extern const char* ToSquareString(int square);
extern const char* ToMoveString(int move);
extern void PrintBitboard(U64 bitboard);