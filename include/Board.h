#pragma once

#include "Data.h"
#include "Globals.h"

struct BoardInfo {
    U64 hashKey;
    int move;
    int castlingPerms;
    int fiftyMoveCount;
    int enPassant = NO_SQUARE;
};

class Board {
    public:

    Board();

    void Print();
    void ParseFEN(const char* fen);

    const char* GenerateFEN();

    void CheckValid();
    bool CheckValidQuiet(); // no asserts

    //bool CheckDraw(); // dont think I need this here

    bool MakeMove(const int move);
    void TakeMove();

    bool IsSquareAttacked(int square);
    bool IsSquareAttacked(int square, int side);

    U64 bitboards[12];
    U64 occupancy[3];

    U64 hashKey;

    BoardInfo history[MAX_GAME_MOVES];

    int ply = 0;
    int fiftyMoveCount = 0;
    int enPassant = NO_SQUARE;
    int side = WHITE;
    int castlingPerms = 0;
};