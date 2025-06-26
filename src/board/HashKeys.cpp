#include <ctime>

#include "Board.h"
#include "Globals.h"
#include "HashKeys.h"

namespace ErikEngine {
    namespace HashKeys {
        U64 PieceKeys[12][64];
        U64 CastlingPermKeys[16];
        U64 EnPassantKeys[64];
        U64 SideKey;

        void Init() {
            srand(time(nullptr));

            for (int square = 0; square < 64; square++) {
                for (int piece = Piece::WP; piece <= Piece::BK; piece++) {
                    PieceKeys[piece][square] = RandU64();
                }

                EnPassantKeys[square] = RandU64();
            }

            for (int castling = 0; castling < 16; castling++) {
                CastlingPermKeys[castling] = RandU64();
            }

            SideKey = RandU64();
        }

        U64 GenerateHashKey(Board* board) {
            U64 hashKey = 0ULL;

            for (int piece = WP; piece <= BK; piece++) {
                U64 bitboard = board->bitboards[piece];

                while (bitboard) {
                    int square = PopFirstBit(bitboard);
                    hashKey ^= PieceKeys[piece][square];
                }
            }

            hashKey ^= CastlingPermKeys[board->castlingPerms];

            if (board->enPassant != NO_SQUARE) hashKey ^= EnPassantKeys[board->enPassant];
            if (board->side == BLACK) hashKey ^= SideKey;

            return hashKey;
        }
    }
}