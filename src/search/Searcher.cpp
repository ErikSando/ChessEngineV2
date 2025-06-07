#include <iostream>

#include "Board.h"
#include "Evaluation.h"
#include "MoveGen.h"
#include "Searcher.h"
#include "Utils.h"

void SearchInfo::Reset() {
    nodes = 0;
    depth = MAX_DEPTH;
    timeSet = false;
    stopped = false;
}

void Searcher::ClearTTable() {
    ttable.Clear();
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

    int pvNode = beta - alpha > 1;
    int pvMove = 0;
    int score = ttable.GetEntry(board.hashKey, pvMove, alpha, beta, depth);

    if (board.ply && !pvNode && score != NO_SCORE) return score;

    MoveList list;
    MoveGen::GenerateMoves(board, list);

    if (pvMove) {
        for (int i = 0; i < list.length; i++) {
            if (pvMove == list.moves[i].move) {
                list.moves[i].score = INF;
                break;
            }
        }
    }

    OrderMoves(list);

    int legalMoves = 0;
    int bestMove = 0;
    int hashFlag = ALPHA_FLAG;

    for (int i = 0; i < list.length; i++) {
        int move = list.moves[i].move;

        if (!board.MakeMove(move)) continue;
        int score = -AlphaBeta(board, info, depth - 1, -beta, -alpha);
        legalMoves++;
        board.TakeMove();

        if (score >= beta) {
            ttable.StoreEntry(board.hashKey, move, score, BETA_FLAG, depth);
            return beta;
        }

        if (score > alpha) {
            alpha = score;
            bestMove = move;
            hashFlag = EXACT_FLAG;
        }
    }

    if (!legalMoves) {
        int KP = board.side == WHITE ? WK : BK;
        if (board.IsSquareAttacked(FirstBitIndex(board.bitboards[KP]))) return -INF + board.ply;
        else return 0;
    }

    ttable.StoreEntry(board.hashKey, bestMove, alpha, hashFlag, depth);

    return alpha;
}

void Searcher::Search(Board& board, SearchInfo& info) {
    int alpha = -INF;
    int beta = INF;

    for (int depth = 1; depth <= info.depth; depth++) {
        int score = AlphaBeta(board, info, depth, alpha, beta);

        if (info.stopped) break;

        int now = Utils::GetTimeMS();
        int time = now - info.startTime;

        if (!info.post) continue;

        switch (info.postType) {
            case DEFAULT: {
                int knps = 0;
                double seconds = (double) time / 1000;

                if (seconds > 0) {
                    knps = info.nodes / seconds / 1000;
                }

                std::cout << "Depth: " << depth << ", eval: " << score << ", nodes: " << info.nodes;
                std::cout << ", time: " << time << ", knps: ";
                knps != 0 ? std::cout << knps : std::cout << "undef";
                std::cout << ", moves: ";
                // print pv line
                break;
            }

            case UCI:
                std::cout << "info depth " << depth << " cp " << score;
                std::cout << " nodes " << info.nodes << " time " << time;
                std::cout << " pv "; // print pv line
            break;
        }

        std::cout << "\n";
    }

    if (info.quitting) return;

    std::cout << "bestmove " << "\n";
}