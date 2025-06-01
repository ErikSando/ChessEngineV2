#include <cassert>
#include <iostream>

#include "Board.h"

void Board::Check() {
    int nWK = CountBits(bitboards[WK]);
    int nBK = CountBits(bitboards[BK]);

    assert(nWK == 1);
    assert(nBK == 1);

    int wkpos = FirstBitIndex(bitboards[WK]);
    int bkpos = FirstBitIndex(bitboards[BK]);

    int ekpos = FirstBitIndex(bitboards[side == White ? BK : WK]);

    if (IsSquareAttacked(ekpos, side ^ 1)) Print();
    assert(!IsSquareAttacked(ekpos, side ^ 1));

    int nWP = CountBits(bitboards[WP]);
    int nBP = CountBits(bitboards[BP]);

    if (!(nWP <= 8 && nBP <= 8)) {
        Print(); std::cout << "\n";
        std::cout << ToMoveString(history[ply - 1].move) << "\n";
        TakeMove();
        //Print(); std::cout << "\n";
        std::cout << ToMoveString(history[ply - 1].move) << "\n";
        TakeMove();
        //Print(); std::cout << "\n";
        std::cout << ToMoveString(history[ply - 1].move) << "\n";
        TakeMove();
        //Print(); std::cout << "\n";
    }
    assert(nWP <= 8);
    assert(nBP <= 8);

    if (enPassant != NO_SQUARE) {
        int rank = GetRank(enPassant);

        assert(side == White ? rank == Rank6 : rank == Rank3);

        int dir = side == White ? -8 : 8;
        int p = side == White ? BP : WP;

        if (!IsBitSet(bitboards[p], enPassant + dir)) Print();
        assert(IsBitSet(bitboards[p], enPassant + dir));
    }

    if (castlingPerms & WKC) {
        assert(wkpos == E1);
        assert(IsBitSet(bitboards[WR], H1));
    }

    if (castlingPerms & WQC) {
        assert(wkpos == E1);
        assert(IsBitSet(bitboards[WR], A1));
    }

    if (castlingPerms & BKC) {
        assert(bkpos == E8);
        assert(IsBitSet(bitboards[BR], H8));
    }

    if (castlingPerms & BQC) {
        assert(bkpos == E8);
        assert(IsBitSet(bitboards[BR], A8));
    }
}