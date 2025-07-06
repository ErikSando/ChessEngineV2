#include <string_view>

#include "Board.h"
#include "Debug.h"
#include "Globals.h"

namespace ErikEngine {
    const char* FILE_NAME(std::string_view path) {
        /* ./src/.../name.cpp
                ^ want here onwards */
        size_t src_slash = find_nth_occurence(2, "/\\", path);
        return src_slash == std::string_view::npos ? path.data() : path.substr(src_slash + 1).data();
    }

    namespace Debug {
        void AssertValid(const Board& board) {
            int nWK = CountBits(board.bitboards[WK]);
            int nBK = CountBits(board.bitboards[BK]);

            if (nWK != 1 || nBK != 1) board.Print();

            EE_ASSERT(nWK == 1);
            EE_ASSERT(nBK == 1);

            int white_king_square = FirstBitIndex(board.bitboards[WK]);
            int black_king_square = FirstBitIndex(board.bitboards[BK]);

            int ekpos = board.side == WHITE ? black_king_square : white_king_square;

            bool king_can_be_captured = board.IsSquareAttacked(ekpos, board.side);

            if (king_can_be_captured) board.Print();
            EE_ASSERT(!king_can_be_captured);

            int number_of_white_pawns = CountBits(board.bitboards[WP]);
            int number_of_black_pawns = CountBits(board.bitboards[BP]);

            if (number_of_white_pawns > 8 || number_of_black_pawns > 8) board.Print();

            EE_ASSERT(number_of_white_pawns <= 8); // won't bother checking for negative pawns
            EE_ASSERT(number_of_black_pawns <= 8);

            if (board.enPassant != NO_SQUARE) {
                int en_passant_square_rank = GetRank(board.enPassant);
                int correct_en_passant_rank = board.side == WHITE ? RANK_6 : RANK_3;

                if (en_passant_square_rank != correct_en_passant_rank) board.Print();
                EE_ASSERT(en_passant_square_rank == correct_en_passant_rank);

                int dir = board.side == WHITE ? -8 : 8;
                int p = board.side == WHITE ? BP : WP;

                bool en_passant_square_is_above_or_below_pawn = IsBitSet(board.bitboards[p], board.enPassant + dir);

                if (!en_passant_square_is_above_or_below_pawn) board.Print();
                EE_ASSERT(en_passant_square_is_above_or_below_pawn);
            }

            if (board.castlingPerms & WKC) {
                EE_ASSERT(white_king_square == e1);
                bool rook_for_castling_exists = IsBitSet(board.bitboards[WR], h1);
                (void) rook_for_castling_exists; // suppress unused warning in non-debug build
                EE_ASSERT(rook_for_castling_exists);
            }

            if (board.castlingPerms & WQC) {
                EE_ASSERT(white_king_square == e1);
                bool rook_for_castling_exists = IsBitSet(board.bitboards[WR], a1);
                (void) rook_for_castling_exists;
                EE_ASSERT(rook_for_castling_exists);
            }

            if (board.castlingPerms & BKC) {
                EE_ASSERT(black_king_square == e8);
                bool rook_for_castling_exists = IsBitSet(board.bitboards[BR], h8);
                (void) rook_for_castling_exists;
                EE_ASSERT(rook_for_castling_exists);
            }

            if (board.castlingPerms & BQC) {
                EE_ASSERT(black_king_square == e8);
                bool rook_for_castling_exists = IsBitSet(board.bitboards[BR], a8);
                (void) rook_for_castling_exists;
                EE_ASSERT(rook_for_castling_exists);
            }
        }
    }
}