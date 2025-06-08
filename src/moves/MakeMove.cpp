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

bool Board::MakeMove(const int move) {
    history[ply].hashKey = hashKey;
    history[ply].move = move;
    history[ply].castlingPerms = castlingPerms;
    history[ply].fiftyMoveCount = fiftyMoveCount++;
    history[ply].enPassant = enPassant;

    ply++;

    int enemy = side ^ 1;
    int fromSquare = GetFromSquare(move);
    int toSquare = GetToSquare(move);
    int piece = GetMovedPiece(move);
    int promoted = GetPromotedPiece(move);

    if (IS_PAWN[piece]) fiftyMoveCount = 0;

    HashCastling(hashKey, castlingPerms);
    castlingPerms &= CastlingPermsUpdate[fromSquare] & CastlingPermsUpdate[toSquare];
    HashCastling(hashKey, castlingPerms);

    int oldEnPassant = enPassant;

    if (enPassant != NO_SQUARE) {
        HashEnPassant(hashKey, enPassant);
        enPassant = NO_SQUARE;
    }

    ClearBit(bitboards[piece], fromSquare);
    ClearBit(occupancy[side], fromSquare);
    HashPiece(hashKey, piece, fromSquare);

    if (promoted) {
        SetBit(bitboards[promoted], toSquare);
        HashPiece(hashKey, promoted, toSquare);
        bigPieces[side]++;
    }
    else {
        SetBit(bitboards[piece], toSquare);
        HashPiece(hashKey, piece, toSquare);
    }

    SetBit(occupancy[side], toSquare);

    int flag = GetFlag(move);

    switch (flag) {
        case CAPTURE_FLAG: {
            int captured = GetCapturedPiece(move);
            ClearBit(bitboards[captured], toSquare);
            ClearBit(occupancy[enemy], toSquare);
            HashPiece(hashKey, captured, toSquare);

            if (IS_PIECE_BIG[captured]) bigPieces[enemy]--;

            break;
        }

        case PAWNSTART_FLAG:
            if (side == WHITE) {
                enPassant = fromSquare + 8;
                HashEnPassant(hashKey, enPassant);
            }
            else {
                enPassant = toSquare + 8;
                HashEnPassant(hashKey, enPassant);
            }
        break;

        case ENPASSANT_FLAG:
            if (side == WHITE) {
                ClearBit(bitboards[BP], oldEnPassant - 8);
                ClearBit(occupancy[BLACK], oldEnPassant - 8);
                HashPiece(hashKey, BP, oldEnPassant - 8);
            }
            else {
                ClearBit(bitboards[WP], oldEnPassant + 8);
                ClearBit(occupancy[WHITE], oldEnPassant + 8);
                HashPiece(hashKey, WP, oldEnPassant + 8);
            }
        break;

        case CASTLING_FLAG:
            switch (toSquare) {
                case g1: MoveRook(hashKey, bitboards[WR], occupancy[WHITE], WR, h1, f1); break;
                case c1: MoveRook(hashKey, bitboards[WR], occupancy[WHITE], WR, a1, d1); break;
                case g8: MoveRook(hashKey, bitboards[BR], occupancy[BLACK], BR, h8, f8); break;
                case c8: MoveRook(hashKey, bitboards[BR], occupancy[BLACK], BR, a8, d8); break;
            }
        break;
    }

    occupancy[BOTH] = occupancy[WHITE] | occupancy[BLACK];

    int kingSquare = FirstBitIndex(bitboards[side == WHITE ? WK : BK]);

    side ^= 1;
    HashSide(hashKey);

    if (IsSquareAttacked(kingSquare, side)) {
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
    HashSide(hashKey);

    if (enPassant != NO_SQUARE) {
        HashEnPassant(hashKey, enPassant);
        enPassant = NO_SQUARE;
    }
}