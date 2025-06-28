#pragma once

#include "Globals.h"
#include "Move.h"
#include "MoveHandling.h"

namespace ErikEngine {
    constexpr int CastlingPermsUpdate[64] = {
        13, 15, 15, 15, 12, 15, 15, 14,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        7, 15, 15, 15,  3, 15, 15, 11
    };

    template<MoveContext moveContext, bool pseudoLegal>
    inline bool Board::MakeMove(const int move) {
        constexpr bool updateHash = (moveContext != MoveContext::Perft);

        if (updateHash) history[ply].hashKey = hashKey;
        history[ply].move = move;
        history[ply].castlingPerms = castlingPerms;
        if (updateHash) history[ply].fiftyMoveCount = fiftyMoveCount;
        history[ply].enPassant = enPassant;

        if (updateHash) fiftyMoveCount++;
        ply++;

        int enemy = side ^ 1;
        int fromSquare = GetFromSquare(move);
        int toSquare = GetToSquare(move);
        int piece = GetMovedPiece(move);
        int promoted = GetPromotedPiece(move);

        if (updateHash) if (IS_PAWN[piece]) fiftyMoveCount = 0;

        if (updateHash) hashKey ^= HashKeys::CastlingPermKeys[castlingPerms];
        castlingPerms &= CastlingPermsUpdate[fromSquare] & CastlingPermsUpdate[toSquare];
        if (updateHash) hashKey ^= HashKeys::CastlingPermKeys[castlingPerms];

        int _enPassant = enPassant;

        if (enPassant != NO_SQUARE) {
            if (updateHash) hashKey ^= HashKeys::EnPassantKeys[enPassant];
            enPassant = NO_SQUARE;
        }

        RemovePiece<updateHash>(this, piece, side, fromSquare);

        if (promoted) {
            AddPiece<updateHash>(this, promoted, side, toSquare);
            bigPieces[side]++;
        }
        else {
            AddPiece<updateHash>(this, piece, side, toSquare);
        }

        int flag = GetFlag(move);

        switch (flag) {
            case CAPTURE_FLAG: {
                int captured = GetCapturedPiece(move);
                RemovePiece<updateHash>(this, captured, enemy, toSquare);
                if (!IS_PAWN[captured]) bigPieces[enemy]--;
                break;
            }

            case PAWNSTART_FLAG:
                if (side == WHITE) {
                    enPassant = fromSquare + 8;
                    if (updateHash) hashKey ^= HashKeys::EnPassantKeys[enPassant];
                }
                else {
                    enPassant = toSquare + 8;
                    if (updateHash) hashKey ^= HashKeys::EnPassantKeys[enPassant];
                }
            break;

            case ENPASSANT_FLAG:
                if (side == WHITE) {
                    RemovePiece<updateHash>(this, BP, BLACK, _enPassant - 8);
                }
                else {
                    RemovePiece<updateHash>(this, WP, WHITE, _enPassant + 8);
                }
            break;

            case CASTLING_FLAG:
                switch (toSquare) {
                    case g1: MovePiece<updateHash>(this, WR, WHITE, h1, f1); break;
                    case c1: MovePiece<updateHash>(this, WR, WHITE, a1, d1); break;
                    case g8: MovePiece<updateHash>(this, BR, BLACK, h8, f8); break;
                    case c8: MovePiece<updateHash>(this, BR, BLACK, a8, d8); break;
                }
            break;
        }

        occupancy[BOTH] = occupancy[WHITE] | occupancy[BLACK];

        int kingSquare = FirstBitIndex(bitboards[side == WHITE ? WK : BK]);

        side ^= 1;
        if (updateHash) hashKey ^= HashKeys::SideKey;

        if constexpr (pseudoLegal) {
            if (IsSquareAttacked(kingSquare, side)) {
                TakeMove<moveContext>();
                return false;
            }
        }

        return true;
    }

    inline void Board::MakeNullMove() {
        history[ply].hashKey = hashKey;
        history[ply].enPassant = enPassant;

        ply++;

        side ^= 1;
        hashKey ^= HashKeys::SideKey;

        if (enPassant != NO_SQUARE) {
            hashKey ^= HashKeys::EnPassantKeys[enPassant];
            enPassant = NO_SQUARE;
        }
    }
}