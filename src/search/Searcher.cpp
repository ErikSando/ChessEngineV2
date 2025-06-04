#include <iostream>

#include "Board.h"
#include "Evaluation.h"
#include "MoveGen.h"
#include "Searcher.h"
#include "Utils.h"

inline void CheckTimeUp(SearchInfo& info) {
    info.stopped = info.timeSet && (Utils::GetTimeMS() > info.stopTime);
    // check if stop command was written here
}

int Quiescence(Board& board, SearchInfo& info, int alpha, int beta) {
    info.nodes++;

    MoveList list;
    MoveGen::GenerateCaptures(board, list);

    if (list.length <= 0) {
        return Evaluation::Evaluate(board);
    }

    for (int i = 0; i < list.length; i++) {
        int score = -Quiescence(board, info, -beta, -alpha);

        if (score > alpha) {
            if (score >= beta) {
                return beta;
            }

            alpha = score;
        }
    }

    return alpha;
}

int AlphaBeta(Board& board, SearchInfo& info, int alpha, int beta, int depth) {
    if (info.nodes & 4095 == 0) CheckTimeUp(info);
    if (info.stopped) return 0;
    if (depth == 0) return Quiescence(board, info, -INFINITY, INFINITY);

    info.nodes++;

    MoveList list;
    MoveGen::GenerateMoves(board, list);

    if (list.length <= 0) {
        int KP = board.side == WHITE ? WK : BK;
        if (board.IsSquareAttacked(FirstBitIndex(board.bitboards[KP]))) return -INFINITY + depth;
        else return 0;
    }

    for (int i = 0; i < list.length; i++) {
        int score = -AlphaBeta(board, info, -beta, -alpha, depth - 1);
    
        if (score > alpha) {
            if (score >= beta) {
                return beta;
            }

            alpha = score;
        }
    }

    return alpha;
}

void Searcher::Search(Board& board, SearchInfo& info) {
    int alpha = -INFINITY;
    int beta = INFINITY;

    for (int depth = 1; depth <= info.depth; depth++) {
        int value = AlphaBeta(board, info, alpha, beta, depth);

        if (info.stopped) break;

        int now = Utils::GetTimeMS();
        int time = now - info.startTime;

        std::cout << "info depth " << depth << " cp " << value << " nodes " << info.nodes << " time " << time << "\n";
    }

    std::cout << "bestmove " << "\n";
}