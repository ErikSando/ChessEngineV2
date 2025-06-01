#pragma once

#include <array>
#include <cstdint>

#include "Globals.h"
#include "Moves.h"

enum Side {
    White, Black, Both
};

enum Piece {
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK
};

constexpr int NO_SQUARE = -1;
constexpr int NO_PIECE = 12;
constexpr int MAX_GAME_MOVES = 2048;

constexpr const char* START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

constexpr char PIECE_CHAR[13] = { 'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k', '.' };

constexpr int PIECE_SIDE[12] = {
    White, White, White, White, White, White,
    Black, Black, Black, Black, Black, Black
};

inline int PieceID(char piece) {
    switch (piece) {
        case 'P': return WP;
        case 'N': return WN;
        case 'B': return WB;
        case 'R': return WR;
        case 'Q': return WQ;
        case 'K': return WK;
        case 'p': return BP;
        case 'n': return BN;
        case 'b': return BB;
        case 'r': return BR;
        case 'q': return BQ;
        case 'k': return BK;
        default: return NO_PIECE;
    }
}

enum Castling {
    WKC = 1, WQC = 2,
    BKC = 4, BQC = 8
};

enum File {
    A, B, C, D, E, F, G, H
};

enum Rank {
    One, Two, Three, Four, Five, Six, Seven, Eight
};

enum Square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};

struct BoardInfo {
    int enPassant;
    U64 key;
};

class Board {
    public:

    Board();

    void Print();
    void GeneratePositionKey();
    void ParseFEN(const char* fen);

    const char* GenerateFEN();

    bool CheckDraw();

    void GenerateMoves(MoveList& list);
    void GenerateCaptures(MoveList& list);

    bool MakeMove(int move);
    void TakeMove();

    private:

    void InitAttackTables();

    bool IsSquareAttacked(int square, int side);
    bool IsSquaresAttacked(U64 squaresMask, int side);

    U64 GetBishopAttacks(int square, U64 occupancy);
    U64 GetRookAttacks(int square, U64 occupancy);
    U64 GetQueenAttacks(int square, U64 occupancy);

    U64 pawnMoves[2][64];
    U64 pawnCaptures[2][64];
    U64 knightAttacks[64];
    U64 bishopAttacks[64][512];
    U64 rookAttacks[64][4096];
    U64 queenAttacks[64]; // not sure how to deal with this yet
    U64 kingAttacks[64];

    U64 bitboards[12];
    U64 occupancy[3];

    U64 positionKey;

    std::array<BoardInfo, MAX_GAME_MOVES> history;

    int move = 0;
    int fiftyMoveCounter = 0;
    int enPassant = NO_SQUARE;
    int side = Side::White;
    int castlingPerms = 0;
};