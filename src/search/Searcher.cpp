#include <iostream>

#include "Board.h"
#include "Evaluation.h"
#include "MoveGen.h"
#include "Searcher.h"
#include "Utils.h"

inline int Quiescence(Board& board, int alpha, int beta) {
    MoveList list;
    MoveGen::GenerateCaptures(board, list);

    if (list.length <= 0) {
        return Evaluation::Evaluate(board);
    }

    for (int i = 0; i < list.length; i++) {
        int score = -Quiescence(board, -beta, -alpha);

        if (score > alpha) {
            if (score >= beta) {
                return beta;
            }

            alpha = score;
        }
    }

    return alpha;
}

inline int AlphaBeta(Board& board, int alpha, int beta, int depth) {
    if (depth == 0) return Quiescence(board, -INFINITY, INFINITY);

    MoveList list;
    MoveGen::GenerateMoves(board, list);

    if (list.length <= 0) {
        int KP = board.side == WHITE ? WK : BK;
        if (board.IsSquareAttacked(FirstBitIndex(board.bitboards[KP]))) return -INFINITY + depth;
        else return 0;
    }

    for (int i = 0; i < list.length; i++) {
        int score = -AlphaBeta(board, -beta, -alpha, depth - 1);
    
        if (score > alpha) {
            if (score >= beta) {
                return beta;
            }

            alpha = score;
        }
    }

    return alpha;
}

void Searcher::Search(Board& board, const SearchInfo& info) {
    int alpha = -INFINITY;
    int beta = INFINITY;

    for (int depth = 1; depth <= info.depth; depth++) {
        int value = AlphaBeta(board, alpha, beta, depth);

        std::cout << "info depth " << depth << " cp " << value << "\n";
    }

    std::cout << "bestmove " << "\n";
}