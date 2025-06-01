#pragma once

constexpr int NO_SQUARE = -1;
constexpr int MAX_GAME_MOVES = 2048;

enum Side {
    White, Black, Both
};

enum Piece {
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK, NO_PIECE
};

constexpr const char* START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

constexpr char PIECE_CHAR[13] = { 'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k', '.' };

constexpr int PIECE_SIDE[12] = {
    White, White, White, White, White, White,
    Black, Black, Black, Black, Black, Black
};

constexpr bool IS_PAWN[12] =   {  true, false, false, false, false, false,  true, false, false, false, false, false };
constexpr bool IS_KNIGHT[12] = { false,  true, false, false, false, false, false,  true, false, false, false, false };
constexpr bool IS_BISHOP[12] = { false, false,  true, false, false, false, false, false,  true, false, false, false };
constexpr bool IS_ROOK[12] =   { false, false, false,  true, false, false, false, false, false,  true, false, false };
constexpr bool IS_QUEEN[12] =  { false, false, false, false,  true, false, false, false, false, false,  true, false };
constexpr bool IS_KING[12] =   { false, false, false, false, false,  true, false, false, false, false, false,  true };

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
    FileA, FileB, FileC, FileD, FileE, FileF, FileG, FileH
};

enum Rank {
    Rank1, Rank2, Rank3, Rank4, Rank5, Rank6, Rank7, Rank8
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