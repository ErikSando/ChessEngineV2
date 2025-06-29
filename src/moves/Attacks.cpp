#include <cassert>
#include <cstdlib>
#include <string.h>

#include "Attacks.h"
#include "Data.h"
#include "Board.h"
#include "Utils.h"

namespace ErikEngine {
    namespace Attacks {
        U64 PawnMoves[2][64];
        U64 PawnCaptures[2][64];
        U64 KnightAttacks[64];
        U64 BishopAttacks[64][512];
        U64 RookAttacks[64][4096];
        U64 QueenAttacks[64];
        U64 KingAttacks[64];

        U64 BishopRays[64][64];
        U64 RookRays[64][64];
        //U64 SliderRays[64][64];

        U64 BishopAttackMasks[64];
        U64 RookAttackMasks[64];

        U64 GeneratePawnMoves(int side, int square) {
            U64 attacks = 0ULL;
            U64 bitboard = 1ULL << square;

            if (side == WHITE) {
                attacks |= bitboard << 8;
                if (GetRank(square) == RANK_2) attacks |= bitboard << 16;
            }
            else {
                attacks |= bitboard >> 8;
                if (GetRank(square) == RANK_7) attacks |= bitboard >> 16;
            }

            return attacks;
        }

        U64 GeneratePawnCaptures(int side, int square) {
            U64 attacks = 0ULL;
            U64 bitboard = 1ULL << square;

            if (side == WHITE) {
                attacks |=
                    ((bitboard << 7) & NotFileH) |
                    ((bitboard << 9) & NotFileA);
            }
            else {
                attacks |=
                    ((bitboard >> 7) & NotFileA) |
                    ((bitboard >> 9) & NotFileH);
            }

            return attacks;
        }

        U64 GenerateKnightAttacks(int square) {
            U64 attacks = 0ULL;
            U64 bitboard = 1ULL << square;

            attacks |=
                ((bitboard << 6) & NotFileGH) |
                ((bitboard << 10) & NotFileAB) |
                ((bitboard << 15) & NotFileH) |
                ((bitboard << 17) & NotFileA) |
                ((bitboard >> 6) & NotFileAB) |
                ((bitboard >> 10) & NotFileGH) |
                ((bitboard >> 15) & NotFileA) |
                ((bitboard >> 17) & NotFileH);

            return attacks;
        }

        U64 GenerateKingAttacks(int square) {
            U64 attacks = 0ULL;
            U64 bitboard = 1ULL << square;

            attacks |=
                ((bitboard << 7) & NotFileH) |
                ((bitboard << 8)) |
                ((bitboard << 9) & NotFileA) |
                ((bitboard << 1) & NotFileA) |
                ((bitboard >> 1) & NotFileH) |
                ((bitboard >> 7) & NotFileA) |
                ((bitboard >> 8)) |
                ((bitboard >> 9) & NotFileH);

            return attacks;
        }

        U64 IndexToU64(int index, int bits, U64 bitboard) {
            U64 occupancy = 0ULL;

            for (int i = 0; i < bits; i++) {
                int square = PopFirstBit(bitboard);
                if (index & (1 << i)) occupancy |= (1ULL << square);
            }

            return occupancy;
        }

        U64 GenerateBishopAttackMask(int square) {
            U64 mask = 0ULL;

            int file = GetFile(square);
            int rank = GetRank(square);

            for (int f = file + 1, r = rank + 1; f <= FILE_G && r <= RANK_7; f++, r++) SetBit(mask, GetSquare(f, r));
            for (int f = file + 1, r = rank - 1; f <= FILE_G && r >= RANK_2; f++, r--) SetBit(mask, GetSquare(f, r));
            for (int f = file - 1, r = rank + 1; f >= FILE_B && r <= RANK_7; f--, r++) SetBit(mask, GetSquare(f, r));
            for (int f = file - 1, r = rank - 1; f >= FILE_B && r >= RANK_2; f--, r--) SetBit(mask, GetSquare(f, r));

            return mask;
        }

