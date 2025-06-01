#include <iostream>

#include "Globals.h"
#include "Board.h"

void Board::GenerateCaptures(MoveList& list) {
    int enemy = side ^ 1;
    int direction = enemy ? 8 : -8;
    int promotionRank = enemy ? Rank::Seven : Rank::Two;
    int captureStart = enemy ? Piece::BP : Piece::WP;

    // pawns
    int piece = enemy ? Piece::WP : Piece::BP;
    U64 bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 captures = pawnCaptures[side][fromSquare] & occupancy[enemy];

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
        U64 moves = knightAttacks[fromSquare] & occupancy[enemy];
        // PrintBitboard(moves);

        while (moves) {
            int toSquare = PopFirstBit(moves);
            int captured = 0;

            for (; captured < captureStart + 6; captured++) {
                if (IsBitSet(bitboards[captured], toSquare)) break;
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, CaptureFlag));
        }
    }

    // bishops
    piece++;
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = GetBishopAttacks(fromSquare, occupancy[Both]) & occupancy[enemy];
        // PrintBitboard(moves);

        while (moves) {
            int toSquare = PopFirstBit(moves);
            int captured = 0;

            for (; captured < captureStart + 6; captured++) {
                if (IsBitSet(bitboards[captured], toSquare)) break;
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, CaptureFlag));
        }
    }
    // rooks
    piece++;
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = GetRookAttacks(fromSquare, occupancy[Both]) & occupancy[enemy];
        // PrintBitboard(moves);

        while (moves) {
            int toSquare = PopFirstBit(moves);
            int captured = 0;

            for (; captured < captureStart + 6; captured++) {
                if (IsBitSet(bitboards[captured], toSquare)) break;
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, CaptureFlag));
        }
    }

    // queens
    piece++;
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = GetQueenAttacks(fromSquare, occupancy[Both]) & occupancy[enemy];
        // PrintBitboard(moves);

        while (moves) {
            int toSquare = PopFirstBit(moves);
            int captured = 0;

            for (; captured < captureStart + 6; captured++) {
                if (IsBitSet(bitboards[captured], toSquare)) break;
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, CaptureFlag));
        }
    }

    // king
    piece++;
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        U64 moves = kingAttacks[fromSquare] & occupancy[enemy];

        while (moves) {
            int toSquare = PopFirstBit(moves);
            if (IsSquareAttacked(toSquare, side)) continue;

            int captured = 0;

            for (; captured < captureStart + 6; captured++) {
                if (IsBitSet(bitboards[captured], toSquare)) break;
            }

            AddMove(list, EncodeMove(fromSquare, toSquare, piece, captured, 0, CaptureFlag));
        }
    }
}