#pragma once

#include <cstdlib>

namespace ErikEngine {
    using U64 = unsigned long long;
    // using Bitboard = U64; // to be clear with what variables are used for

    constexpr const char* ENGINE_NAME = "ErikEngine";
    constexpr const char* ENGINE_AUTHOR = "Erik";
    constexpr const char* ENGINE_VERSION = "2.0";

    #if defined(__GNUC__) || defined(__clang__)

    inline int CountBits(U64 bitboard) {
        return __builtin_popcountll(bitboard);
    }

    inline int FirstBitIndex(U64 bitboard) {
        return __builtin_ctzll(bitboard);
    }

    #elif defined(_MSC_VER) && defined(_M_X64)

    #include <intrin.h>
    #pragma intrinsic(__popcnt64)

    inline int CountBits(U64 bitboard) {
        return static_cast<int>(__popcnt64(bitboard));
    }

    inline int FirstBitIndex(U64 bitboard) {
        unsigned long index;
        _BitScanForward64(&index, bitboard);
        return static_cast<int>(index);
    }

    #else

    inline int CountBits(U64 bitboard) {
        int n = 0;
        for (; bitboard; n++, bitboard &= (bitboard - 1));
        return n;
    }

    constexpr int BitTable[64] = {
        63, 30, 3, 32, 25, 41, 22, 33,
        15, 50, 42, 13, 11, 53, 19, 34,
        61, 29, 2, 51, 21, 43, 45, 10,
        18, 47, 1, 54, 9, 57, 0, 35,
        62, 31, 40, 4, 49, 5, 52, 26,
        60, 6, 23, 44, 46, 27, 56, 16,
        7, 39, 48, 24, 59, 14, 12, 55,
        38, 28, 58, 20, 37, 17, 36, 8
    };

    inline int FirstBitIndex(U64 bitboard) {
        U64 b = bitboard ^ (bitboard - 1);
        unsigned int fold = (unsigned) ((b & 0xFFFFFFFF) ^ (b >> 32));
        return BitTable[(fold * 0x783A9B23) >> 26];
    }

    #endif

    inline int PopFirstBit(U64& bitboard) {
        int index = FirstBitIndex(bitboard);
        bitboard &= (bitboard - 1);
        return index;
    }

    inline bool IsBitSet(U64 bitboard, int bit) {
        return (bitboard & (1ULL << bit)) != 0ULL;
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

    inline U64 RandU64() {
        U64 u1, u2, u3, u4;

        #ifdef _WIN32

        u1 = (U64) (rand()) & 0xFFFF;
        u2 = (U64) (rand()) & 0xFFFF;
        u3 = (U64) (rand()) & 0xFFFF;
        u4 = (U64) (rand()) & 0xFFFF;

        #else

        u1 = (U64) (random()) & 0xFFFF;
        u2 = (U64) (random()) & 0xFFFF;
        u3 = (U64) (random()) & 0xFFFF;
        u4 = (U64) (random()) & 0xFFFF;

        #endif

        return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
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
}