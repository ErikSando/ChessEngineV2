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
constexpr int MAX_GAME_Attacks = 2048;

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
    A1, A2, A3, A4, A5, A6, A7, A8,
    B1, B2, B3, B4, B5, B6, B7, B8,
    C1, C2, C3, C4, C5, C6, C7, C8,
    D1, D2, D3, D4, D5, D6, D7, D8,
    E1, E2, E3, E4, E5, E6, E7, E8,
    F1, F2, F3, F4, F5, F6, F7, F8,
    G1, G2, G3, G4, G5, G6, G7, G8,
    H1, H2, H3, H4, H5, H6, H7, H8
};

struct BoardInfo {
    int enPassant;
    U64 key;
};

class Board {
    public:

    Board();

    void GeneratePositionKey();

    void ParseFEN(const char* fen);
    void Print();

    const char* GenerateFEN();

    bool CheckDraw();

    void GenerateMoves(MoveList& list);
    void GenerateCaptures(MoveList& list);

    bool MakeMove(int move);
    void TakeMove();

    private:

    void InitAttackTables();

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

    std::array<BoardInfo, MAX_GAME_Attacks> history;

    int move = 0;
    int fiftyMoveCounter = 0;
    int enPassant = NO_SQUARE;
    int side = Side::White;
    int castlingPerms = 0;
};