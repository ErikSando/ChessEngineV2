#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string.h>

#include "Board.h"
#include "Evaluation.h"
#include "Move.h"
#include "MoveGen.h"
#include "MoveScoring.h"
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
    // for (int i = 0; i < list.length; i++) {
    //     std::cout << Utils::ToMoveString(list.moves[i].move) << ": " << list.moves[i].score << "\n";
    // }

    // std::cout << "\n";

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

    // for (int i = 0; i < list.length; i++) {
    //     std::cout << Utils::ToMoveString(list.moves[i].move) << ": " << list.moves[i].score << "\n";
    // }

    // std::cout << "\n";
}

int Searcher::Quiescence(Board& board, SearchInfo& info, int alpha, int beta) {
    if ((info.nodes & 4095) == 0) CheckTimeUp(info);
    if (info.stopped) return 0;

    info.nodes++;

    if (board.fiftyMoveCount >= 100 || ThreeFoldRepetition(board)) return 0;

    int eval = Evaluation::Evaluate(board);

    if (board.ply >= MAX_DEPTH) return eval;

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

    if (score != NO_SCORE && board.ply && !pvNode) return score;

    if (board.ply >= MAX_DEPTH) return Evaluation::Evaluate(board);

    int kingSquare = FirstBitIndex(board.bitboards[board.side == WHITE ? WK : BK]);
    bool inCheck = board.IsSquareAttacked(kingSquare);

    int nullMoveR = NULL_MOVE_REDUCTION + (depth > 6);

    if (doNull && !inCheck && depth > nullMoveR + 1 && board.bigPieces[board.side] > 1) { // null move pruning
        board.MakeNullMove();
        int score = -AlphaBeta(board, info, depth - 1 - nullMoveR, beta - 1, beta, false);
        board.TakeNullMove();

        if (score >= beta && std::abs(score) < MATE_SCORE) return beta;
    }

    int enemyKingSquare = FirstBitIndex(board.bitboards[board.side == WHITE ? BK : WK]);
    int enemyInCheck = board.IsSquareAttacked(enemyKingSquare, board.side);

    int extension = 0;

    if (inCheck || enemyInCheck) extension = 1; // check extension

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

        int score;

        if (legalMoves == 0) { // legalMoves is considered "moves searched so far" in this context
            score = -AlphaBeta(board, info, depth - 1 + extension, -beta, -alpha, true);
        }
        else if (!inCheck && legalMoves >= FULL_DEPTH_MOVES && depth >= MIN_REDUCTION_DEPTH && list.moves[i].score == 0) { // late move reductions
            int reduction = std::clamp((legalMoves - 4) / 3, 0, MAX_REDUCTION); // reduce depth by 1 for every 3 moves searched until the maximum is hit
            int newDepth = std::max(1, depth - 1 - reduction) + extension;

            score = -AlphaBeta(board, info, newDepth, -alpha - 1, -alpha, true);
        }
        else {
            score = -AlphaBeta(board, info, depth - 1 + extension, -alpha - 1, -alpha, true);

            if (score > alpha && score < beta) {
                score = -AlphaBeta(board, info, depth - 1 + extension, -beta, -alpha, true);
            }
        }

        legalMoves++;

        board.TakeMove();

        if (score >= beta) { // beta cut off
            ttable.StoreEntry(board.hashKey, move, score, BETA_FLAG, depth);

            if (GetFlag(move) != CAPTURE_FLAG) {
                MoveScoring::KillerMoves[1][board.ply] = MoveScoring::KillerMoves[0][board.ply];
                MoveScoring::KillerMoves[0][board.ply] = move;

                int previousMove = board.history[board.ply - 1].move;

                MoveScoring::CounterMoves[GetFromSquare(previousMove)][GetToSquare(previousMove)] = move;
            }

            return beta;
        }

        if (score > alpha) {
            alpha = score;
            bestMove = move;
            hashFlag = EXACT_FLAG;

            if (GetFlag(move) != CAPTURE_FLAG) {
                MoveScoring::UpdateHistory(MoveScoring::HistoryMoves[GetMovedPiece(move)][GetToSquare(move)], depth);
                //MoveScoring::HistoryMoves[GetMovedPiece(move)][GetToSquare(move)] += depth;
            }
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
    MoveScoring::Reset();

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

                std::cout << "Depth: " << depth << ", eval: ";

                if (score > MATE_SCORE) std::cout << "+M" << (int) ((INF - score) / 2 + 1); // int cast rounds down
                else if (score < -MATE_SCORE) std::cout << "-M" << (int) (-(INF + score) / 2 - 1);
                else {
                    std::cout << std::fixed << std::setprecision(2) << std::showpos;
                    std::cout << ((double) score / 100) << std::noshowpos << std::defaultfloat;
                }

                std::cout<< ", nodes: " << info.nodes << ", time: " << time << ", knps: ";
                knps != 0 ? std::cout << knps : std::cout << "undef";
                std::cout  << ", moves:" << pvString.str();
                break;
            }

            case UCI:
                std::cout << "info depth " << depth << " score ";

                if (score > MATE_SCORE) std::cout << "mate " << (int) ((INF - score) / 2 + 1);
                else if (score < -MATE_SCORE) std::cout << "mate " << (int) (-(INF + score) / 2 - 1);
                else std::cout << "cp " << score;

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