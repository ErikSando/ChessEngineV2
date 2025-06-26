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

        unsigned int state = 1804289383;

        unsigned int RandU32() {
            state ^= state << 13;
            state ^= state >> 17;
            state ^= state << 5;
            return state;
        }

        U64 GenerateHashKey() {
            U64 u1, u2, u3, u4;
            u1 = (U64) (RandU32()) & 0xFFFF;
            u2 = (U64) (RandU32()) & 0xFFFF;
            u3 = (U64) (RandU32()) & 0xFFFF;
            u4 = (U64) (RandU32()) & 0xFFFF;
            return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
        }

        void Init() {
            srand(time(nullptr));

            for (int square = 0; square < 64; square++) {
                for (int piece = Piece::WP; piece <= Piece::BK; piece++) {
                    PieceKeys[piece][square] = GenerateHashKey();
                }

                EnPassantKeys[square] = GenerateHashKey();
            }

            for (int castling = 0; castling < 16; castling++) {
                CastlingPermKeys[castling] = GenerateHashKey();
            }

            SideKey = GenerateHashKey();
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