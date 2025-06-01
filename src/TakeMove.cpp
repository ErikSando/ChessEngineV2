#include "Board.h"
#include "MoveHandling.h"
#include "Moves.h"

void Board::TakeMove() {
    // std::cout << "Taking move\n";
    int enemy = side;

    BoardInfo info = history[--ply];
    //hashKey = info.hashKey;

    //std::cout << "\nTaking move: " << info.move << "\n\n";

    if (enPassant != NO_SQUARE) HashEnPassant(hashKey, enPassant);
    enPassant = info.enPassant;
    if (enPassant != NO_SQUARE) HashEnPassant(hashKey, enPassant);

    side ^= 1;
    HashSide(hashKey);

    int move = info.move;

    int fromSquare = GetFromSquare(move);
    int toSquare = GetToSquare(move);
    int piece = GetMovedPiece(move);
    int promoted = GetPromotedPiece(move);

    if (promoted) {
        ClearBit(bitboards[promoted], toSquare);
        HashPiece(hashKey, piece, toSquare);
    }
    else {
        ClearBit(bitboards[piece], toSquare);
        HashPiece(hashKey, piece, toSquare);
    }

    ClearBit(occupancy[side], toSquare);

    SetBit(bitboards[piece], fromSquare);
    SetBit(occupancy[side], fromSquare);
    HashPiece(hashKey, piece, fromSquare);

    int flag = GetFlag(move);

    switch (flag) {
        case CaptureFlag: {
            int captured = GetCapturedPiece(move);
            SetBit(bitboards[captured], fromSquare);
            SetBit(occupancy[enemy], fromSquare);
            HashPiece(hashKey, captured, fromSquare);
        }

        case EnPassantFlag:
            if (side == White) {
                SetBit(bitboards[BP], toSquare - 8);
                SetBit(occupancy[Black], toSquare - 8);
                HashPiece(hashKey, BP, toSquare - 8);
            }
            else {
                SetBit(bitboards[WP], toSquare + 8);
                SetBit(occupancy[White], toSquare + 8);
                HashPiece(hashKey, WP, toSquare + 8);
            }
        break;

        case CastlingFlag:

        break;
    }

    // if (IsEnPassant(move)) {
    //     HashEnPassant(hashKey, enPassant);
    //     enPassant = toSquare;
    //     HashEnPassant(hashKey, enPassant);
        
    //     if (side == White) {
    //         SetBit(bitboards[BP], toSquare - 8);
    //         SetBit(occupancy[Black], toSquare - 8);
    //         HashPiece(hashKey, BP, toSquare - 8);
    //     }
    //     else {
    //         SetBit(bitboards[WP], toSquare + 8);
    //         SetBit(occupancy[White], toSquare + 8);
    //         HashPiece(hashKey, WP, toSquare + 8);
    //     }
    // }

    occupancy[Both] = occupancy[White] | occupancy[Black];

    // std::cout << "Took move\n";
}