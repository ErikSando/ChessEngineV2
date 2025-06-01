#include <iostream>

#include "Globals.h"
#include "Board.h"

void Board::GenerateMoves(MoveList& list) {
    //std::cout << "Generating moves\n";

    // test this out, probably slower with optimisations
    // int enemy = side ^ 1;
    // int direction = side * -16 + 8;
    // int promotionRank = side * -5 + 7;
    // int captureStart = enemy * 6;

    // int piece = side * 6; // pawns

    list.length = 0;

    int enemy = side ^ 1;
    int direction = enemy ? 8 : -8;
    int promotionRank = enemy ? Rank7 : Rank2;
    int captureStart = enemy ? BP : WP;

    // castling
    if (enemy) { // equivalent to if side == White
        if (castlingPerms & Castling::WKC) {
            if (!IsBitSet(occupancy[Both], F1) &&
                !IsBitSet(occupancy[Both], G1) &&
                !IsSquareAttacked(E1, side) &&
                !IsSquareAttacked(F1, side) &&
                !IsSquareAttacked(G1, side)
            ) {
                AddMove(list, EncodeMove(E1, G1, WK, 0, 0, CastlingFlag));
            }

            // U64 squaresMask = GetSquareMask(E1) | GetSquareMask(F1) | GetSquareMask(G1);

            // if (!IsSquaresAttacked(squaresMask, side)) {
            //     AddMove(list, EncodeMove(E1, G1, WK, 0, 0, CastlingFlag));
            // }
        }

        if (castlingPerms & Castling::WQC) {
            if (!IsBitSet(occupancy[Both], D1) &&
                !IsBitSet(occupancy[Both], C1) &&
                !IsSquareAttacked(E1, side) &&
                !IsSquareAttacked(D1, side) &&
                !IsSquareAttacked(C1, side)
            ) {
                AddMove(list, EncodeMove(E1, C1, WK, 0, 0, CastlingFlag));
            }

            // U64 squaresMask = GetSquareMask(E1) | GetSquareMask(D1) | GetSquareMask(C1);

            // if (!IsSquaresAttacked(squaresMask, side)) {
            //     AddMove(list, EncodeMove(E1, C1, WK, 0, 0, CastlingFlag));
            // }
        }
    }
    else {
        if (castlingPerms & Castling::BKC) {
            if (!IsBitSet(occupancy[Both], F8) &&
                !IsBitSet(occupancy[Both], G8) &&
                !IsSquareAttacked(E8, side) &&
                !IsSquareAttacked(F8, side) &&
                !IsSquareAttacked(G8, side)
            ) {
                AddMove(list, EncodeMove(E8, G8, BK, 0, 0, CastlingFlag));
            }

            // U64 squaresMask = GetSquareMask(E8) | GetSquareMask(F8) | GetSquareMask(G8);

            // if (!IsSquaresAttacked(squaresMask, side)) {
            //     AddMove(list, EncodeMove(E8, G8, BK, 0, 0, CastlingFlag));
            // }
        }

        if (castlingPerms & Castling::BQC) {
            if (!IsBitSet(occupancy[Both], D8) &&
                !IsBitSet(occupancy[Both], C8) &&
                !IsSquareAttacked(E8, side) &&
                !IsSquareAttacked(D8, side) &&
                !IsSquareAttacked(C8, side)
            ) {
                AddMove(list, EncodeMove(E8, C8, BK, 0, 0, CastlingFlag));
            }

            // U64 squaresMask = GetSquareMask(E8) | GetSquareMask(D8) | GetSquareMask(C8);

            // if (!IsSquaresAttacked(squaresMask, side)) {
            //     AddMove(list, EncodeMove(E8, G8, BK, 0, 0, CastlingFlag));
            // }
        }
    }

    // pawns
    int piece = enemy ? WP : BP;
    U64 bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = pawnMoves[side][fromSquare] & ~occupancy[Both];
        U64 captures = pawnCaptures[side][fromSquare] & occupancy[enemy];

        while (moves) {
            int toSquare = PopFirstBit(moves);

            if (GetRank(fromSquare) == promotionRank) {
                AddMove(list, EncodeMove(fromSquare, toSquare, piece, 0, piece + 4, 0));
                AddMove(list, EncodeMove(fromSquare, toSquare, piece, 0, piece + 3, 0));
                AddMove(list, EncodeMove(fromSquare, toSquare, piece, 0, piece + 2, 0));
                AddMove(list, EncodeMove(fromSquare, toSquare, piece, 0, piece + 1, 0));
                continue;
            }

            int flag = 0;

            if (toSquare - fromSquare == direction * 2) {
                if (IsBitSet(occupancy[Both], fromSquare + direction)) continue;
                flag = PawnStartFlag;
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, 0, 0, flag));
        }

        while (captures) {
            int toSquare = PopFirstBit(captures);
            int captured = captureStart;

            for (; captured < captureStart + 6; captured++) {
                if (IsBitSet(bitboards[captured], toSquare)) break;
            }

            if (GetRank(fromSquare) == promotionRank) {
                AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, piece + 4, CaptureFlag));
                AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, piece + 3, CaptureFlag));
                AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, piece + 2, CaptureFlag));
                AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, piece + 1, CaptureFlag));
                continue;
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, CaptureFlag));
        }

        if (enPassant != NO_SQUARE) {
            if (IsBitSet(pawnCaptures[side][fromSquare], enPassant)) {
                AddMove(list, EncodeMove(fromSquare, enPassant, piece, 0, 0, EnPassantFlag));
            }
        }
    }

    // knights
    piece++;
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = knightAttacks[fromSquare] & ~occupancy[side];

        while (moves) {
            int toSquare = PopFirstBit(moves);
            int captured = 0;
            int flag = 0;

            if (IsBitSet(occupancy[enemy], toSquare)) {
                flag = CaptureFlag;

                for (; captured < captureStart + 6; captured++) {
                    if (IsBitSet(bitboards[captured], toSquare)) break;
                }
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flag));
        }
    }

    // bishops
    piece++;
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = GetBishopAttacks(fromSquare, occupancy[Both]) & ~occupancy[side];
        // PrintBitboard(moves);

        while (moves) {
            int toSquare = PopFirstBit(moves);
            int captured = 0;
            int flag = 0;

            if (IsBitSet(occupancy[enemy], toSquare)) {
                flag = CaptureFlag;

                for (; captured < captureStart + 6; captured++) {
                    if (IsBitSet(bitboards[captured], toSquare)) break;
                }
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flag));
        }
    }

    // rooks
    piece++;
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = GetRookAttacks(fromSquare, occupancy[Both]) & ~occupancy[side];
        // PrintBitboard(moves);

        while (moves) {
            int toSquare = PopFirstBit(moves);
            int captured = 0;
            int flag = 0;

            if (IsBitSet(occupancy[enemy], toSquare)) {
                flag = CaptureFlag;

                for (; captured < captureStart + 6; captured++) {
                    if (IsBitSet(bitboards[captured], toSquare)) break;
                }
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flag));
        }
    }

    // queens
    piece++;
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = GetQueenAttacks(fromSquare, occupancy[Both]) & ~occupancy[side];
        // PrintBitboard(moves);

        while (moves) {
            int toSquare = PopFirstBit(moves);
            int captured = 0;
            int flag = 0;

            if (IsBitSet(occupancy[enemy], toSquare)) {
                flag = CaptureFlag;

                for (; captured < captureStart + 6; captured++) {
                    if (IsBitSet(bitboards[captured], toSquare)) break;
                }
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flag));
        }
    }

    // king
    piece++;
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = kingAttacks[fromSquare] & ~occupancy[side];

        while (moves) {
            int toSquare = PopFirstBit(moves);
            if (IsSquareAttacked(toSquare, side)) continue;

            int captured = 0;
            int flag = 0;

            if (IsBitSet(occupancy[enemy], toSquare)) {
                flag = CaptureFlag;

                for (; captured < captureStart + 6; captured++) {
                    if (IsBitSet(bitboards[captured], toSquare)) break;
                }
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flag));
        }
    }

    //std::cout << "Generated moves\n";
}