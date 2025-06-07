#include <iostream>
#include <sstream>
#include <string.h>

#include "Board.h"
#include "Evaluation.h"
#include "MoveGen.h"
#include "Searcher.h"
#include "Utils.h"

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

int Searcher::AlphaBeta(Board& board, SearchInfo& info, int depth, int alpha, int beta, bool doNull) {
    if ((info.nodes & 4095) == 0) CheckTimeUp(info);
    if (info.stopped) return 0;
    if (depth <= 0) return Quiescence(board, info, alpha, beta);

    info.nodes++;

    if (board.fiftyMoveCount >= 100 || ThreeFoldRepetition(board)) return 0;

    int pvNode = beta - alpha > 1;
    int pvMove = 0;
    int score = ttable.GetEntry(board.hashKey, pvMove, alpha, beta, depth);

    if (board.ply && !pvNode && score != NO_SCORE) return score;

    int kingPiece = board.side == WHITE ? WK : BK;
    int kingSquare = FirstBitIndex(board.bitboards[kingPiece]);
    bool inCheck = board.IsSquareAttacked(kingSquare);

    int R = NULL_MOVE_REDUCTION + (depth > 6);

    if (doNull && !inCheck && depth > R + 1 && board.bigPieces[board.side] > 1) {
        board.MakeNullMove();
        int score = -AlphaBeta(board, info, depth - 1 - R, beta - 1, beta, false);
        board.TakeNullMove();

        if (score >= beta && std::abs(score) < MATE_SCORE) return beta;
    }

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
        int score = -AlphaBeta(board, info, depth - 1, -beta, -alpha, doNull);
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
        return inCheck ? -INF + board.ply : 0;
    }

    ttable.StoreEntry(board.hashKey, bestMove, alpha, hashFlag, depth);

    return alpha;
}

void Searcher::Search(Board& board, SearchInfo& info) {
    board.ply = 0;
    ttable.age++;

    info.nodes = 0ULL;
    info.stopped = false;

    int alpha = -INF;
    int beta = INF;
    int fails = 0;

    int pvLine[MAX_DEPTH];
    memset(pvLine, 0, sizeof(pvLine));

    for (int depth = 1; depth <= info.depth;) {
        int score = AlphaBeta(board, info, depth, alpha, beta, true);

        if (score <= alpha) {
            int delta = CalculateWindow(fails);
            fails++;
            alpha = score - delta;
            continue;
        }

        if (score >= beta) {
            int delta = CalculateWindow(fails);
            fails++;
            beta = score + delta;
            continue;
        }

        fails = 0;
        alpha = score - ASPIRATION_WINDOW;
        beta = score + ASPIRATION_WINDOW;

        if (info.stopped) break;

        int time = Utils::GetTimeMS() - info.startTime;

        if (!info.post) continue;

        ttable.GetPVLine(board, pvLine, depth);

        std::ostringstream pvString;

        for (int i = 0; i < MAX_DEPTH && pvLine[i]; i++) {
            pvString << " " << Utils::ToMoveString(pvLine[i]);
        }

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
                std::cout << ", moves:" << pvString.str();
                break;
            }

            case UCI:
                std::cout << "info depth " << depth << " score cp " << score;
                std::cout << " nodes " << info.nodes << " time " << time;
                std::cout << " pv" << pvString.str();
            break;
        }

        std::cout << std::endl; // need to flush std::cout

        depth++;
    }

    if (info.quitting) return;

    std::cout << "bestmove " << Utils::ToMoveString(pvLine[0]) << std::endl;
}