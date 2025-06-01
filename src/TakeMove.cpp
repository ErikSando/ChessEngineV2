#include "Board.h"
#include "MoveHandling.h"
#include "Moves.h"

void Board::TakeMove() {
    int enemy = side;

    BoardInfo info = history[--ply];
    hashKey = info.hashKey;
    castlingPerms = info.castlingPerms;
    fiftyMoveCount = info.fiftyMoveCount;

    // if (enPassant != NO_SQUARE) HashEnPassant(hashKey, enPassant);
    enPassant = info.enPassant;
    // if (enPassant != NO_SQUARE) HashEnPassant(hashKey, enPassant);

    side ^= 1;
    // HashSide(hashKey);

    int move = info.move;

    int fromSquare = GetFromSquare(move);
    int toSquare = GetToSquare(move);
    int piece = GetMovedPiece(move);
    int promoted = GetPromotedPiece(move);

    if (promoted) {
        ClearBit(bitboards[promoted], toSquare);
        //HashPiece(hashKey, piece, toSquare);
    }
    else {
        ClearBit(bitboards[piece], toSquare);
        //HashPiece(hashKey, piece, toSquare);
    }

    ClearBit(occupancy[side], toSquare);

    SetBit(bitboards[piece], fromSquare);
    SetBit(occupancy[side], fromSquare);
    //HashPiece(hashKey, piece, fromSquare);

    int flag = GetFlag(move);

    switch (flag) {
        case CaptureFlag: {
            int captured = GetCapturedPiece(move);
            SetBit(bitboards[captured], toSquare);
            SetBit(occupancy[enemy], toSquare);
            //HashPiece(hashKey, captured, toSquare);
            break;
        }

        case EnPassantFlag:
            if (side == White) {
                SetBit(bitboards[BP], toSquare - 8);
                SetBit(occupancy[Black], toSquare - 8);
                //HashPiece(hashKey, BP, toSquare - 8);
            }
            else {
                SetBit(bitboards[WP], toSquare + 8);
                SetBit(occupancy[White], toSquare + 8);
                //HashPiece(hashKey, WP, toSquare + 8);
            }
        break;

        case CastlingFlag:
            switch (toSquare) {
                case G1: MoveRook(hashKey, bitboards[WR], occupancy[White], WR, F1, H1); break;
                case C1: MoveRook(hashKey, bitboards[WR], occupancy[White], WR, D1, A1); break;
                case G8: MoveRook(hashKey, bitboards[BR], occupancy[Black], BR, F8, H8); break;
                case C8: MoveRook(hashKey, bitboards[BR], occupancy[Black], BR, D8, A8); break;
            }
        break;
    }

    occupancy[Both] = occupancy[White] | occupancy[Black];
}