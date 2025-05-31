#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "Attacks.h"
#include "Board.h"

// stuff to find magics
// https://www.chessprogramming.org/Looking_for_Magics

#if defined(__GNUC__) || defined(__clang__)
    inline int CountBits(U64 bitboard) {
        return __builtin_popcountll(bitboard);
    }
#elif defined(_MSC_VER) && defined(_M_X64)
    #include <intrin.h>
    #pragma intrinsic(_popcnt64)

    inline int CountBits(U64 bitboard) {
        return static_cast<int>(_popcnt64(bitboard));
    }
#else
    inline int CountBits(U64 bitboard) {
        int n = 0;
        for (; bitboard; n++, bitboard &= (bitboard - 1));
        return n;
    }
#endif

U64 RandU64() {
    U64 u1, u2, u3, u4;
    u1 = (U64)(random()) & 0xFFFF;
    u2 = (U64)(random()) & 0xFFFF;
    u3 = (U64)(random()) & 0xFFFF;
    u4 = (U64)(random()) & 0xFFFF;
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

U64 RandU64FewBits() {
    return RandU64() & RandU64() & RandU64();
}

const int BitTable[64] = {
    63, 30, 3, 32, 25, 41, 22, 33,
    15, 50, 42, 13, 11, 53, 19, 34,
    61, 29, 2, 51, 21, 43, 45, 10,
    18, 47, 1, 54, 9, 57, 0, 35,
    62, 31, 40, 4, 49, 5, 52, 26,
    60, 6, 23, 44, 46, 27, 56, 16,
    7, 39, 48, 24, 59, 14, 12, 55,
    38, 28, 58, 20, 37, 17, 36, 8
};

int Pop1stBit(U64& bitboard) {
    U64 b = bitboard ^ (bitboard - 1);
    unsigned int fold = (unsigned) ((b & 0xFFFFFFFF) ^ (b >> 32));
    bitboard &= bitboard - 1;
    return BitTable[(fold * 0x783A9B23) >> 26];
}

U64 IndexToU64(int index, int bits, U64 bitboard) {
    U64 result = 0ULL;

    for (int i = 0; i < bits; i++) {
        int square = Pop1stBit(bitboard);
        if (index & (1 << i)) result |= (1ULL << square);
    }

    return result;
}

U64 MaskR(int square) {
    U64 result = 0ULL;

    int file = GetFile(square);
    int rank = GetRank(square);

    for (int f = file + 1; f <= G; f++) SetBit(result, f + rank * 8);
    for (int f = file - 1; f >= B; f--) SetBit(result, f + rank * 8);
    for (int r = rank + 1; r <= Seven; r++) SetBit(result, file + r * 8);
    for (int r = rank - 1; r >= Two; r--) SetBit(result, file + r * 8);

    return result;
}

U64 MaskB(int square) {
    U64 result = 0ULL;

    int file = GetFile(square);
    int rank = GetRank(square);

    for (int f = file + 1, r = rank + 1; f <= G && r <= Seven; f++, r++) SetBit(result, f + r * 8);
    for (int f = file + 1, r = rank - 1; f <= G && r >= Two; f++, r--) SetBit(result, f + r * 8);
    for (int f = file - 1, r = rank + 1; f >= B && r <= Seven; f--, r++) SetBit(result, f + r * 8);
    for (int f = file - 1, r = rank - 1; f >= B && r >= Two; f--, r--) SetBit(result, f + r * 8);

    return result;
}

U64 AttackMaskR(int square, U64 blockers) {
    U64 result = 0ULL;

    int file = GetFile(square);
    int rank = GetRank(square);

    for (int f = file + 1; f <= File::H; f++) {
        int square = f + rank * 8;
        SetBit(result, square);
        if (IsBitSet(blockers, square)) break;
    }

    for (int f = file - 1; f >= File::A; f--) {
        int square = f + rank * 8;
        SetBit(result, square);
        if (IsBitSet(blockers, square)) break;
    }

    for (int r = rank + 1; r <= Rank::Eight; r++) {
        int square = file + r * 8;
        SetBit(result, square);
        if (IsBitSet(blockers, square)) break;
    }

    for (int r = rank - 1; r >= Rank::One; r--) {
        int square = file + r * 8;
        SetBit(result, square);
        if (IsBitSet(blockers, square)) break;
    }

    return result;
}

U64 AttackMaskB(int square, U64 blockers) {
    U64 result = 0ULL;

    int file = GetFile(square);
    int rank = GetRank(square);

    for (int f = file + 1, r = rank + 1; f <= H && r <= Eight; f++, r++) {
        int square = f + r * 8;
        SetBit(result, square);
        if (IsBitSet(blockers, square)) break;
    }

    for (int f = file + 1, r = rank - 1; f <= H && r >= One; f++, r--) {
        int square = f + r * 8;
        SetBit(result, square);
        if (IsBitSet(blockers, square)) break;
    }

    for (int f = file - 1, r = rank + 1; f >= A && rank <= Eight; f--, r++) {
        int square = f + r * 8;
        SetBit(result, square);
        if (IsBitSet(blockers, square)) break;
    }

    for (int f = file - 1, r = rank - 1; f >= A && r >= One; f--, r--) {
        int square = f + r * 8;
        SetBit(result, square);
        if (IsBitSet(blockers, square)) break;
    }

    return result;
}

U64 FindMagic(int square, int bits, bool bishop) {
    U64 magic, blockers[4096], attacks[4096], used[4096];

    U64 mask = bishop ? MaskB(square) : MaskR(square);
    int n = CountBits(mask);

    for (int i = 0; i < (1 << bits); i++) {
        blockers[i] = IndexToU64(i, bits, mask);
        attacks[i] = bishop ? AttackMaskB(square, blockers[i]) : AttackMaskR(square, blockers[i]);
    }

    for (int k = 0; k < 100000000; k++) {
        magic = RandU64FewBits();

        if (CountBits((mask * magic) & 0xFF00000000000000ULL) < 6) continue;

        memset(used, 0ULL, sizeof(used));

        bool fail = false;

        for (int i = 0; !fail && i < (1 << bits); i++) {
            int index = (int) (blockers[i] * magic) >> (64 - bits);

            if (used[index] == 0ULL) used[index] = attacks[i];
            else if (used[index] != attacks[index]) fail = true;
        }

        if (!fail) return magic;
    }

    std::cout << "Failed to find magic for square " << ToSquareString(square);
    return 0ULL;
}

void FindMagics() {
    srand(time(nullptr));

    std::cout << "constexpr U64 MagicsR[64] = {\n";

    for (int square = 0; square < 64; square++) {
        std::cout << "    0x" << std::hex << FindMagic(square, RelevantBitsR[square], false) << "ULL,\n";
    }

    std::cout << "};\n\n";

    std::cout << "constexpr U64 MagicsB[64] = {\n";

    for (int square = 0; square < 64; square++) {
        std::cout << "    0x" << std::hex << FindMagic(square, RelevantBitsB[square], true) << "ULL,\n";
    }

    std::cout << "};\n\n";
}