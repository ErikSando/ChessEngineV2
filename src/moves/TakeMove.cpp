#include <cassert>

#include "Board.h"
#include "Move.h"
#include "MoveHandling.h"

namespace ErikEngine {
    void Board::TakeMove() {
        ply--;

        assert(ply >= 0);

        int enemy = side;

        hashKey = history[ply].hashKey;
        castlingPerms = history[ply].castlingPerms;
        fiftyMoveCount = history[ply].fiftyMoveCount;
        enPassant = history[ply].enPassant;
        side ^= 1;

        int move = history[ply].move;

        int fromSquare = GetFromSquare(move);
        int toSquare = GetToSquare(move);
        int piece = GetMovedPiece(move);
        int promoted = GetPromotedPiece(move);

        if (promoted) {
            ClearBit(bitboards[promoted], toSquare);
        }
        else {
            ClearBit(bitboards[piece], toSquare);
        }

        ClearBit(occupancy[side], toSquare);

        SetBit(bitboards[piece], fromSquare);
        SetBit(occupancy[side], fromSquare);

        int flag = GetFlag(move);

        switch (flag) {
            case CAPTURE_FLAG: {
                int captured = GetCapturedPiece(move);
                SetBit(bitboards[captured], toSquare);
                SetBit(occupancy[enemy], toSquare);

                if (!IS_PAWN[captured]) bigPieces[enemy]++;

                break;
            }

            case ENPASSANT_FLAG:
                if (side == WHITE) {
                    SetBit(bitboards[BP], toSquare - 8);
                    SetBit(occupancy[BLACK], toSquare - 8);
                }
                else {
                    SetBit(bitboards[WP], toSquare + 8);
                    SetBit(occupancy[WHITE], toSquare + 8);
                }
            break;

            case CASTLING_FLAG:
                // switch (toSquare) {
                //     case g1: MoveRook(hashKey, bitboards[WR], occupancy[WHITE], WR, f1, h1); break;
                //     case c1: MoveRook(hashKey, bitboards[WR], occupancy[WHITE], WR, d1, a1); break;
                //     case g8: MoveRook(hashKey, bitboards[BR], occupancy[BLACK], BR, f8, h8); break;
                //     case c8: MoveRook(hashKey, bitboards[BR], occupancy[BLACK], BR, d8, a8); break;
                // }

                switch (toSquare) {
                    case g1: MovePieceNoHashing(this, WR, WHITE, f1, h1); break;
                    case c1: MovePieceNoHashing(this, WR, WHITE, d1, a1); break;
                    case g8: MovePieceNoHashing(this, BR, BLACK, f8, h8); break;
                    case c8: MovePieceNoHashing(this, BR, BLACK, d8, a8); break;
                }
            break;
        }

        occupancy[BOTH] = occupancy[WHITE] | occupancy[BLACK];
    }

    void Board::TakeNullMove() {
        ply--;

        hashKey = history[ply].hashKey;
        enPassant = history[ply].enPassant;

        side ^= 1;
    }
}