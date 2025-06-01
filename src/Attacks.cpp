#include <iostream>

#include "Attacks.h"
#include "Data.h"
#include "Board.h"

U64 AttackMasksB[64];
U64 AttackMasksR[64];

U64 GeneratePawnMoves(int side, int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 1ULL << square;

    if (side == White) {
        attacks |= bitboard << 8;
        if (GetRank(square) == Rank2) attacks |= bitboard << 16;
    }
    else {
        attacks |= bitboard >> 8;
        if (GetRank(square) == Rank7) attacks |= bitboard >> 16;
    }

    return attacks;
}

U64 GeneratePawnCaptures(int side, int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 1ULL << square;

    if (side == White) {
        attacks |=
            (bitboard << 7) & NotFileH |
            (bitboard << 9) & NotFileA;
    }
    else {
        attacks |=
            (bitboard >> 7) & NotFileA |
            (bitboard >> 9) & NotFileH;
    }

    return attacks;
}

U64 GenerateKnightAttacks(int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 1ULL << square;

    attacks |=
        (bitboard << 6) & NotFileGH |
        (bitboard << 10) & NotFileAB |
        (bitboard << 15) & NotFileH |
        (bitboard << 17) & NotFileA |
        (bitboard >> 6) & NotFileAB |
        (bitboard >> 10) & NotFileGH |
        (bitboard >> 15) & NotFileA |
        (bitboard >> 17) & NotFileH;

    return attacks;
}

