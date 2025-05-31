#include "Attacks.h"
#include "Board.h"

U64 AttackMasksB[64];
U64 AttackMasksR[64];

U64 GeneratePawnMoves(int side, int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 1ULL << square;

    if (side == White) {
        attacks |= bitboard << 8;
        if (GetRank(square) == Rank::Two) attacks |= bitboard << 16;
    }
    else {
        attacks |= bitboard >> 8;
        if (GetRank(square) == Rank::Seven) attacks |= bitboard >> 16;
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
    U64 result = 0ULL;

    for (int i = 0; i < bits; i++) {
        int square = PopFirstBit(bitboard);
        if (index & (1 << i)) result |= (1ULL << square);
    }

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

U64 AttackMaskB(int square, U64 occupancy) {
    U64 result = 0ULL;

    int file = GetFile(square);
    int rank = GetRank(square);

    for (int f = file + 1, r = rank + 1; f <= H && r <= Eight; f++, r++) {
        int square = f + r * 8;
        SetBit(result, square);
        if (IsBitSet(occupancy, square)) break;
    }

    for (int f = file + 1, r = rank - 1; f <= H && r >= One; f++, r--) {
        int square = f + r * 8;
        SetBit(result, square);
        if (IsBitSet(occupancy, square)) break;
    }

    for (int f = file - 1, r = rank + 1; f >= A && rank <= Eight; f--, r++) {
        int square = f + r * 8;
        SetBit(result, square);
        if (IsBitSet(occupancy, square)) break;
    }

    for (int f = file - 1, r = rank - 1; f >= A && r >= One; f--, r--) {
        int square = f + r * 8;
        SetBit(result, square);
        if (IsBitSet(occupancy, square)) break;
    }

    return result;
}

U64 AttackMaskR(int square, U64 occupancy) {
    U64 result = 0ULL;

    int file = GetFile(square);
    int rank = GetRank(square);

    for (int f = file + 1; f <= File::H; f++) {
        int square = f + rank * 8;
        SetBit(result, square);
        if (IsBitSet(occupancy, square)) break;
    }

    for (int f = file - 1; f >= File::A; f--) {
        int square = f + rank * 8;
        SetBit(result, square);
        if (IsBitSet(occupancy, square)) break;
    }

    for (int r = rank + 1; r <= Rank::Eight; r++) {
        int square = file + r * 8;
        SetBit(result, square);
        if (IsBitSet(occupancy, square)) break;
    }

    for (int r = rank - 1; r >= Rank::One; r--) {
        int square = file + r * 8;
        SetBit(result, square);
        if (IsBitSet(occupancy, square)) break;
    }

    return result;
}

U64 Board::GenerateBishopAttacks(int square, U64 occupancy) {
    occupancy &= MaskB(square);
    occupancy *= MagicsB[square];
    occupancy >>= 64 - RelevantBitsB[square];

    return bishopAttacks[square][occupancy];
}

U64 Board::GenerateRookAttacks(int square, U64 occupancy) {
    occupancy &= MaskR(square);
    occupancy *= MagicsR[square];
    occupancy >>= 64 - RelevantBitsR[square];

    return rookAttacks[square][occupancy];
}

U64 Board::GenerateQueenAttacks(int square, U64 occupancy) {
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
}