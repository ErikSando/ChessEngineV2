#include <ctime>

#include "Board.h"
#include "Globals.h"

U64 PieceKeys[12][64];
U64 CastlingKeys[16];
U64 EnPassantKeys[64];
U64 SideKey;
bool keysGenerated = false;

void InitHashKeys() {
    srand(time(nullptr));

    for (int square = 0; square < 64; square++) {
        for (int piece = Piece::WP; piece <= Piece::BK; piece++) {
            PieceKeys[piece][square] = RandU64();
        }

        EnPassantKeys[square] = RandU64();
    }

    for (int castling = 0; castling < 16; castling++) {
        CastlingKeys[castling] = RandU64();
    }

    SideKey = RandU64();

    keysGenerated = true;
}

void Board::GenerateHashKey() {
    hashKey = 0ULL;

    for (int piece = WP; piece <= BK; piece++) {
        U64 bitboard = bitboards[piece];

        while (bitboard) {
            int square = PopFirstBit(bitboard);
            hashKey ^= PieceKeys[piece][square];
        }
    }

    hashKey ^= CastlingKeys[castlingPerms];

    if (enPassant != NO_SQUARE) hashKey ^= EnPassantKeys[enPassant];
    if (side == Black) hashKey ^= SideKey;
}