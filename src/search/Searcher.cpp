#include <iostream>

#include "Board.h"
#include "Evaluation.h"
#include "MoveGen.h"
#include "Searcher.h"
#include "Utils.h"

Searcher::Searcher(TTable& t_table) : ttable(t_table) {

}

inline void CheckTimeUp(SearchInfo& info) {
    info.stopped = info.timeSet && (Utils::GetTimeMS() > info.stopTime);
    // check if stop command was written here
}

inline bool ThreeFoldRepetition(Board& board) {
    int repetitions = 0;

    for (int i = board.ply - board.fiftyMoveCount; i < board.ply - 1; i++) {
        if (board.hashKey == board.history[i].hashKey) {
            repetitions++;
            if (repetitions >= 2) break;
        }
    }

    return repetitions >= 2;
}

inline void OrderMoves(MoveList& list) {
    for (int startidx = 0; startidx < list.length - 1; startidx++) {
        int bestScore = list.moves[startidx].score;
        int bestIndex = startidx;
        
        for (int i = startidx + 1; i < list.length; i++) {
            if (list.moves[i].score > bestScore) {
                bestScore = list.moves[i].score;
                bestIndex = i;
            }
        }

        Move temp = list.moves[bestIndex];
        list.moves[bestIndex] = list.moves[startidx];
        list.moves[startidx] = temp;
    }
}

int Searcher::Quiescence(Board& board, SearchInfo& info, int alpha, int beta) {
    if ((info.nodes & 4095) == 0) CheckTimeUp(info);
    if (info.stopped) return 0;

    info.nodes++;

    if (board.fiftyMoveCount >= 100 || ThreeFoldRepetition(board)) return 0;

    int eval = Evaluation::Evaluate(board);

    if (eval >= beta) return eval;
    if (eval > alpha) alpha = eval;

    MoveList list;
    MoveGen::GenerateCaptures(board, list);
    OrderMoves(list);

    for (int i = 0; i < list.length; i++) {
        if (!board.MakeMove(list.moves[i].move)) continue;
        int score = -Quiescence(board, info, -beta, -alpha);
        board.TakeMove();

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}

int Searcher::AlphaBeta(Board& board, SearchInfo& info, int depth, int alpha, int beta) {
    if ((info.nodes & 4095) == 0) CheckTimeUp(info);
    if (info.stopped) return 0;
    if (depth <= 0) return Quiescence(board, info, alpha, beta);

    info.nodes++;

    if (board.fiftyMoveCount >= 100 || ThreeFoldRepetition(board)) return 0;

    MoveList list;
    MoveGen::GenerateMoves(board, list);
    OrderMoves(list);

    int legalMoves = 0;

    for (int i = 0; i < list.length; i++) {
        if (!board.MakeMove(list.moves[i].move)) continue;
        int score = -AlphaBeta(board, info, depth - 1, -beta, -alpha);
        legalMoves++;
        board.TakeMove();

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    if (!legalMoves) {
        int KP = board.side == WHITE ? WK : BK;

        if (board.IsSquareAttacked(FirstBitIndex(board.bitboards[KP]))) return -INFINITY + board.ply;
        else return 0;
    }

    return alpha;
}

void Searcher::Search(Board& board, SearchInfo& info) {
    int alpha = -INFINITY;
    int beta = INFINITY;

    for (int depth = 1; depth <= info.depth; depth++) {
        int value = AlphaBeta(board, info, depth, alpha, beta);

        if (info.stopped) break;

        int now = Utils::GetTimeMS();
        int time = now - info.startTime;

        std::cout << "info depth " << depth << " cp " << value;
        std::cout << " nodes " << info.nodes << " time " << time;
        
        std::cout << "\n";
    }

    std::cout << "bestmove " << "\n";
}