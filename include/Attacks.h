#pragma once

#include "Data.h"
#include "Globals.h"

namespace ErikEngine {
    namespace Attacks {
        void Init();
        void FindMagics();

        U64 IndexToU64(int index, int bits, U64 bitboard);
        U64 GenerateBishopAttackMask(int square);
        U64 GenerateRookAttackMask(int square);
        U64 GenerateBishopAttacks(int square, U64 blockers);
        U64 GenerateRookAttacks(int square, U64 blockers);
        // U64 GetBishopAttacks(int square, U64 blockers);
        // U64 GetRookAttacks(int square, U64 blockers);
        // U64 GetQueenAttacks(int square, U64 blockers);

        extern U64 PawnMoves[2][64];
        extern U64 PawnCaptures[2][64];
        extern U64 KnightAttacks[64];
        extern U64 BishopAttacks[64][512];
        extern U64 RookAttacks[64][4096];
        extern U64 QueenAttacks[64];
        extern U64 KingAttacks[64];

        extern U64 BishopRays[64][64];
        extern U64 RookRays[64][64];
        //extern U64 SliderRays[64][64];

        extern U64 BishopAttackMasks[64];
        extern U64 RookAttackMasks[64];

        inline U64 GetSliderRay(int fromSquare, int toSquare, int pieceType = Q) {
            switch (pieceType) {
                case B: return BishopRays[fromSquare][toSquare];
                case R: return RookRays[fromSquare][toSquare];
            }

            return BishopRays[fromSquare][toSquare] | RookRays[fromSquare][toSquare];
        }

        constexpr U64 NotFileA = 0xFEFEFEFEFEFEFEFEULL;
        constexpr U64 NotFileAB = 0xFCFCFCFCFCFCFCFCULL;
        constexpr U64 NotFileGH = 0x3F3F3F3F3F3F3F3FULL;
        constexpr U64 NotFileH = 0x7F7F7F7F7F7F7F7FULL;

        constexpr int BishopRelevantBits[64] = {
            6, 5, 5, 5, 5, 5, 5, 6,
            5, 5, 5, 5, 5, 5, 5, 5,
            5, 5, 7, 7, 7, 7, 5, 5,
            5, 5, 7, 9, 9, 7, 5, 5,
            5, 5, 7, 9, 9, 7, 5, 5,
            5, 5, 7, 7, 7, 7, 5, 5,
            5, 5, 5, 5, 5, 5, 5, 5,
            6, 5, 5, 5, 5, 5, 5, 6
        };

        constexpr int RookRelevantBits[64] = {
            12, 11, 11, 11, 11, 11, 11, 12,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            12, 11, 11, 11, 11, 11, 11, 12
        };

        constexpr U64 BishopMagics[64] = {
            0x202420204040080ULL, 0x1108100440802008ULL, 0x4221010522004000ULL, 0x104041080410213ULL,
            0x11040c0500208ULL, 0x1010900420020e00ULL, 0x240201500a0442ULL, 0x420041084000ULL,
            0x41866002140101ULL, 0x3004010204011aULL, 0x48100100510420ULL, 0x200808410c4022ULL,
            0x41044940340ULL, 0x41080a060e222040ULL, 0x920840242026008ULL, 0x90100822000ULL,
            0x1821000408020816ULL, 0x4012008020051ULL, 0x92480a0408002208ULL, 0xc008000082004000ULL,
            0x940046020a0002ULL, 0x2120200202012030ULL, 0x4400800500c82000ULL, 0x401000021080210ULL,
            0x10502004149000ULL, 0x111100008026801ULL, 0x521050130184200ULL, 0x6004004004010002ULL,
            0x40840002020201ULL, 0x8801020100405000ULL, 0x10280144030c0280ULL, 0x201003280420800ULL,
            0xc514000040401ULL, 0x2018040200100281ULL, 0x20cc23000080048ULL, 0x8006080e0080200ULL,
            0x30108200052200ULL, 0x10248b01000a1018ULL, 0x1004008088820800ULL, 0xe081420022020104ULL,
            0x610044440a220ULL, 0x800808820500800ULL, 0x1021404020801000ULL, 0x12100c208804802ULL,
            0x880880304040240ULL, 0x8010203480200100ULL, 0x41009121c948410ULL, 0x480802084a000040ULL,
            0xa1011010448020ULL, 0xa001040201040940ULL, 0x4000004404040000ULL, 0x4001001610441000ULL,
            0x582821090000ULL, 0x100100250410800ULL, 0x40048104010002ULL, 0x500822108212002cULL,
            0x90220802080200ULL, 0x8040020221040200ULL, 0x18040002108080cULL, 0x80840414ULL,
            0x8000084040850101ULL, 0x6002420a8010840ULL, 0x40089084980040ULL, 0x8041000a2008200ULL
        };

