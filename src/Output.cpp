#include <iostream>
#include <cassert>
#include <stdlib.h>

#include "Board.h"
#include "Globals.h"
#include "Moves.h"

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

    static char squareString[3];

    sprintf(squareString, "%c%c", 'a' + file, '1' + rank);

    return squareString;
}

const char* ToMoveString(int move) {
    static char moveString[6];

    int fromSquare = GetFromSquare(move);
    int toSquare = GetToSquare(move);
    int promoted = GetPromotedPiece(move);

    char fromF = 'a' + GetFile(fromSquare);
    char fromR = '1' + GetRank(fromSquare);
    char toF = 'a' + GetFile(toSquare);
    char toR = '1' + GetRank(toSquare);

    char promotedChar = '\0';
    if (promoted) promotedChar = PIECE_CHAR[promoted];

    sprintf(moveString, "%c%c%c%c%c", fromF, fromR, toF, toR, promotedChar);

    return moveString;
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