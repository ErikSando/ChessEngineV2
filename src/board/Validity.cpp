// #include <cassert>
#include <iostream>

#include "Board.h"
#include "Debug.h"
#include "Utils.h"

// assert expressions are given long names to be very explicit in what it is checking if printed

namespace ErikEngine {
    bool Board::IsValid() const {
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