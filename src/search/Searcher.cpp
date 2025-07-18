#include <algorithm>
#include <cmath>
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

namespace ErikEngine {
    constexpr int NULL_MOVE_REDUCTION = 2;

    constexpr int FULL_DEPTH_MOVES = 5;
    constexpr int MIN_REDUCTION_DEPTH = 3;
    constexpr int MAX_REDUCTION = 3;
    constexpr float REDUCTION_FACTOR = 0.5;

    constexpr int ASPIRATION_WINDOW = 50;
    constexpr int WINDOW_EXPANSION_FACTOR = 30;

    constexpr int MAX_FUTILITY_PRUNE_DEPTH = 2;
    constexpr int FUTILITY_MARGIN_FACTOR = 100;
    constexpr int MIN_NON_FUTILITY_PRUNED = 3;

    inline int CalculateWindow(int fails) {
        return ASPIRATION_WINDOW + fails * WINDOW_EXPANSION_FACTOR;
    }

    void Searcher::ClearTTable() {
        ttable.Clear();
    }

    inline void CheckTimeUp(SearchInfo& info) {
        info.stopped = info.timeSet && (Utils::GetTimeMS() > info.stopTime);
    }

    inline bool ThreeFoldRepetition(Board& board) {
        int repetitions = 0;

        for (int i = board.ply - board.fiftyMoveCount; i < board.ply; i++) {
            if (board.hashKey == board.history[i].hashKey) {
                repetitions++;
                if (repetitions >= 2) return true;
            }
        }

        return false;
    }

    inline void OrderMoves(MoveList& list) {
        std::sort(list.begin(), list.end(), [&] (const Move& a, const Move& b) {
            return a.score > b.score;
        });
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

        for (size_t i = 0; i < list.size(); i++) {
            if (!board.MakeMove(list.move_at(i))) continue;
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

        bool pvNode = beta - alpha > 1;
        int pvMove = 0;

        int score = ttable.GetEntry(board, pvMove, alpha, beta, depth);
        if (score != NO_SCORE /*&& board.ply*/ && !pvNode) return score;

        if (board.ply >= MAX_DEPTH) return Evaluation::Evaluate(board);

        int kingSquare = FirstBitIndex(board.bitboards[board.side == WHITE ? WK : BK]);
        bool inCheck = board.IsSquareAttacked(kingSquare);

        int nullMoveR = NULL_MOVE_REDUCTION + (depth > 6);

        // null move pruning
        if (doNull && !inCheck && depth > nullMoveR && board.bigPieces[board.side] > 1) {
            board.MakeNullMove();
            int score = -AlphaBeta(board, info, depth - 1 - nullMoveR, -beta, -beta + 1, false);
            board.TakeNullMove();

            if (score >= beta && std::abs(score) < MATE_SCORE) return beta;
        }

        int enemyKingSquare = FirstBitIndex(board.bitboards[board.side == WHITE ? BK : WK]);
        int enemyInCheck = board.IsSquareAttacked(enemyKingSquare, board.side);

        int extension = 0;

        // check extension
        if (inCheck || enemyInCheck) extension = 1;

        MoveList list;
        MoveGen::GenerateMoves(board, list);

        // search PV move first
        if (pvMove) {
            for (size_t i = 0; i < list.size(); i++) {
                if (pvMove == list.move_at(i)) {
                    list[i].score = INF;
                    break;
                }
            }
        }

        OrderMoves(list);

        int legalMoves = 0;
        int bestMove = 0;
        int bestScore = -INF;
        int hashFlag = ALPHA_FLAG;

        for (size_t i = 0; i < list.size(); i++) {
            int move = list.move_at(i);

            // futility pruning
            if (
                depth <= MAX_FUTILITY_PRUNE_DEPTH &&
                legalMoves > MIN_NON_FUTILITY_PRUNED &&
                !inCheck &&
                list.score_at(i) == 0
            ) {
                int eval = Evaluation::Evaluate(board);
                if (eval + FUTILITY_MARGIN_FACTOR * depth <= alpha) continue;
            }

            if (!board.MakeMove(move)) continue;

            int score;

            if (legalMoves == 0) { // legalMoves is considered "moves searched so far" in this context
                score = -AlphaBeta(board, info, depth - 1 + extension, -beta, -alpha, true);
            }
            // late move reductions
            else if (
                !inCheck &&
                legalMoves >= FULL_DEPTH_MOVES &&
                depth >= MIN_REDUCTION_DEPTH &&
                list.score_at(i) == 0
            ) {
                // int reduction = REDUCTION_FACTOR * std::log(depth) * std::log(legalMoves - FULL_DEPTH_MOVES);
                // reduction = std::clamp(reduction, 1, MAX_REDUCTION);
                // int newDepth = std::max(1, depth - 1 - reduction) + extension;
                int newDepth = depth - 2 + extension;

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

            // beta cut off
            if (score >= beta) {
                ttable.StoreEntry(board, move, score, BETA_FLAG, depth);

                if (GetFlag(move) != CAPTURE_FLAG) {
                    MoveScoring::KillerMoves[1][board.ply] = MoveScoring::KillerMoves[0][board.ply];
                    MoveScoring::KillerMoves[0][board.ply] = move;

                    //int previousMove = board.history[board.ply - 1].move;

                    //MoveScoring::CounterMoves[GetFromSquare(previousMove)][GetToSquare(previousMove)] = move;
                }

                return beta;
            }

            if (score > bestScore) {
                bestMove = move;
                bestScore = score;

                if (score > alpha) {
                    alpha = score;
                    hashFlag = EXACT_FLAG;

                    if (GetFlag(move) != CAPTURE_FLAG) {
                        //MoveScoring::UpdateHistory(MoveScoring::HistoryMoves[GetMovedPiece(move)][GetToSquare(move)], depth);
                        MoveScoring::HistoryMoves[GetMovedPiece(move)][GetToSquare(move)] += depth;
                    }
                }
            }
        }

        if (!legalMoves) {
            return inCheck ? -INF + board.ply : 0;
        }

        ttable.StoreEntry(board, bestMove, bestScore, hashFlag, depth);

        return alpha;
    }

    void Searcher::Search(Board& board, SearchInfo& info) {
        board.ply = 0;
        ttable.IncAge();
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

            ttable.GetPVLine(board, pvLine, depth);

            if (!info.post) continue;

            std::ostringstream pvString;

            for (int i = 0; i < MAX_DEPTH && pvLine[i]; i++) {
                pvString << " " << Utils::ToMoveString(pvLine[i]);
            }

            U64 time = Utils::GetTimeMS() - info.startTime;

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

        if (info.quitting || !info.postBestMove) return;

        std::cout << "bestmove " << Utils::ToMoveString(pvLine[0]) << std::endl;
    }
}