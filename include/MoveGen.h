#pragma once

#include "Board.h"
#include "MoveScoring.h"

constexpr int MAX_MOVES = 256;

struct Move {
    int move;
    int score;
};

struct MoveList {
    Move moves[MAX_MOVES];
    int length = 0;

    inline Move& operator[](size_t index) {
        return moves[index];
    }

    // inline Move& at(size_t index) {
    //     return moves[index];
    // }

    // these could be references, but i think readability is better if
    // these are read only and the move list is edited [index].move/score
    inline int move_at(size_t index) {
        return moves[index].move;
    }

    inline int score_at(size_t index) {
        return moves[index].score;
    }
};

namespace MoveGen {
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

    inline void AddMove(MoveList& list, int score, int move) {
        list.moves[list.length].move = move;
        list.moves[list.length].score = score;
        list.length++;
    }

    inline int GetScore(const Board& board, int move, int piece, int toSquare) {
        int score = 0;

        if (move == MoveScoring::KillerMoves[0][board.ply]) {
            score = MoveScoring::Heuristics::KillerScore0;
        }
        else if (move == MoveScoring::KillerMoves[1][board.ply]) {
            score = MoveScoring::Heuristics::KillerScore1;
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