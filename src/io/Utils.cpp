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
    int ToSquare(char fileC, char rankC) {
        assert(fileC >= 'a' && fileC <= 'h');
        assert(rankC >= '1' && rankC <= '8');

        return GetSquare(fileC - 'a', rankC - '1');
    }

    int ToSquare(const char* square) {
        return ToSquare(square[0], square[1]);
    }

    int ToSquare(std::string& square) {
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

    int ParseMove(const Board& board, const int fromSquare, const int toSquare, const char promoted) {
        MoveList list;
        MoveGen::GenerateMovesPL(board, list);
    
        for (int i = 0; i < list.length; i++) {
            int move = list.moves[i].move;
    
            int _fromSquare = GetFromSquare(move);
            int _toSquare = GetToSquare(move);
    
            if (_fromSquare == fromSquare && _toSquare == toSquare) {
                if (!promoted) return move;

                int prom = GetPromotedPiece(move);

                switch (promoted) {
                    case 'q': if (IS_QUEEN[prom]) return move;
                    case 'r': if (IS_ROOK[prom]) return move;
                    case 'b': if (IS_BISHOP[prom]) return move;
                    case 'n': if (IS_KNIGHT[prom]) return move;
                }
            }
        }
    
        return 0;
    }
    
    int ParseMove(const Board& board, const char* move) {
        int fromF = *move++ - 'a';
        int fromR = *move++ - '1';
        int toF = *move++ - 'a';
        int toR = *move++ - '1';
        char promoted = *move;
    
        int fromSquare = GetSquare(fromF, fromR);
        int toSquare = GetSquare(toF, toR);
    
        return ParseMove(board, fromSquare, toSquare, promoted);
    }
    
    int ParseMove(const Board& board, const std::string& move) {
        int fromF = move.at(0) - 'a';
        int fromR = move.at(1) - '1';
        int toF = move.at(2) - 'a';
        int toR = move.at(3) - '1';
        char promoted = move.size() >= 5 ? move.at(4) : '\0';
    
        int fromSquare = GetSquare(fromF, fromR);
        int toSquare = GetSquare(toF, toR);
    
        return ParseMove(board, fromSquare, toSquare, promoted);
    }

    bool MoveExists(const Board& board, int move) {
        MoveList list;
        // choosing to use pseudo legal to differentiate between invalid and illegal moves
        // invalid meaning the piece can not move in the way specified
        // illegal meaning the piece movement is valid, but the move leaves the king in check
        // i made up that definition of invalid, not sure if that is correct chess terminology
        MoveGen::GenerateMovesPL(board, list);

        for (int i = 0; i < list.length; i++) {
            if (move == list.moves[i].move) return true;
        }

        return false;
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