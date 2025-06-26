// #include <cassert>
#include <iostream>

#include "Board.h"
#include "Debug.h"
#include "Utils.h"

// assert expressions are given long names to be very explicit in what it is checking if printed

namespace ErikEngine {
    void Board::AssertValid() {
        int nWK = CountBits(bitboards[WK]);
        int nBK = CountBits(bitboards[BK]);

        if (nWK != 1 || nBK != 1) Print();

        EE_ASSERT(nWK == 1);
        EE_ASSERT(nBK == 1);

        int white_king_square = FirstBitIndex(bitboards[WK]);
        int black_king_square = FirstBitIndex(bitboards[BK]);

        int ekpos = side == WHITE ? black_king_square : white_king_square;

        bool king_can_be_captured = IsSquareAttacked(ekpos, side);

        if (king_can_be_captured) Print();
        EE_ASSERT(!king_can_be_captured);

        int number_of_white_pawns = CountBits(bitboards[WP]);
        int number_of_black_pawns = CountBits(bitboards[BP]);

        if (number_of_white_pawns > 8 || number_of_black_pawns > 8) Print();

        EE_ASSERT(number_of_white_pawns <= 8); // won't bother checking for negative pawns
        EE_ASSERT(number_of_black_pawns <= 8);

        if (enPassant != NO_SQUARE) {
            int en_passant_square_rank = GetRank(enPassant);
            int correct_en_passant_rank = side == WHITE ? RANK_6 : RANK_3;

            EE_ASSERT(en_passant_square_rank == correct_en_passant_rank);

            int dir = side == WHITE ? -8 : 8;
            int p = side == WHITE ? BP : WP;

            bool en_passant_square_is_above_or_below_pawn = IsBitSet(bitboards[p], enPassant + dir);

            if (!en_passant_square_is_above_or_below_pawn) Print();
            EE_ASSERT(en_passant_square_is_above_or_below_pawn);
        }

        if (castlingPerms & WKC) {
            EE_ASSERT(white_king_square == e1);
            bool rook_for_castling_exists = IsBitSet(bitboards[WR], h1);
            EE_ASSERT(rook_for_castling_exists);
        }

        if (castlingPerms & WQC) {
            EE_ASSERT(white_king_square == e1);
            bool rook_for_castling_exists = IsBitSet(bitboards[WR], a1);
            EE_ASSERT(rook_for_castling_exists);
        }

        if (castlingPerms & BKC) {
            EE_ASSERT(black_king_square == e8);
            bool rook_for_castling_exists = IsBitSet(bitboards[BR], h8);
            EE_ASSERT(rook_for_castling_exists);
        }

        if (castlingPerms & BQC) {
            EE_ASSERT(black_king_square == e8);
            bool rook_for_castling_exists = IsBitSet(bitboards[BR], a8);
            EE_ASSERT(rook_for_castling_exists);
        }
    }

    bool Board::IsValid() {
        int nWK = CountBits(bitboards[WK]);
        int nBK = CountBits(bitboards[BK]);

        if (nWK != 1 || nBK != 1) return false;

        int wkpos = FirstBitIndex(bitboards[WK]);
        int bkpos = FirstBitIndex(bitboards[BK]);

        int ekpos = side == WHITE ? bkpos : wkpos;

        if (IsSquareAttacked(ekpos, side)) return false;

        int nWP = CountBits(bitboards[WP]);
        int nBP = CountBits(bitboards[BP]);

        if (nWP > 8 || nBP > 8) return false;

        if (enPassant != NO_SQUARE) {
            int rank = GetRank(enPassant);

            if (side == WHITE ? rank != RANK_6 : rank != RANK_3) return false;

            int dir = side == WHITE ? -8 : 8;
            int p = side == WHITE ? BP : WP;

            if (!IsBitSet(bitboards[p], enPassant + dir)) return false;
        }

        if (castlingPerms & WKC) {
            if (wkpos != e1 || !IsBitSet(bitboards[WR], h1)) return false;
        }

        if (castlingPerms & WQC) {
            if (wkpos != e1 || !IsBitSet(bitboards[WR], a1)) return false;
        }

        if (castlingPerms & BKC) {
            if (bkpos != e8 || !IsBitSet(bitboards[BR], h8)) return false;
        }

        if (castlingPerms & BQC) {
            if (bkpos != e8 || !IsBitSet(bitboards[BR], a8)) return false;
        }

        return true;
    }
}