#include <iostream>
#include <cassert>
#include <stdlib.h>

#include "Globals.h"

int ToSquare(const char* square) {
    assert(square[0] >= 'a' && square[0] <= 'h');
    assert(square[1] >= '1' && square[1] <= '8');

    int file = square[0] - 'a';
    int rank = square[1] - '1';

    return GetSquare(file, rank);
}

const char* ToSquareString(int square) {
    int file = GetFile(square);
    int rank = GetRank(square);

    char fileChar = 'a' + file;
    char rankChar = '1' + rank;

    static char* squareString = new char[3];

    squareString[0] = fileChar;
    squareString[1] = rankChar;
    squareString[2] = '\0';

    return squareString;
}

const char* ToMoveString(int move) {
    return "";
}

void PrintBitboard(U64 bitboard) {
    for (int row = 7; row >= 0; row--) {
        for (int col = 0; col < 8; col++) {
            int bit = row * 8 + col;
            std::cout << " " << GetBit(bitboard, bit);
        }

        std::cout << "\n";
    }
}