        constexpr U64 RookMagics[64] = {
            0x2180008129400010ULL, 0x8040200010004000ULL, 0x780083000200184ULL, 0x4100082210000500ULL,
            0x200100402000920ULL, 0x2a00100801020084ULL, 0x880110002000080ULL, 0x11800b0004a44080ULL,
            0x21800040018030ULL, 0x220400050002002ULL, 0x40801000200084ULL, 0x2080801000800800ULL,
            0x10800800040081ULL, 0x14a001008020004ULL, 0x2a0808042002100ULL, 0x8042000418804912ULL,
            0x6201818001400020ULL, 0xc020058021400180ULL, 0x100220010420080ULL, 0x305242000a002210ULL,
            0x4008010008100500ULL, 0x2008004008002ULL, 0x820040008024130ULL, 0x20000985104ULL,
            0x220800080204000ULL, 0x500240022000ULL, 0x1000108200420020ULL, 0x402042200100840ULL,
            0xa048008080080400ULL, 0x44040080800200ULL, 0x980080400020110ULL, 0x800000420023018cULL,
            0x80804000800020ULL, 0x402001401000ULL, 0x400104101002002ULL, 0x5500100080800800ULL,
            0x108020040400400ULL, 0x30800200800401ULL, 0x910b484104000250ULL, 0x40404082000401ULL,
            0x9100800040008020ULL, 0x4400481090020ULL, 0x210200010008080ULL, 0x2830001100210008ULL,
            0x1014001008010100ULL, 0x4010044010080120ULL, 0x1284100201040008ULL, 0xa004091020024ULL,
            0x80002000400040ULL, 0x802000400080ULL, 0x1000100085200680ULL, 0x8061002010000900ULL,
            0x25001004080100ULL, 0x20080040080ULL, 0x1001019210083400ULL, 0x8044008044010200ULL,
            0x6080022080401501ULL, 0xa0c8210200401082ULL, 0xa1000810200441ULL, 0x20080500201001ULL,
            0x21000800701205ULL, 0x1002000108041002ULL, 0x1090209002080104ULL, 0x4a88002084010042ULL
        };

        inline U64 GetBishopAttacks(int square, U64 occupancy) {
            occupancy &= BishopAttackMasks[square];
            occupancy *= BishopMagics[square];
            occupancy >>= 64 - BishopRelevantBits[square];

            return BishopAttacks[square][occupancy];
        }

        inline U64 GetRookAttacks(int square, U64 occupancy) {
            occupancy &= RookAttackMasks[square];
            occupancy *= RookMagics[square];
            occupancy >>= 64 - RookRelevantBits[square];

            return RookAttacks[square][occupancy];
        }

        inline U64 GetQueenAttacks(int square, U64 occupancy) {
            U64 bishopOccupancy = occupancy;
            U64 rookOccupancy = occupancy;

            bishopOccupancy &= BishopAttackMasks[square];
            bishopOccupancy *= BishopMagics[square];
            bishopOccupancy >>= 64 - BishopRelevantBits[square];

            rookOccupancy &= RookAttackMasks[square];
            rookOccupancy *= RookMagics[square];
            rookOccupancy >>= 64 - RookRelevantBits[square];

            return BishopAttacks[square][bishopOccupancy] | RookAttacks[square][rookOccupancy];
        }

        // inline U64 GetPieceAttacks(int pieceType, int square, U64 blockers = 0ULL) {
        //     switch (pieceType) {
        //         case N: return KnightAttacks[square];
        //         case B: return GetBishopAttacks(square, blockers);
        //         case R: return GetRookAttacks(square, blockers);
        //         case Q: return GetQueenAttacks(square, blockers);
        //         case K: return KingAttacks[square];
        //     }

        //     return 0ULL;
        // }

        inline U64 GetAttackMask(int pieceType, int square, U64 blockers = 0ULL) {
            switch (pieceType) {
                case N: return KnightAttacks[square];
                case B: return GetBishopAttacks(square, blockers);
                case R: return GetRookAttacks(square, blockers);
                case Q: return GetQueenAttacks(square, blockers);
                case K: return KingAttacks[square];
            }

            return 0ULL;
        }
    }
}