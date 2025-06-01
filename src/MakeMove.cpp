#include <iostream>

#include "Board.h"
#include "Data.h"
#include "MoveHandling.h"
#include "Moves.h"
#include "Globals.h"

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
    // std::cout << "Making move\n";

   // std::cout << "\nMaking move: " << move << "\n\n";

    history[ply].hashKey = hashKey;
    history[ply].move = move;
    history[ply].castlingPerms = castlingPerms;
    history[ply].fiftyMoveCount = fiftyMoveCount++;
    history[ply++].enPassant = enPassant;

    // std::cout << "Updated undo info\n";

    int fromSquare = GetFromSquare(move);
    int toSquare = GetToSquare(move);
    int piece = GetMovedPiece(move);
    int promoted = GetPromotedPiece(move);

    if (IS_PAWN[piece]) fiftyMoveCount = 0;

    // std::cout << "Got move info\n";

    HashCastling(hashKey, castlingPerms);
    castlingPerms &= CastlingPermsUpdate[fromSquare] & CastlingPermsUpdate[toSquare];
    HashCastling(hashKey, castlingPerms);

    int oldEnPassant = enPassant;

    if (enPassant != NO_SQUARE) {
        HashEnPassant(hashKey, enPassant);
        enPassant = NO_SQUARE;
    }

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
        case CaptureFlag: {
            int captured = GetCapturedPiece(move);
            ClearBit(bitboards[captured], toSquare);
            ClearBit(occupancy[side ^ 1], toSquare);
            HashPiece(hashKey, captured, toSquare);
            // std::cout << "Unhashed captured piece\n";
            break;
        }

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
            // std::cout << "EN PASSANT PLAYED !!!!!!!!!!!!!!!1\n";
            // std::cout << ToSquareString(oldEnPassant) << "\n";
            // std::cout << ToSquareString(oldEnPassant - 8) << "\n";
            if (side == White) {
                ClearBit(bitboards[BP], oldEnPassant - 8);
                ClearBit(occupancy[Black], oldEnPassant - 8);
                HashPiece(hashKey, BP, oldEnPassant - 8);
            }
            else {
                ClearBit(bitboards[WP], oldEnPassant + 8);
                ClearBit(occupancy[White], oldEnPassant + 8);
                HashPiece(hashKey, WP, oldEnPassant + 8);
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

                case C1: MoveRook(hashKey, bitboards[WR], occupancy[White], WR, A1, D1); break;
                case G8: MoveRook(hashKey, bitboards[BR], occupancy[Black], BR, H8, F8); break;
                case C8: MoveRook(hashKey, bitboards[BR], occupancy[Black], BR, A8, D8); break;
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

bool Board::ParseMove(const int fromSquare, const int toSquare, const char promoted) {
    MoveList list;
    GenerateMoves(list);

    for (int i = 0; i < list.length; i++) {
        int move = list.moves[i];

        int _fromSquare = GetFromSquare(move);
        int _toSquare = GetToSquare(move);

        if (_fromSquare == fromSquare && _toSquare == toSquare) {
            if (promoted) {
                int prom = GetPromotedPiece(move);

                switch (promoted) {
                    case 'q': if (IS_QUEEN[prom]) return MakeMove(move);
                    case 'r': if (IS_ROOK[prom]) return MakeMove(move);
                    case 'b': if (IS_BISHOP[prom]) return MakeMove(move);
                    case 'n': if (IS_KNIGHT[prom]) return MakeMove(move);
                }

                return false;
            }

            return MakeMove(move);
        }
    }

    return false;
}

bool Board::ParseMove(const char* move) {
    int fromF = *move++ - 'a';
    int fromR = *move++ - '1';
    int toF = *move++ - 'a';
    int toR = *move++ - '1';
    char promoted = *move;

    // MoveList list;
    // GenerateMoves(list);

    // for (int i = 0; i < list.length; i++) {
    //     int move = list.moves[i];

    //     int fS = GetFromSquare(move);
    //     int tS = GetToSquare(move);
    //     int fF = GetFile(fS);
    //     int fR = GetRank(fS);
    //     int tF = GetFile(tS);
    //     int tR = GetRank(tS);

    //     if (fF == fromF && fR == fromR && tF == toF && tR == toR) {
    //         if (promoted) {
    //             int prom = GetPromotedPiece(move);

    //             switch (promoted) {
    //                 case 'q': if (IS_QUEEN[prom]) return MakeMove(move); break;
    //                 case 'r': if (IS_ROOK[prom]) return MakeMove(move); break;
    //                 case 'b': if (IS_BISHOP[prom]) return MakeMove(move); break;
    //                 case 'n': if (IS_KNIGHT[prom]) return MakeMove(move); break;
    //             }

    //             return false;
    //         }

    //         return MakeMove(move);
    //     }
    // }

    int fromSquare = GetSquare(fromF, fromR);
    int toSquare = GetSquare(toF, toR);

    return ParseMove(fromSquare, toSquare, promoted);
}

bool Board::ParseMove(const std::string move) {
    int fromF = move.at(0) - 'a';
    int fromR = move.at(1) - '1';
    int toF = move.at(2) - 'a';
    int toR = move.at(3) - '1';
    char promoted = move.size() >= 5 ? move.at(4) : '\0';

    int fromSquare = GetSquare(fromF, fromR);
    int toSquare = GetSquare(toF, toR);

    return ParseMove(fromSquare, toSquare, promoted);
}