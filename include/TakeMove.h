#pragma once

#include <cassert>

#include "Globals.h"
#include "Move.h"
#include "MoveHandling.h"

namespace ErikEngine {
    template<MoveContext moveContext>
    inline void Board::TakeMove() {
        constexpr bool inSearch = moveContext == MoveContext::Search;

        ply--;

        assert(ply >= 0);

        int enemy = side;

        if (inSearch) hashKey = history[ply].hashKey;
        castlingPerms = history[ply].castlingPerms;
        if (inSearch) fiftyMoveCount = history[ply].fiftyMoveCount;
        enPassant = history[ply].enPassant;
        side ^= 1;

        int move = history[ply].move;

        int fromSquare = GetFromSquare(move);
        int toSquare = GetToSquare(move);
        int piece = GetMovedPiece(move);
        int promoted = GetPromotedPiece(move);

        if (promoted) {
            RemovePiece<false>(this, promoted, side, toSquare);
            bigPieces[side]--;
        }
        else {
            RemovePiece<false>(this, piece, side, toSquare);
        }

        AddPiece<false>(this, piece, side, fromSquare);

        int flag = GetFlag(move);

        switch (flag) {
            case CAPTURE_FLAG: {
                int captured = GetCapturedPiece(move);
                AddPiece<false>(this, captured, enemy, toSquare);
                if (!IS_PAWN[captured]) bigPieces[enemy]++;
                break;
            }

            case ENPASSANT_FLAG:
                if (side == WHITE) {
                    AddPiece<false>(this, BP, BLACK, toSquare - 8);
                }
                else {
                    AddPiece<false>(this, WP, WHITE, toSquare + 8);
                }
            break;

            case CASTLING_FLAG:
                switch (toSquare) {
                    case g1: MovePiece<false>(this, WR, WHITE, f1, h1); break;
                    case c1: MovePiece<false>(this, WR, WHITE, d1, a1); break;
                    case g8: MovePiece<false>(this, BR, BLACK, f8, h8); break;
                    case c8: MovePiece<false>(this, BR, BLACK, d8, a8); break;
                }
            break;
        }

        occupancy[BOTH] = occupancy[WHITE] | occupancy[BLACK];
    }

    inline void Board::TakeNullMove() {
        ply--;

        hashKey = history[ply].hashKey;
        enPassant = history[ply].enPassant;

        side ^= 1;
    }
}