#include <cassert>
#include <iostream>
#include <sstream>

#include "Board.h"
#include "Globals.h"
#include "Move.h"
#include "MoveGen.h"

inline char to_lower(char c) {
    // branchless code, very epic
    return c + 32 * (c >= 'A' && c <= 'Z');
}

namespace Utils {
    bool ParseMove(Board& board, const int fromSquare, const int toSquare, const char promoted) {
        MoveList list;
        MoveGen::GenerateMoves(board, list);
    
        for (int i = 0; i < list.length; i++) {
            int move = list.moves[i];
    
            int _fromSquare = GetFromSquare(move);
            int _toSquare = GetToSquare(move);
    
            if (_fromSquare == fromSquare && _toSquare == toSquare) {
                if (!promoted) return board.MakeMove(move);

                int prom = GetPromotedPiece(move);

                switch (promoted) {
                    case 'q': if (IS_QUEEN[prom]) return board.MakeMove(move);
                    case 'r': if (IS_ROOK[prom]) return board.MakeMove(move);
                    case 'b': if (IS_BISHOP[prom]) return board.MakeMove(move);
                    case 'n': if (IS_KNIGHT[prom]) return board.MakeMove(move);
                }
            }
        }
    
        return false;
    }
    
    bool ParseMove(Board& board, const char* move) {
        int fromF = *move++ - 'a';
        int fromR = *move++ - '1';
        int toF = *move++ - 'a';
        int toR = *move++ - '1';
        char promoted = *move;
    
        int fromSquare = GetSquare(fromF, fromR);
        int toSquare = GetSquare(toF, toR);
    
        return ParseMove(board, fromSquare, toSquare, promoted);
    }
    
    bool ParseMove(Board& board, const std::string move) {
        int fromF = move.at(0) - 'a';
        int fromR = move.at(1) - '1';
        int toF = move.at(2) - 'a';
        int toR = move.at(3) - '1';
        char promoted = move.size() >= 5 ? move.at(4) : '\0';
    
        int fromSquare = GetSquare(fromF, fromR);
        int toSquare = GetSquare(toF, toR);
    
        return ParseMove(board, fromSquare, toSquare, promoted);
    }

    int ToSquare(char fileC, char rankC) {
        assert(fileC >= 'a' && fileC <= 'h');
        assert(rankC >= '1' && rankC <= '8');

        return GetSquare(fileC - 'a', rankC - '1');
    }

    int ToSquare(const char* square) {
        return ToSquare(square[0], square[1]);
    }

    int ToSquare(std::string square) {
        return ToSquare(square[0], square[1]);
    }

    std::string ToSquareString(int square) {
        int file = GetFile(square);
        int rank = GetRank(square);

        std::ostringstream squareString;

        char fileC = 'a' + file;
        char rankC = '1' + rank;

        squareString << fileC << rankC;

        return squareString.str();
    }

    std::string ToMoveString(int move) {
        std::ostringstream moveString;

        int fromSquare = GetFromSquare(move);
        int toSquare = GetToSquare(move);
        int promoted = GetPromotedPiece(move);

        char fromF = 'a' + GetFile(fromSquare);
        char fromR = '1' + GetRank(fromSquare);
        char toF = 'a' + GetFile(toSquare);
        char toR = '1' + GetRank(toSquare);

        char promotedChar = '\0';
        if (promoted) promotedChar = to_lower(PIECE_CHAR[promoted]);

        moveString << fromF << fromR << toF << toR << promotedChar;

        return moveString.str();
    }

    void PrintBitboard(U64 bitboard) {
        for (int row = 7; row >= 0; row--) {
            for (int col = 0; col < 8; col++) {
                int bit = row * 8 + col;
                std::cout << " " << GetBit(bitboard, bit);
            }

            std::cout << "\n";
        }
    }
}