        U64 GenerateRookAttackMask(int square) {
            U64 mask = 0ULL;

            int file = GetFile(square);
            int rank = GetRank(square);

            for (int f = file + 1; f <= FILE_G; f++) SetBit(mask, GetSquare(f, rank));
            for (int f = file - 1; f >= FILE_B; f--) SetBit(mask, GetSquare(f, rank));
            for (int r = rank + 1; r <= RANK_7; r++) SetBit(mask, GetSquare(file, r));
            for (int r = rank - 1; r >= RANK_2; r--) SetBit(mask, GetSquare(file, r));

            return mask;
        }

        U64 GenerateBishopAttacks(int square, U64 occupancy) {
            U64 attacks = 0ULL;

            int file = GetFile(square);
            int rank = GetRank(square);

            for (int f = file + 1, r = rank + 1; f <= FILE_H && r <= RANK_8; f++, r++) {
                int square = GetSquare(f, r);
                SetBit(attacks, square);
                if (GetBit(occupancy, square)) break;
            }


            for (int f = file + 1, r = rank - 1; f <= FILE_H && r >= RANK_1; f++, r--) {
            int square = GetSquare(f, r);
            SetBit(attacks, square);
                if (GetBit(occupancy, square)) break;
            }

            for (int f = file - 1, r = rank + 1; f >= FILE_A && r <= RANK_8; f--, r++) {
                int square = GetSquare(f, r);
                SetBit(attacks, square);
                if (GetBit(occupancy, square)) break;
            }

            for (int f = file - 1, r = rank - 1; f >= FILE_A && r >= RANK_1; f--, r--) {
                int square = GetSquare(f, r);
                SetBit(attacks, square);
                if (GetBit(occupancy, square)) break;
            }

            return attacks;
        }

        U64 GenerateRookAttacks(int square, U64 occupancy) {
            U64 attacks = 0ULL;

            int file = GetFile(square);
            int rank = GetRank(square);

            for (int f = file + 1; f <= FILE_H; f++) {
                int square = GetSquare(f, rank);
                SetBit(attacks, square);
                if (GetBit(occupancy, square)) break;
            }

            for (int f = file - 1; f >= FILE_A; f--) {
                int square = GetSquare(f, rank);
                attacks |= (1ULL << square);
                if (GetBit(occupancy, square)) break;
            }

            for (int r = rank + 1; r <= RANK_8; r++) {
                int square = GetSquare(file, r);
                SetBit(attacks, square);
                if (GetBit(occupancy, square)) break;
            }

            for (int r = rank - 1; r >= RANK_1; r--) {
                int square = GetSquare(file, r);
                SetBit(attacks, square);
                if (GetBit(occupancy, square)) break;
            }

            return attacks;
        }

        // U64 GetBishopAttacks(int square, U64 occupancy) {
        //     occupancy &= BishopAttackMasks[square];
        //     occupancy *= BishopMagics[square];
        //     occupancy >>= 64 - BishopRelevantBits[square];

        //     return BishopAttacks[square][occupancy];
        // }

        // U64 GetRookAttacks(int square, U64 occupancy) {
        //     occupancy &= RookAttackMasks[square];
        //     occupancy *= RookMagics[square];
        //     occupancy >>= 64 - RookRelevantBits[square];

        //     return RookAttacks[square][occupancy];
        // }

        // U64 GetQueenAttacks(int square, U64 occupancy) {
        //     U64 bishopOccupancy = occupancy;
        //     U64 rookOccupancy = occupancy;

        //     bishopOccupancy &= BishopAttackMasks[square];
        //     bishopOccupancy *= BishopMagics[square];
        //     bishopOccupancy >>= 64 - BishopRelevantBits[square];

        //     rookOccupancy &= RookAttackMasks[square];
        //     rookOccupancy *= RookMagics[square];
        //     rookOccupancy >>= 64 - RookRelevantBits[square];

