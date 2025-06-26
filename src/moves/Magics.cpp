#include <iostream>
#include <string.h>

#include "Attacks.h"
#include "Board.h"
#include "Utils.h"

// https://www.chessprogramming.org/Looking_for_Magics

namespace ErikEngine {
    namespace Attacks {
        U64 RandU64FewBits() {
            return RandU64() & RandU64() & RandU64();
        }

        U64 FindMagic(int square, bool bishop) {
            U64 magic, occupancy[4096], attacks[4096], used[4096];

            U64 mask = bishop ? BishopAttackMasks[square] : RookAttackMasks[square];
            int bits = bishop ? BishopRelevantBits[square] : RookRelevantBits[square];
            //int bits = CountBits(mask);

            int occupIndices = 1 << bits;

            for (int i = 0; i < occupIndices; i++) {
                occupancy[i] = IndexToU64(i, bits, mask);
                attacks[i] = bishop ? GenerateBishopAttacks(square, occupancy[i])
                                    : GenerateRookAttacks(square, occupancy[i]);
            }

            for (int k = 0; k < 100000000; k++) {
                magic = RandU64FewBits();

                if (CountBits((mask * magic) & 0xFF00000000000000ULL) < 6) continue;

                memset(used, 0, sizeof(used));

                bool fail = false;

                for (int i = 0; !fail && i < occupIndices; i++) {
                    int index = (int) ((occupancy[i] * magic) >> (64 - bits));

                    if (used[index] == 0ULL) used[index] = attacks[i];
                    else if (used[index] != attacks[i]) fail = true;
                }

                if (!fail) return magic;
            }

            std::cout << "Failed to find magic for square " << Utils::ToSquareString(square);
            return 0ULL;
        }

        void FindMagics() {
            std::cout << "constexpr U64 BishopMagics[64] = {\n";

            for (int row = 0; row < 16; row++) {
                std::cout << "   ";

                for (int col = 0; col < 4; col ++) {
                    int square = col + row * 4;
                    std::cout << " 0x" << std::hex << FindMagic(square, true) << "ULL,";
                }

                std::cout << "\n";
            }

            std::cout << "};\n\nconstexpr U64 RookMagics[64] = {\n";

            for (int row = 0; row < 16; row++) {
                std::cout << "   ";

                for (int col = 0; col < 4; col ++) {
                    int square = col + row * 4;
                    std::cout << " 0x" << std::hex << FindMagic(square, false) << "ULL,";
                }

                std::cout << "\n";
            }

            std::cout << "};\n\n";
        }
    }
}