#pragma once

constexpr int NO_SQUARE = -1;
constexpr int MAX_GAME_MOVES = 2048;

constexpr int MAX_DEPTH = 64;
constexpr int INF = 10000;
constexpr int MATE_SCORE = INF - MAX_DEPTH;
constexpr int NO_SCORE = INF + 1;

enum Side {
    WHITE, BLACK, BOTH
};

enum Piece {
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK, NO_PIECE
};

enum PieceType {
    P, N, B, R, Q, K
};

constexpr const char* START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

constexpr char PIECE_CHAR[14] = "PNBRQKpnbrqk.";

constexpr int PIECE_SIDE[12] = {
    WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
    BLACK, BLACK, BLACK, BLACK, BLACK, BLACK
};

constexpr int PIECE_TYPE[12] = { P, N, B, R, Q, K, P, N, B, R, Q, K };

constexpr bool IS_PAWN[12] =      {  true, false, false, false, false, false,  true, false, false, false, false, false };
constexpr bool IS_KNIGHT[12] =    { false,  true, false, false, false, false, false,  true, false, false, false, false };
constexpr bool IS_BISHOP[12] =    { false, false,  true, false, false, false, false, false,  true, false, false, false };
constexpr bool IS_ROOK[12] =      { false, false, false,  true, false, false, false, false, false,  true, false, false };
constexpr bool IS_QUEEN[12] =     { false, false, false, false,  true, false, false, false, false, false,  true, false };
constexpr bool IS_KING[12] =      { false, false, false, false, false,  true, false, false, false, false, false,  true };

constexpr bool IS_PIECE_BIG[12] = { false,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true };

constexpr bool IS_PIECE_SLIDER[12] =    { false, false,  true,  true,  true, false, false, false,  true,  true,  true, false };

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
    }

    return NO_PIECE;
}

enum CastlingPerms {
    WKC = 1, WQC = 2,
    BKC = 4, BQC = 8
};

enum File {
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H
};

enum Rank {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8
};

enum Square {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};