        //     return BishopAttacks[square][bishopOccupancy] | RookAttacks[square][rookOccupancy];
        // }

        void InitAttackTables() {
            for (int square = 0; square < 64; square++) {
                PawnMoves[WHITE][square] = GeneratePawnMoves(WHITE, square);
                PawnMoves[BLACK][square] = GeneratePawnMoves(BLACK, square);
                PawnCaptures[WHITE][square] = GeneratePawnCaptures(WHITE, square);
                PawnCaptures[BLACK][square] = GeneratePawnCaptures(BLACK, square);
                KnightAttacks[square] = GenerateKnightAttacks(square);
                KingAttacks[square] = GenerateKingAttacks(square);

                BishopAttackMasks[square] = GenerateBishopAttackMask(square);
                U64 attackGenerateBishopAttackMask = BishopAttackMasks[square];

                int bishopRelevantBits = BishopRelevantBits[square];
                int occupancyIndicesB = 1 << bishopRelevantBits;

                for (int i = 0; i < occupancyIndicesB; i++) {
                    U64 occupancy = IndexToU64(i, bishopRelevantBits, attackGenerateBishopAttackMask);
                    int magicIndex = (occupancy * BishopMagics[square]) >> (64 - bishopRelevantBits);
                    BishopAttacks[square][magicIndex] = GenerateBishopAttacks(square, occupancy);
                }

                RookAttackMasks[square] = GenerateRookAttackMask(square);
                U64 attackGenerateRookAttackMask = RookAttackMasks[square];

                int relevantBitsR = RookRelevantBits[square];
                int occupancyIndicesR = 1 << relevantBitsR;

                for (int i = 0; i < occupancyIndicesR; i++) {
                    U64 occupancy = IndexToU64(i, relevantBitsR, attackGenerateRookAttackMask);
                    int magicIndex = (occupancy * RookMagics[square]) >> (64 - relevantBitsR);
                    RookAttacks[square][magicIndex] = GenerateRookAttacks(square, occupancy);
                }
            }
        }

        inline void CreateRay(int fromSquare, int toSquare, int fileJump, int rankJump, U64& ray) {
            int fromFile = GetFile(fromSquare);
            int fromRank = GetRank(fromSquare);
            int toFile = GetFile(toSquare);
            int toRank = GetRank(toSquare);
            
            int file = fromFile + fileJump;
            int rank = fromRank + rankJump;

            while (file != toFile || rank != toRank) {
                int square = GetSquare(file, rank);
                SetBit(ray, square);

                file += fileJump;
                rank += rankJump;
            }
        }

        void InitSliderRays() {
            memset(BishopRays, 0, sizeof(BishopRays));
            memset(RookRays, 0, sizeof(RookRays));
            //memset(SliderRays, 0, sizeof(SliderRays));

            for (int fromSquare = 0; fromSquare < 64; fromSquare++) {
                int fromFile = GetFile(fromSquare);
                int fromRank = GetRank(fromSquare);

                for (int toSquare = 0; toSquare < 64; toSquare++) {
                    if (fromSquare == toSquare) continue;
                    
                    int toFile = GetFile(toSquare);
                    int toRank = GetRank(toSquare);

                    int dFile = toFile - fromFile; // delta or change in file
                    int dRank = toRank - fromRank;

                    int fileJump = !dFile ? 0 : std::abs(dFile) / dFile;
                    int rankJump = !dRank ? 0 : std::abs(dRank) / dRank;

                    if (std::abs(dFile) == std::abs(dRank)) { // bishops
                        CreateRay(fromSquare, toSquare, fileJump, rankJump, BishopRays[fromSquare][toSquare]);
                    }

                    if (!dFile || !dRank) { // rooks
                        CreateRay(fromSquare, toSquare, fileJump, rankJump, RookRays[fromSquare][toSquare]);
                    }
                }
            }
        }

        void Init() {
            InitAttackTables();
            InitSliderRays();
        }
    }
}