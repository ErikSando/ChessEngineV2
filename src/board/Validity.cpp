#include <cassert>
#include <iostream>

#include "Board.h"
#include "Utils.h"

void Board::CheckValid() {
    int nWK = CountBits(bitboards[WK]);
    int nBK = CountBits(bitboards[BK]);

    if (nWK != 1 || nBK != 1) {
        Print();
    }

    assert(nWK == 1);
    assert(nBK == 1);

    int wkpos = FirstBitIndex(bitboards[WK]);
    int bkpos = FirstBitIndex(bitboards[BK]);

    // int ekpos = FirstBitIndex(bitboards[side == WHITE ? BK : WK]);
    int ekpos = side == WHITE ? bkpos : wkpos;

    if (IsSquareAttacked(ekpos, side)) Print();
    assert(!IsSquareAttacked(ekpos, side));

    int nWP = CountBits(bitboards[WP]);
    int nBP = CountBits(bitboards[BP]);

    if (!(nWP <= 8 && nBP <= 8)) {
        Print();
    }
    
    assert(nWP <= 8); // won't bother checking for negative pawns
    assert(nBP <= 8);

    if (enPassant != NO_SQUARE) {
        int rank = GetRank(enPassant);

        assert(side == WHITE ? rank == RANK_6 : rank == RANK_3);

        int dir = side == WHITE ? -8 : 8;
        int p = side == WHITE ? BP : WP;

        if (!IsBitSet(bitboards[p], enPassant + dir)) Print();
        assert(IsBitSet(bitboards[p], enPassant + dir));
    }

    if (castlingPerms & WKC) {
        assert(wkpos == e1);
        assert(IsBitSet(bitboards[WR], h1));
    }

    if (castlingPerms & WQC) {
        assert(wkpos == e1);
        assert(IsBitSet(bitboards[WR], a1));
    }

    if (castlingPerms & BKC) {
        assert(bkpos == e8);
        assert(IsBitSet(bitboards[BR], h8));
    }

    if (castlingPerms & BQC) {
        assert(bkpos == e8);
        assert(IsBitSet(bitboards[BR], a8));
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