U64 GenerateKingAttacks(int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 1ULL << square;

    attacks |=
        (bitboard << 7) & NotFileH |
        (bitboard << 8) |
        (bitboard << 9) & NotFileA |
        (bitboard << 1) & NotFileA |
        (bitboard >> 1) & NotFileH |
        (bitboard >> 7) & NotFileA |
        (bitboard >> 8) |
        (bitboard >> 9) & NotFileH;

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

U64 MaskB(int square) {
    U64 mask = 0ULL;

    int file = GetFile(square);
    int rank = GetRank(square);

    for (int f = file + 1, r = rank + 1; f <= FileG && r <= Rank7; f++, r++) SetBit(mask, GetSquare(f, r));
    for (int f = file + 1, r = rank - 1; f <= FileG && r >= Rank2; f++, r--) SetBit(mask, GetSquare(f, r));
    for (int f = file - 1, r = rank + 1; f >= FileB && r <= Rank7; f--, r++) SetBit(mask, GetSquare(f, r));
    for (int f = file - 1, r = rank - 1; f >= FileB && r >= Rank2; f--, r--) SetBit(mask, GetSquare(f, r));

    return mask;
}

U64 MaskR(int square) {
    U64 mask = 0ULL;

    int file = GetFile(square);
    int rank = GetRank(square);

    for (int f = file + 1; f <= FileG; f++) SetBit(mask, GetSquare(f, rank));
    for (int f = file - 1; f >= FileB; f--) SetBit(mask, GetSquare(f, rank));
    for (int r = rank + 1; r <= Rank7; r++) SetBit(mask, GetSquare(file, r));
    for (int r = rank - 1; r >= Rank2; r--) SetBit(mask, GetSquare(file, r));

    return mask;
}

U64 GenerateBishopAttacks(int square, U64 occupancy) {
    U64 attacks = 0ULL;

    int file = GetFile(square);
    int rank = GetRank(square);

    for (int f = file + 1, r = rank + 1; f <= FileH && r <= Rank8; f++, r++) {
        int square = GetSquare(f, r);
        SetBit(attacks, square);
        if (GetBit(occupancy, square)) break;
    }


    for (int f = file + 1, r = rank - 1; f <= FileH && r >= Rank1; f++, r--) {
       int square = GetSquare(f, r);
       SetBit(attacks, square);
        if (GetBit(occupancy, square)) break;
    }

    for (int f = file - 1, r = rank + 1; f >= FileA && r <= Rank8; f--, r++) {
        int square = GetSquare(f, r);
        SetBit(attacks, square);
        if (GetBit(occupancy, square)) break;
    }

    for (int f = file - 1, r = rank - 1; f >= FileA && r >= Rank1; f--, r--) {
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

    for (int f = file + 1; f <= FileH; f++) {
        int square = GetSquare(f, rank);
        SetBit(attacks, square);
        if (GetBit(occupancy, square)) break;
    }

    for (int f = file - 1; f >= FileA; f--) {
        int square = GetSquare(f, rank);
        attacks |= (1ULL << square);
        if (GetBit(occupancy, square)) break;
    }

    for (int r = rank + 1; r <= Rank8; r++) {
        int square = GetSquare(file, r);
        SetBit(attacks, square);
        if (GetBit(occupancy, square)) break;
    }

    for (int r = rank - 1; r >= Rank1; r--) {
        int square = GetSquare(file, r);
        SetBit(attacks, square);
        if (GetBit(occupancy, square)) break;
    }

    return attacks;
}

U64 Board::GetBishopAttacks(int square, U64 occupancy) {
    occupancy &= AttackMasksB[square];
    occupancy *= MagicsB[square];
    occupancy >>= 64 - RelevantBitsB[square];

    return bishopAttacks[square][occupancy];
}

U64 Board::GetRookAttacks(int square, U64 occupancy) {
    occupancy &= AttackMasksR[square];
    occupancy *= MagicsR[square];
    occupancy >>= 64 - RelevantBitsR[square];

    return rookAttacks[square][occupancy];
}

U64 Board::GetQueenAttacks(int square, U64 occupancy) {
    U64 bishopOccupancy = occupancy;
    U64 rookOccupancy = occupancy;

    bishopOccupancy &= AttackMasksB[square];
    bishopOccupancy *= MagicsB[square];
    bishopOccupancy >>= 64 - RelevantBitsB[square];

    rookOccupancy &= AttackMasksR[square];
    rookOccupancy *= MagicsR[square];
    rookOccupancy >>= 64 - RelevantBitsR[square];

    return bishopAttacks[square][bishopOccupancy] | rookAttacks[square][rookOccupancy];
}

void Board::InitAttackTables() {
    for (int square = 0; square < 64; square++) {
        pawnMoves[White][square] = GeneratePawnMoves(White, square);
        pawnMoves[Black][square] = GeneratePawnMoves(Black, square);
        pawnCaptures[White][square] = GeneratePawnCaptures(White, square);
        pawnCaptures[Black][square] = GeneratePawnCaptures(Black, square);
        knightAttacks[square] = GenerateKnightAttacks(square);
        kingAttacks[square] = GenerateKingAttacks(square);

        AttackMasksB[square] = MaskB(square);
        U64 attackMaskB = AttackMasksB[square];

        int relevantBitsB = RelevantBitsB[square];
        int occupancyIndicesB = 1 << relevantBitsB;

        for (int i = 0; i < occupancyIndicesB; i++) {
            U64 occupancy = IndexToU64(i, relevantBitsB, attackMaskB);
            int magicIndex = (occupancy * MagicsB[square]) >> (64 - relevantBitsB);
            bishopAttacks[square][magicIndex] = GenerateBishopAttacks(square, occupancy);
        }

        AttackMasksR[square] = MaskR(square);
        U64 attackMaskR = AttackMasksR[square];

        int relevantBitsR = RelevantBitsR[square];
        int occupancyIndicesR = 1 << relevantBitsR;

        for (int i = 0; i < occupancyIndicesR; i++) {
            U64 occupancy = IndexToU64(i, relevantBitsR, attackMaskR);
            int magicIndex = (occupancy * MagicsR[square]) >> (64 - relevantBitsR);
            rookAttacks[square][magicIndex] = GenerateRookAttacks(square, occupancy);
        }
    }

    // FindMagics();
}