#include <iostream>

#include "Board.h"
#include "Moves.h"

inline void HashPiece(U64& key, int piece, int square) {
    key ^= PieceKeys[piece][square];
}

inline void HashCastling(U64& key, int castlingPerms) {
    key ^= CastlingKeys[castlingPerms];
}

inline void HashEnPassant(U64& key, int enPassant) {
    key ^= EnPassantKeys[enPassant];
}

inline void HashSide(U64& key) {
    key ^= SideKey;
}

inline void MoveRook(U64& hashKey, U64& bitboard, U64& occupancy, int rook, int fromSquare, int toSquare) {
    HashPiece(hashKey, rook, fromSquare);
    ClearBit(bitboard, fromSquare);
    ClearBit(occupancy, fromSquare);
    SetBit(bitboard, toSquare);
    SetBit(occupancy, toSquare);
    HashPiece(hashKey, rook, toSquare);
}

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

bool Board::MakeMove(int move) {
    // std::cout << "Making move\n";

   // std::cout << "\nMaking move: " << move << "\n\n";

    history[ply].hashKey = hashKey;
    history[ply].move = move;
    history[ply++].enPassant = enPassant;

    // std::cout << "Updated undo info\n";

    int fromSquare = GetFromSquare(move);
    int toSquare = GetToSquare(move);
    int piece = GetMovedPiece(move);
    int promoted = GetPromotedPiece(move);

    // std::cout << "Got move info\n";

    if (enPassant != NO_SQUARE) {
        HashEnPassant(hashKey, enPassant);
        enPassant = NO_SQUARE;
    }

    HashCastling(hashKey, castlingPerms);
    castlingPerms &= CastlingPermsUpdate[fromSquare] & CastlingPermsUpdate[toSquare];
    HashCastling(hashKey, castlingPerms);

    // std::cout << "Updated castling perms\n";

    ClearBit(bitboards[piece], fromSquare);
    ClearBit(occupancy[side], fromSquare);
    HashPiece(hashKey, piece, fromSquare);

    // std::cout << "Removed moving piece\n";

    if (promoted) {
        SetBit(bitboards[promoted], toSquare);
        HashPiece(hashKey, promoted, toSquare);
    }
    else {
        SetBit(bitboards[piece], toSquare);
        HashPiece(hashKey, piece, toSquare);
    }

    // std::cout << "Replaced moving piece\n";

    SetBit(occupancy[side], toSquare);

    int flag = GetFlag(move);
    // std::cout << "Got flag\n";

    switch (flag) {
        case CaptureFlag:
            HashPiece(hashKey, GetCapturedPiece(move), toSquare);
            // std::cout << "Unhashed captured piece\n";
        break;

        case PawnStartFlag:
            if (side == White) {
                enPassant = fromSquare + 8;
                HashEnPassant(hashKey, enPassant);
            }
            else {
                enPassant = toSquare + 8;
                HashEnPassant(hashKey, enPassant);
            }
            // std::cout << "Set en passant\n";
        break;

        case EnPassantFlag:
            if (side == White) {
                ClearBit(bitboards[BP], enPassant);
                ClearBit(bitboards[Black], enPassant);
                HashPiece(hashKey, BP, enPassant);
            }
            else {
                ClearBit(bitboards[WP], enPassant);
                ClearBit(bitboards[White], enPassant);
                HashPiece(hashKey, WP, enPassant);
            }
            // std::cout << "Removed pawn from en passant\n";
        break;

        case CastlingFlag:
            switch (toSquare) {
                case G1:
                    MoveRook(hashKey, bitboards[WR], occupancy[White], WR, H1, F1);
                    // HashPiece(hashKey, WR, H1);
                    // ClearBit(bitboards[WR], H1);
                    // ClearBit(occupancy[White], H1);
                    // SetBit(bitboards[WR], F1);
                    // SetBit(occupancy[White], F1);
                    // HashPiece(hashKey, WR, F1);
                break;

                case C1:
                    MoveRook(hashKey, bitboards[WR], occupancy[White], WR, B1, D1);
                break;

                case G8:
                    MoveRook(hashKey, bitboards[BR], occupancy[Black], BR, H8, F8);
                break;

                case C8:
                    MoveRook(hashKey, bitboards[BR], occupancy[Black], BR, B8, D8);
                break;
            }
            // std::cout << "Moved rook in castling\n";
        break;
    }

    occupancy[Both] = occupancy[White] | occupancy[Black];

    int kingSquare = FirstBitIndex(bitboards[side == White ? WK : BK]);

    side ^= 1;
    HashSide(hashKey);

    if (IsSquareAttacked(kingSquare, side ^ 1)) {
        // std::cout << "Illegal, taking\n";
        TakeMove();
        return false;
    }

    // std::cout << "Made move\n";

    return true;
}

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