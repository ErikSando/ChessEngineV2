#pragma once

#include "Board.h"
#include "Globals.h"
#include "MoveScoring.h"

namespace ErikEngine {
    constexpr int MAX_MOVES = 256;

    struct Move {
        int move;
        int score;
    };

    struct MoveList {
        int length = 0;

        inline Move& operator[](size_t index) { return m_moves[index]; }

        // inline Move& at(size_t index) { return m_moves[index]; }

        // these could be references, but i think readability (and performance) is better if
        // these are read only and the move list is edited with [index].move/score
        inline int move_at(size_t index) { return m_moves[index].move; }
        inline int score_at(size_t index) { return m_moves[index].score; }

        inline void add(int score, int move) {
            m_moves[length].move = move;
            m_moves[length].score = score;
            length++;
        }

        private:

        Move m_moves[MAX_MOVES];
    };

    namespace MoveGen {
        constexpr U64 WKC_EMPTY_MASK = (1ULL << f1) | (1ULL << g1);
        constexpr U64 WKC_SAFE_MASK = (1ULL << e1) | WKC_EMPTY_MASK;
        constexpr U64 WQC_EMPTY_MASK = (1ULL << c1) | (1ULL << d1);
        constexpr U64 WQC_SAFE_MASK = (1ULL << e1) | WQC_EMPTY_MASK;
        constexpr U64 BKC_EMPTY_MASK = (1ULL << f8) | (1ULL << g8);
        constexpr U64 BKC_SAFE_MASK = (1ULL << e8) | BKC_EMPTY_MASK;
        constexpr U64 BQC_EMPTY_MASK = (1ULL << c8) | (1ULL << d8);
        constexpr U64 BQC_SAFE_MASK = (1ULL << e8) | BQC_EMPTY_MASK;

        extern U64 RANK_1_MASK; // defined in MoveGen.cpp

        void GenerateMovesPL(const Board& board, MoveList& list);
        void GenerateCapturesPL(const Board& board, MoveList& list);
        void GenerateMoves(Board& board, MoveList& list);
        void GenerateCaptures(Board& board, MoveList& list);

        // MOVE REPRESENTATION
        // 0              0            0              0           0000      0000      0000   000000  000000  (28 bits)
        // is en passant  is castling  is pawn start  is capture  promoted  captured  piece  to      from
        inline int EncodeMove(int fromSquare, int toSquare, int moved, int captured, int promoted, int flag) {
            return (fromSquare) | (toSquare << 6) | (moved << 12) | (captured << 16) | (promoted << 20) | flag;
        }

        inline int GetScore(const Board& board, int move, int piece, int toSquare) {
            int score = 0;

            if (move == MoveScoring::KillerMoves[0][board.ply]) {
                score = MoveScoring::KillerScore0;
            }
            else if (move == MoveScoring::KillerMoves[1][board.ply]) {
                score = MoveScoring::KillerScore1;
            }
            else {
                // int lastMove = board.history[board.ply - 1].move;
                // if (move == CounterMoves[GetFromSquare(lastMove)][GetToSquare(lastMove)]) score = CounterMoveScore;
                // else score = HistoryMoves[piece][toSquare];
                score = MoveScoring::HistoryMoves[piece][toSquare];
            }

            return score;
        }
    }
}