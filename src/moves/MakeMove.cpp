#include <iostream>

#include "Board.h"
#include "Data.h"
#include "Globals.h"
#include "Move.h"
#include "MoveHandling.h"

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

bool Board::MakeMove(const int move, bool pseudoLegal) {
    history[ply].hashKey = hashKey;
    history[ply].move = move;
    history[ply].castlingPerms = castlingPerms;
    history[ply].fiftyMoveCount = fiftyMoveCount;
    history[ply].enPassant = enPassant;

    fiftyMoveCount++;
    ply++;

    int enemy = side ^ 1;
    int fromSquare = GetFromSquare(move);
    int toSquare = GetToSquare(move);
    int piece = GetMovedPiece(move);
    int promoted = GetPromotedPiece(move);

    if (IS_PAWN[piece]) fiftyMoveCount = 0;

    // HashCastling(this);
    hashKey ^= HashKeys::CastlingPermKeys[castlingPerms];
    castlingPerms &= CastlingPermsUpdate[fromSquare] & CastlingPermsUpdate[toSquare];
    // HashCastling(this);
    hashKey ^= HashKeys::CastlingPermKeys[castlingPerms];

    int _enPassant = enPassant;

    if (enPassant != NO_SQUARE) {
        // HashEnPassant(this);
        hashKey ^= HashKeys::EnPassantKeys[enPassant];
        enPassant = NO_SQUARE;
    }

    RemovePiece(this, piece, side, fromSquare);

    if (promoted) {
        AddPiece(this, promoted, side, toSquare);
        bigPieces[side]++;
    }
    else {
        AddPiece(this, piece, side, toSquare);
    }

    int flag = GetFlag(move);

    switch (flag) {
        case CAPTURE_FLAG: {
            int captured = GetCapturedPiece(move);
            RemovePiece(this, captured, enemy, toSquare);

            if (IS_PIECE_BIG[captured]) bigPieces[enemy]--;

            break;
        }

        case PAWNSTART_FLAG:
            if (side == WHITE) {
                enPassant = fromSquare + 8;
                // HashEnPassant(this);
                hashKey ^= HashKeys::EnPassantKeys[enPassant];
            }
            else {
                enPassant = toSquare + 8;
                // HashEnPassant(this);
                hashKey ^= HashKeys::EnPassantKeys[enPassant];
            }
        break;

        case ENPASSANT_FLAG:
            if (side == WHITE) {
                RemovePiece(this, BP, BLACK, _enPassant - 8);
            }
            else {
                RemovePiece(this, WP, WHITE, _enPassant + 8);
            }
        break;

        case CASTLING_FLAG:
            switch (toSquare) {
                case g1: MovePiece(this, WR, WHITE, h1, f1); break;
                case c1: MovePiece(this, WR, WHITE, a1, d1); break;
                case g8: MovePiece(this, BR, BLACK, h8, f8); break;
                case c8: MovePiece(this, BR, BLACK, a8, d8); break;
            }
        break;
    }

    occupancy[BOTH] = occupancy[WHITE] | occupancy[BLACK];

    int kingSquare = FirstBitIndex(bitboards[side == WHITE ? WK : BK]);

    side ^= 1;
    // HashSide(this);
    hashKey ^= HashKeys::SideKey;

    if (pseudoLegal && IsSquareAttacked(kingSquare, side)) {
        TakeMove();
        return false;
    }

    return true;
}

void Board::MakeNullMove() {
    history[ply].hashKey = hashKey;
    history[ply].enPassant = enPassant;

    ply++;

    side ^= 1;
    // HashSide(this);
    hashKey ^= HashKeys::SideKey;

    if (enPassant != NO_SQUARE) {
        // HashEnPassant(this);
        hashKey ^= HashKeys::EnPassantKeys[enPassant];
        enPassant = NO_SQUARE;
    }
}