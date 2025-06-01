#pragma once

#include <array>
#include <cstdint>
#include <iostream>

#include "Data.h"
#include "Globals.h"
#include "Moves.h"

struct BoardInfo {
    U64 hashKey;
    int move;
    int castlingPerms;
    int enPassant = NO_SQUARE;
};

class Board {
    public:

    Board();

    void Print();
    void GenerateHashKey();
    void ParseFEN(const char* fen);

    const char* GenerateFEN();

    void PerftTest(int depth);

    void Check(); // debugging function
    bool CheckDraw();

    void GenerateMoves(MoveList& list);
    void GenerateCaptures(MoveList& list);

    bool MakeMove(int move);
    bool MakeMove(const char* move);
    bool MakeMove(std::string move);
    void TakeMove();

    // below here could be private

    void InitAttackTables();

    bool IsSquareAttacked(int square, int side);
    bool IsSquaresAttacked(U64 squaresMask, int side);

    U64 GetBishopAttacks(int square, U64 occupancy);
    U64 GetRookAttacks(int square, U64 occupancy);
    U64 GetQueenAttacks(int square, U64 occupancy);

    U64 Perft(int depth);

    U64 pawnMoves[2][64];
    U64 pawnCaptures[2][64];
    U64 knightAttacks[64];
    U64 bishopAttacks[64][512];
    U64 rookAttacks[64][4096];
    U64 queenAttacks[64]; // not sure how to deal with this yet
    U64 kingAttacks[64];

    U64 bitboards[12];
    U64 occupancy[3];

    U64 hashKey;

    std::array<BoardInfo, MAX_GAME_MOVES> history;

    int ply = 0;
    int fiftyMoveCounter = 0;
    int enPassant = NO_SQUARE;
    int side = Side::White;
    int castlingPerms = 0;
};