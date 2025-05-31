#include <iostream>

#include "Board.h"

void Board::GenerateMoves(MoveList& list) {
    bool white = side == Side::White;
    int direction = white ? 8 : -8;


    int piece = white ? Piece::WP : Piece::BP; // pawns
    U64 bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);

        std::cout << "Pawn on square " << ToSquareString(fromSquare) << "\n";
    }

    piece++; // knights
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);

        std::cout << "Knight on square " << ToSquareString(fromSquare) << "\n";
    }

    piece++; // bishops
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);

        std::cout << "Bishop on square " << ToSquareString(fromSquare) << "\n";
    }

    piece++; // rooks
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);

        std::cout << "Rook on square " << ToSquareString(fromSquare) << "\n";
    }

    piece++; // queens
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);

        std::cout << "Queen on square " << ToSquareString(fromSquare) << "\n";
    }

    piece++; // king
    bitboard = bitboards[piece];

    while (bitboard) {
        int fromSquare = PopFirstBit(bitboard);

        std::cout << "King on square " << ToSquareString(fromSquare) << "\n";
    }
}

void Board::GenerateCaptures(MoveList& list) {
    
}