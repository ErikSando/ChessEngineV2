#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "Attacks.h"
#include "Board.h"

// https://www.chessprogramming.org/Looking_for_Magics

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

U64 FindMagic(int square, int bits, bool bishop) {
    U64 magic, occupancy[4096], attacks[4096], used[4096];

    U64 mask = bishop ? MaskB(square) : MaskR(square);
    int n = CountBits(mask);

    for (int i = 0; i < (1 << bits); i++) {
        occupancy[i] = IndexToU64(i, bits, mask);
        attacks[i] = bishop ? GenerateBishopAttacks(square, occupancy[i])
                            : GenerateRookAttacks(square, occupancy[i]);
    }

    for (int k = 0; k < 100000000; k++) {
        magic = RandU64FewBits();

        if (CountBits((mask * magic) & 0xFF00000000000000ULL) < 6) continue;

        bool slotUsed[4096] = { false };
        memset(used, 0, sizeof(used));

        bool fail = false;

        for (int i = 0; !fail && i < (1 << bits); i++) {
            int index = ((occupancy[i] * magic) >> (64 - bits)) & ((1 << bits) - 1);

            if (!slotUsed[index]) {
                used[index] = attacks[i];
                slotUsed[index] = true;
            }
            else if (used[index] != attacks[i]) {
                fail = true;
            }

            // if (used[index] == 0ULL) used[index] = attacks[i];
            // else if (used[index] != attacks[index]) fail = true;
        }

        if (!fail) return magic;
    }

    std::cout << "Failed to find magic for square " << ToSquareString(square);
    return 0ULL;
}

void FindMagics() {
    srand(time(nullptr));

    std::cout << "constexpr U64 MagicsB[64] = {\n";

    for (int row = 0; row < 16; row++) {
        std::cout << "   ";

        for (int col = 0; col < 4; col ++) {
            int square = col + row * 4;
            std::cout << " 0x" << std::hex << FindMagic(square, RelevantBitsR[square], true) << "ULL,";
        }

        std::cout << "\n";
    }

    std::cout << "};\n\n";

    std::cout << "constexpr U64 MagicsR[64] = {\n";

    for (int row = 0; row < 16; row++) {
        std::cout << "   ";

        for (int col = 0; col < 4; col ++) {
            int square = col + row * 4;
            std::cout << " 0x" << std::hex << FindMagic(square, RelevantBitsR[square], false) << "ULL,";
        }

        std::cout << "\n";
    }

    std::cout << "};\n\n";
}