#include "Attacks.h"
#include "Board.h"
#include "Globals.h"
#include "Move.h"
#include "MoveGen.h"

namespace MoveGen {
    void GenerateMoves(Board& board, MoveList& list) {
        //std::cout << "Generating moves\n";

        int side = board.side;
        int enemy = side ^ 1;

        // based on testing, there seems to be no performance difference between this and the conditional code
        int direction = side * -16 + 8;
        int promotionRank = side * -7 + 7;
        int captureStart = enemy * 6;

        list.length = 0;

        // int direction = enemy ? 8 : -8;
        // int promotionRank = enemy ? Rank7 : Rank2;
        // int captureStart = enemy ? BP : WP;

        // castling
        if (enemy) { // equivalent to if side == WHITE
            if (board.castlingPerms & WKC) {
                if (!IsBitSet(board.occupancy[BOTH], f1) &&
                    !IsBitSet(board.occupancy[BOTH], g1) &&
                    !board.IsSquareAttacked(e1, side) &&
                    !board.IsSquareAttacked(f1, side) &&
                    !board.IsSquareAttacked(g1, side)
                ) {
                    AddMove(list, EncodeMove(e1, g1, WK, 0, 0, CASTLING_FLAG));
                }
            }

            if (board.castlingPerms & WQC) {
                if (!IsBitSet(board.occupancy[BOTH], d1) &&
                    !IsBitSet(board.occupancy[BOTH], c1) &&
                    !IsBitSet(board.occupancy[BOTH], b1) &&
                    !board.IsSquareAttacked(e1, side) &&
                    !board.IsSquareAttacked(d1, side) &&
                    !board.IsSquareAttacked(c1, side)
                ) {
                    AddMove(list, EncodeMove(e1, c1, WK, 0, 0, CASTLING_FLAG));
                }
            }
        }
        else {
            if (board.castlingPerms & BKC) {
                if (!IsBitSet(board.occupancy[BOTH], f8) &&
                    !IsBitSet(board.occupancy[BOTH], g8) &&
                    !board.IsSquareAttacked(e8, side) &&
                    !board.IsSquareAttacked(f8, side) &&
                    !board.IsSquareAttacked(g8, side)
                ) {
                    AddMove(list, EncodeMove(e8, g8, BK, 0, 0, CASTLING_FLAG));
                }
            }

            if (board.castlingPerms & BQC) {
                if (!IsBitSet(board.occupancy[BOTH], d8) &&
                    !IsBitSet(board.occupancy[BOTH], c8) &&
                    !IsBitSet(board.occupancy[BOTH], b8) &&
                    !board.IsSquareAttacked(e8, side) &&
                    !board.IsSquareAttacked(d8, side) &&
                    !board.IsSquareAttacked(c8, side)
                ) {
                    AddMove(list, EncodeMove(e8, c8, BK, 0, 0, CASTLING_FLAG));
                }
            }
        }

        // pawns
        int piece = board.side * 6;
        // int piece = enemy ? WP : BP;
        U64 bitboard = board.bitboards[piece];

        while (bitboard) {
            int fromSquare = PopFirstBit(bitboard);
            U64 moves = Attacks::PawnMoves[side][fromSquare] & ~board.occupancy[BOTH];
            U64 captures = Attacks::PawnCaptures[side][fromSquare] & board.occupancy[enemy];

            if (board.enPassant != NO_SQUARE) {
                if (IsBitSet(Attacks::PawnCaptures[side][fromSquare], board.enPassant)) {
                    AddMove(list, EncodeMove(fromSquare, board.enPassant, piece, 0, 0, ENPASSANT_FLAG));
                }
            }

            while (moves) {
                int toSquare = PopFirstBit(moves);

                if (GetRank(toSquare) == promotionRank) {
                    AddMove(list, EncodeMove(fromSquare, toSquare, piece, 0, piece + 4, 0));
                    AddMove(list, EncodeMove(fromSquare, toSquare, piece, 0, piece + 3, 0));
                    AddMove(list, EncodeMove(fromSquare, toSquare, piece, 0, piece + 2, 0));
                    AddMove(list, EncodeMove(fromSquare, toSquare, piece, 0, piece + 1, 0));
                    continue;
                }

                int flag = 0;

                if (toSquare - fromSquare == direction * 2) {
                    if (IsBitSet(board.occupancy[BOTH], fromSquare + direction)) continue;
                    flag = PAWNSTART_FLAG;
                }

                AddMove(list, EncodeMove(fromSquare, toSquare, piece, 0, 0, flag));
            }

            while (captures) {
                int toSquare = PopFirstBit(captures);
                int captured = captureStart;

                for (; captured < captureStart + 6; captured++) {
                    if (IsBitSet(board.bitboards[captured], toSquare)) break;
                }

                if (GetRank(toSquare) == promotionRank) {
                    AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, piece + 4, CAPTURE_FLAG));
                    AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, piece + 3, CAPTURE_FLAG));
                    AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, piece + 2, CAPTURE_FLAG));
                    AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, piece + 1, CAPTURE_FLAG));
                    continue;
                }

                AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, CAPTURE_FLAG));
            }
        }

        // knights
        piece++;
        bitboard = board.bitboards[piece];

        while (bitboard) {
            int fromSquare = PopFirstBit(bitboard);
            U64 moves = Attacks::KnightAttacks[fromSquare] & ~board.occupancy[side];

            while (moves) {
                int toSquare = PopFirstBit(moves);
                int captured = captureStart;
                int flag = 0;

                if (IsBitSet(board.occupancy[enemy], toSquare)) {
                    flag = CAPTURE_FLAG;

                    for (; captured < captureStart + 6; captured++) {
                        if (IsBitSet(board.bitboards[captured], toSquare)) break;
                    }
                }

                AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flag));
            }
        }

        // bishops
        piece++;
        bitboard = board.bitboards[piece];

        while (bitboard) {
            int fromSquare = PopFirstBit(bitboard);
            U64 moves = Attacks::GetBishopAttacks(fromSquare, board.occupancy[BOTH]) & ~board.occupancy[side];

            while (moves) {
                int toSquare = PopFirstBit(moves);
                int captured = captureStart;
                int flag = 0;

                if (IsBitSet(board.occupancy[enemy], toSquare)) {
                    flag = CAPTURE_FLAG;

                    for (; captured < captureStart + 6; captured++) {
                        if (IsBitSet(board.bitboards[captured], toSquare)) break;
                    }
                }

                AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flag));
            }
        }

        // rooks
        piece++;
        bitboard = board.bitboards[piece];

        while (bitboard) {
            int fromSquare = PopFirstBit(bitboard);
            U64 moves = Attacks::GetRookAttacks(fromSquare, board.occupancy[BOTH]) & ~board.occupancy[side];

            while (moves) {
                int toSquare = PopFirstBit(moves);
                int captured = captureStart;
                int flag = 0;

                if (IsBitSet(board.occupancy[enemy], toSquare)) {
                    flag = CAPTURE_FLAG;

                    for (; captured < captureStart + 6; captured++) {
                        if (IsBitSet(board.bitboards[captured], toSquare)) break;
                    }
                }

                AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flag));
            }
        }

        // queens
        piece++;
        bitboard = board.bitboards[piece];

        while (bitboard) {
            int fromSquare = PopFirstBit(bitboard);
            U64 moves = Attacks::GetQueenAttacks(fromSquare, board.occupancy[BOTH]) & ~board.occupancy[side];

            while (moves) {
                int toSquare = PopFirstBit(moves);
                int captured = captureStart;
                int flag = 0;

                if (IsBitSet(board.occupancy[enemy], toSquare)) {
                    flag = CAPTURE_FLAG;

                    for (; captured < captureStart + 6; captured++) {
                        if (IsBitSet(board.bitboards[captured], toSquare)) break;
                    }
                }

                AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flag));
            }
        }

        // king
        piece++;
        bitboard = board.bitboards[piece];

        while (bitboard) {
            int fromSquare = PopFirstBit(bitboard);
            U64 moves = Attacks::KingAttacks[fromSquare] & ~board.occupancy[side];

            while (moves) {
                int toSquare = PopFirstBit(moves);
                if (board.IsSquareAttacked(toSquare, side)) continue;

                int captured = captureStart;
                int flag = 0;

                if (IsBitSet(board.occupancy[enemy], toSquare)) {
                    flag = CAPTURE_FLAG;

                    for (; captured < captureStart + 6; captured++) {
                        if (IsBitSet(board.bitboards[captured], toSquare)) break;
                    }
                }

                AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flag));
            }
        }
    }
}