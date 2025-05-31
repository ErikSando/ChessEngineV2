#include <iostream>

#include "Attacks.h"
#include "Board.h"

U64 GeneratePawnMoves(int side, int square) {
    U64 attacks = 0ULL;

    return attacks;
}

U64 GenerateKnightMoves(int square) {
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

U64 GenerateKingMoves(int square) {
    U64 attacks = 0ULL;

    return attacks;
}

U64 GenerateBishopMoves(int square) {
    U64 attacks = 0ULL;

    return attacks;
}

U64 GenerateRookMoves(int square) {
    U64 attacks = 0ULL;

    return attacks;
}

U64 GenerateQueenMoves(int square) {
    U64 attacks = 0ULL;

    return attacks;
}

void Board::InitAttackTables() {
    for (int square = 0; square < 64; square++) {
        pawnMoves[White][square] = GeneratePawnMoves(White, square);
        pawnMoves[Black][square] = GeneratePawnMoves(Black, square);
        knightMoves[square] = GenerateKnightMoves(square);
        bishopMoves[square] = GenerateBishopMoves(square);
        rookMoves[square] = GenerateRookMoves(square);
        queenMoves[square] = GenerateQueenMoves(square);
        kingMoves[square] = GenerateKingMoves(square);
    
        PrintBitboard(knightMoves[square]);
        std::cout << "\n";
    }
}