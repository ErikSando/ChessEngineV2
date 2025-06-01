#include <iostream>

#include "Globals.h"
#include "Board.h"

void Board::GenerateMoves(MoveList& list) {
    // test this out, probably slower with optimisations
    // int enemy = side ^ 1;
    // int direction = side * -16 + 8;
    // int promotionRank = side * -5 + 7;
    // int captureStart = enemy * 6;

    // int piece = side * 6; // pawns

    int enemy = side ^ 1;
    int direction = enemy ? 8 : -8;
    int promotionRank = enemy ? Rank::Seven : Rank::Two;
    int captureStart = enemy ? Piece::BP : Piece::WP;

    // castling
    if (enemy) { // equivalent to if side == White
        if (castlingPerms & Castling::WKC) {
            if (!IsSquareAttacked(E1, side) &&
                !IsSquareAttacked(F1, side) &&
                !IsSquareAttacked(G1, side)
            ) {
                AddMove(list, EncodeMove(E1, G1, Piece::WK, 0, 0, CastlingFlag));
            }

            // U64 squaresMask = GetSquareMask(E1) | GetSquareMask(F1) | GetSquareMask(G1);

            // if (!IsSquaresAttacked(squaresMask, side)) {
            //     AddMove(list, EncodeMove(E1, G1, Piece::WK, 0, 0, CastlingFlag));
            // }
        }

        if (castlingPerms & Castling::WQC) {
            if (!IsSquareAttacked(E1, side) &&
                !IsSquareAttacked(D1, side) &&
                !IsSquareAttacked(C1, side)
            ) {
                AddMove(list, EncodeMove(E1, C1, Piece::WK, 0, 0, CastlingFlag));
            }

            // U64 squaresMask = GetSquareMask(E1) | GetSquareMask(D1) | GetSquareMask(C1);

            // if (!IsSquaresAttacked(squaresMask, side)) {
            //     AddMove(list, EncodeMove(E1, C1, Piece::WK, 0, 0, CastlingFlag));
            // }
        }
    }
    else {
        if (castlingPerms & Castling::BKC) {
            if (!IsSquareAttacked(E8, side) &&
                !IsSquareAttacked(F8, side) &&
                !IsSquareAttacked(G8, side)
            ) {
                AddMove(list, EncodeMove(E8, G8, Piece::BK, 0, 0, CastlingFlag));
            }

            // U64 squaresMask = GetSquareMask(E8) | GetSquareMask(F8) | GetSquareMask(G8);

            // if (!IsSquaresAttacked(squaresMask, side)) {
            //     AddMove(list, EncodeMove(E8, G8, Piece::BK, 0, 0, CastlingFlag));
            // }
        }

        if (castlingPerms & Castling::BQC) {
            if (!IsSquareAttacked(E8, side) &&
                !IsSquareAttacked(D8, side) &&
                !IsSquareAttacked(C8, side)
            ) {
                AddMove(list, EncodeMove(E8, C8, Piece::BK, 0, 0, CastlingFlag));
            }

            // U64 squaresMask = GetSquareMask(E8) | GetSquareMask(D8) | GetSquareMask(C8);

            // if (!IsSquaresAttacked(squaresMask, side)) {
            //     AddMove(list, EncodeMove(E8, G8, Piece::BK, 0, 0, CastlingFlag));
            // }
        }
    }

    int piece = enemy ? Piece::WP : Piece::BP; // pawns
    U64 bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = pawnMoves[side][fromSquare] & ~occupancy[Both];
        U64 captures = pawnCaptures[side][fromSquare] & ~occupancy[side] & occupancy[enemy];

        while (moves) {
            int toSquare = PopFirstBit(moves);
            int flags = 0;

            if (GetRank(fromSquare) == promotionRank) {
                flags |= PromotionFlag;
                AddMove(list, EncodeMove(fromSquare, toSquare, piece, 0, piece + 4, flags));
                AddMove(list, EncodeMove(fromSquare, toSquare, piece, 0, piece + 3, flags));
                AddMove(list, EncodeMove(fromSquare, toSquare, piece, 0, piece + 2, flags));
                AddMove(list, EncodeMove(fromSquare, toSquare, piece, 0, piece + 1, flags));
                continue;
            }

            if (toSquare - fromSquare == direction * 2) {
                flags |= PawnStartFlag;
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, 0, 0, flags));
        }

        while (captures) {
            int toSquare = PopFirstBit(captures);
            int flags = 0;
            int captured = captureStart;

            for (; captured < captureStart + 6; captured++) {
                if (IsBitSet(bitboards[captured], toSquare)) break;
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flags));
        }

        if (enPassant != NO_SQUARE) {
            if (IsBitSet(pawnCaptures[side][fromSquare], enPassant)) {
                AddMove(list, EncodeMove(fromSquare, enPassant, piece, 0, 0, EnPassantFlag));
            }
        }
    }

    piece++; // knights
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = knightAttacks[fromSquare] & ~occupancy[White];

        while (moves) {
            int toSquare = PopFirstBit(moves);
            int flags = 0;
            int captured = 0;

            if (IsBitSet(occupancy[enemy], toSquare)) {
                for (; captured < captureStart + 6; captured++) {
                    if (IsBitSet(bitboards[captured], toSquare)) break;
                }
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flags));
        }
    }

    piece++; // bishops
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = GetBishopAttacks(fromSquare, occupancy[Both]) & ~occupancy[side];
        // PrintBitboard(moves);

        while (moves) {
            int toSquare = PopFirstBit(moves);
            int flags = 0;
            int captured = 0;

            if (IsBitSet(occupancy[enemy], toSquare)) {
                for (; captured < captureStart + 6; captured++) {
                    if (IsBitSet(bitboards[captured], toSquare)) break;
                }
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flags));
        }
    }

    piece++; // rooks
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = GetRookAttacks(fromSquare, occupancy[Both]) & ~occupancy[side];
        // PrintBitboard(moves);

        while (moves) {
            int toSquare = PopFirstBit(moves);
            int flags = 0;
            int captured = 0;

            if (IsBitSet(occupancy[enemy], toSquare)) {
                for (; captured < captureStart + 6; captured++) {
                    if (IsBitSet(bitboards[captured], toSquare)) break;
                }
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flags));
        }
    }

    piece++; // queens
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = GetQueenAttacks(fromSquare, occupancy[Both]) & ~occupancy[side];
        // PrintBitboard(moves);

        while (moves) {
            int toSquare = PopFirstBit(moves);
            int flags = 0;
            int captured = 0;

            if (IsBitSet(occupancy[enemy], toSquare)) {
                for (; captured < captureStart + 6; captured++) {
                    if (IsBitSet(bitboards[captured], toSquare)) break;
                }
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flags));
        }
    }

    piece++; // king
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = kingAttacks[fromSquare] & ~occupancy[side];

        while (moves) {
            int toSquare = PopFirstBit(moves);
            if (IsSquareAttacked(toSquare, side)) continue;

            int flags = 0;
            int captured = 0;

            if (IsBitSet(occupancy[enemy], toSquare)) {
                for (; captured < captureStart + 6; captured++) {
                    if (IsBitSet(bitboards[captured], toSquare)) break;
                }
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, flags));
        }
    }
}

void Board::GenerateCaptures(MoveList& list) {
    
}