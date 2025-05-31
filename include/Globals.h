#pragma once

#define NAME "ErikEngine++"
#define VERSION "0.0"

typedef unsigned long long U64;

#define GetBit(bitboard, bit) (((bitboard) >> (bit)) & (1ULL))
#define SetBit(bitboard, bit) ((bitboard) |= (1ULL << (bit)))
#define ClearBit(bitboard, bit) ((bitboard) &= ~(1ULL << (bit)))

enum Piece {
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK
};
