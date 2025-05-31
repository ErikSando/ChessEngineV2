#include <iostream>

#include "Board.h"

void Board::GenerateMoves(MoveList& list) {
    bool white = side == Side::White;
    int enemy = side ^ 1;
    int direction = white ? 8 : -8;
    int promotionRank = white ? Rank::Seven : Rank::Two;

    int piece = white ? Piece::WP : Piece::BP; // pawns
    U64 bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        // std::cout << "Pawn on square " << ToSquareString(fromSquare) << "\n";

        U64 moves = pawnMoves[side][fromSquare] & ~occupancy[Both];
        U64 captures = pawnCaptures[side][fromSquare] & ~occupancy[side] & occupancy[enemy];

        while (moves) {
            int toSquare = PopFirstBit(moves);
            // std::cout << "Can move to " << ToSquareString(toSquare) << "\n";
        }

        while (captures) {
            int toSquare = PopFirstBit(captures);
            // std::cout << "Can capture on " << ToSquareString(toSquare) << "\n";
        }
    }

    piece++; // knights
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        // std::cout << "Knight on square " << ToSquareString(fromSquare) << "\n";

        U64 moves = knightAttacks[fromSquare] & ~occupancy[White];

        while (moves) {
            int toSquare = PopFirstBit(moves);
            // std::cout << "Can move to " << ToSquareString(toSquare) << "\n";
        }
    }

    piece++; // bishops
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        std::cout << "Bishop on square " << ToSquareString(fromSquare) << "\n";

        U64 moves = GetBishopAttacks(fromSquare, occupancy[Both]) & ~occupancy[side];
        // PrintBitboard(moves);

        while (moves) {
            int toSquare = PopFirstBit(moves);
            std::cout << "Can move to " << ToSquareString(toSquare) << "\n";
        }
    }

    piece++; // rooks
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        std::cout << "Rook on square " << ToSquareString(fromSquare) << "\n";

        U64 moves = GetRookAttacks(fromSquare, occupancy[Both]) & ~occupancy[side];
        // PrintBitboard(moves);

        while (moves) {
            int toSquare = PopFirstBit(moves);
            std::cout << "Can move to " << ToSquareString(toSquare) << "\n";
        }
    }

    piece++; // queens
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        std::cout << "Queen on square " << ToSquareString(fromSquare) << "\n";

        U64 moves = GetQueenAttacks(fromSquare, occupancy[Both]) & ~occupancy[side];
        // PrintBitboard(moves);

        while (moves) {
            int toSquare = PopFirstBit(moves);
            std::cout << "Can move to " << ToSquareString(toSquare) << "\n";
        }
    }

    piece++; // king
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);
        // std::cout << "King on square " << ToSquareString(fromSquare) << "\n";

        U64 moves = kingAttacks[fromSquare] & ~occupancy[White];

        while (moves) {
            int toSquare = PopFirstBit(moves);
            // std::cout << "Can move to " << ToSquareString(toSquare) << "\n";
        }
    }
}

void Board::GenerateCaptures(MoveList& list) {
    
}