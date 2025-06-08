#include "Attacks.h"
#include "Board.h"
#include "Globals.h"
#include "Move.h"
#include "MoveGen.h"
#include "MoveScoring.h"

using namespace MoveScoring;
using namespace MoveScoring::Heuristics;

inline int GetScore(const Board& board, int move, int piece, int toSquare) {
    int score = 0;
    if (move == KillerMoves[0][board.ply]) score = KillerScore0;
    else if (move == KillerMoves[1][board.ply]) score = KillerScore1;
    else {
        int lastMove = board.history[board.ply - 1].move;
        if (move == CounterMoves[GetFromSquare(lastMove)][GetToSquare(lastMove)]) score = CounterMoveScore;
        else score = HistoryMoves[piece][toSquare];
    }

    return score;
}

namespace MoveGen {
    void GenerateMoves(const Board& board, MoveList& list) {
        int side = board.side;
        int enemy = side ^ 1;

        // based on testing, there seems to be no performance difference between this and the conditional code
        int direction = side * -16 + 8;
        int promotionRank = side * -7 + 7;
        int captureStart = enemy * 6;

        list.length = 0;

        // int direction = enemy ? 8 : -8;
        // int promotionRank = enemy ? RANK_8 : RANK_1;
        // int captureStart = enemy ? BP : WP;

        // castling
        if (enemy) { // equivalent to if side == WHITE
            if (board.castlingPerms & WKC) {
                if (!IsBitSet(board.occupancy[BOTH], f1) &&
                    !IsBitSet(board.occupancy[BOTH], g1) &&
                    !board.IsSquareAttacked(e1) &&
                    !board.IsSquareAttacked(f1) &&
                    !board.IsSquareAttacked(g1)
                ) {
                    int move = EncodeMove(e1, g1, WK, 0, 0, CASTLING_FLAG);
                    int score = GetScore(board, move, WK, g1);
                    AddMove(list, score, move);
                }
            }

            if (board.castlingPerms & WQC) {
                if (!IsBitSet(board.occupancy[BOTH], d1) &&
                    !IsBitSet(board.occupancy[BOTH], c1) &&
                    !IsBitSet(board.occupancy[BOTH], b1) &&
                    !board.IsSquareAttacked(e1) &&
                    !board.IsSquareAttacked(d1) &&
                    !board.IsSquareAttacked(c1)
                ) {
                    int move = EncodeMove(e1, c1, WK, 0, 0, CASTLING_FLAG);
                    int score = GetScore(board, move, WK, c1);
                    AddMove(list, score, move);
                }
            }
        }
        else {
            if (board.castlingPerms & BKC) {
                if (!IsBitSet(board.occupancy[BOTH], f8) &&
                    !IsBitSet(board.occupancy[BOTH], g8) &&
                    !board.IsSquareAttacked(e8) &&
                    !board.IsSquareAttacked(f8) &&
                    !board.IsSquareAttacked(g8)
                ) {
                    int move = EncodeMove(e8, g8, BK, 0, 0, CASTLING_FLAG);
                    int score = GetScore(board, move, BK, g8);
                    AddMove(list, score, move);
                }
            }

            if (board.castlingPerms & BQC) {
                if (!IsBitSet(board.occupancy[BOTH], d8) &&
                    !IsBitSet(board.occupancy[BOTH], c8) &&
                    !IsBitSet(board.occupancy[BOTH], b8) &&
                    !board.IsSquareAttacked(e8) &&
                    !board.IsSquareAttacked(d8) &&
                    !board.IsSquareAttacked(c8)
                ) {
                    int move = EncodeMove(e8, c8, BK, 0, 0, CASTLING_FLAG);
                    int score = GetScore(board, move, BK, c8);
                    AddMove(list, score, move);
                }
            }
        }

        // pawns
        int pieceType = P;
        int piece = board.side * 6;
        // int piece = enemy ? WP : BP;
        U64 bitboard = board.bitboards[piece];

        if (board.enPassant != NO_SQUARE) {
            U64 fromSquares = Attacks::PawnCaptures[enemy][board.enPassant];

            while (fromSquares) {
                int fromSquare = PopFirstBit(fromSquares);

                if (IsBitSet(bitboard, fromSquare)) {
                    AddMove(list, MvvLvaScore[P][P], EncodeMove(fromSquare, board.enPassant, piece, 0, 0, ENPASSANT_FLAG));
                }
            }
        }

        while (bitboard) {
            int fromSquare = PopFirstBit(bitboard);
            U64 moves = Attacks::PawnMoves[side][fromSquare] & ~board.occupancy[BOTH];
            U64 captures = Attacks::PawnCaptures[side][fromSquare] & board.occupancy[enemy];

            while (moves) {
                int toSquare = PopFirstBit(moves);

                if (GetRank(toSquare) == promotionRank) {
                    AddMove(list, PromotionScore[Q], EncodeMove(fromSquare, toSquare, piece, 0, piece + 4, 0));
                    AddMove(list, PromotionScore[R], EncodeMove(fromSquare, toSquare, piece, 0, piece + 3, 0));
                    AddMove(list, PromotionScore[B], EncodeMove(fromSquare, toSquare, piece, 0, piece + 2, 0));
                    AddMove(list, PromotionScore[N], EncodeMove(fromSquare, toSquare, piece, 0, piece + 1, 0));
                    continue;
                }

                int flag = 0;

                if (toSquare - fromSquare == direction * 2) {
                    if (IsBitSet(board.occupancy[BOTH], fromSquare + direction)) continue;
                    flag = PAWNSTART_FLAG;
                }

                int move = EncodeMove(fromSquare, toSquare, piece, 0, 0, flag);
                int score = GetScore(board, move, piece, toSquare);

                AddMove(list, score, move);
            }

            while (captures) {
                int toSquare = PopFirstBit(captures);
                int captured = captureStart;

                for (; captured < captureStart + 6; captured++) {
                    if (IsBitSet(board.bitboards[captured], toSquare)) break;
                }

                int mvvlva = MvvLvaScore[captured][P];

                if (GetRank(toSquare) == promotionRank) {
                    AddMove(list, mvvlva + PromotionScore[Q], EncodeMove(fromSquare, toSquare, piece, captured, piece + 4, CAPTURE_FLAG));
                    AddMove(list, mvvlva + PromotionScore[R], EncodeMove(fromSquare, toSquare, piece, captured, piece + 3, CAPTURE_FLAG));
                    AddMove(list, mvvlva + PromotionScore[B], EncodeMove(fromSquare, toSquare, piece, captured, piece + 2, CAPTURE_FLAG));
                    AddMove(list, mvvlva + PromotionScore[N], EncodeMove(fromSquare, toSquare, piece, captured, piece + 1, CAPTURE_FLAG));
                    continue;
                }

                AddMove(list, mvvlva, EncodeMove(fromSquare, toSquare, piece, captured, 0, CAPTURE_FLAG));
            }
        }

        piece++;
        pieceType++;

        for (; pieceType <= K; piece++, pieceType++) {
            bitboard = board.bitboards[piece];

            while (bitboard) {
                int fromSquare = PopFirstBit(bitboard);
                U64 attacks = Attacks::GetPieceAttacks(pieceType, fromSquare, board.occupancy[BOTH]) & ~board.occupancy[side];
            
                while (attacks) {
                    int toSquare = PopFirstBit(attacks);
                    int flag = 0;
                    int score = 0;
                    int captured = captureStart;
                    int move;

                    if (IsBitSet(board.occupancy[enemy], toSquare)) {
                        flag = CAPTURE_FLAG;

                        for (; captured < captureStart + 6; captured++) {
                            if (IsBitSet(board.bitboards[captured], toSquare)) break;
                        }

                        move = EncodeMove(fromSquare, toSquare, piece, captured, 0, flag);
                        score = MvvLvaScore[captured][pieceType];
                    }
                    else {
                        move = EncodeMove(fromSquare, toSquare, piece, captured, 0, flag);
                        score = GetScore(board, move, piece, toSquare);

                        if (move == KillerMoves[0][board.ply]) score = KillerScore0;
                        else if (move == KillerMoves[1][board.ply]) score = KillerScore1;
                        //else if (move == board.history[board.ply - 1].move) score = CounterMoveScore;
                        else score = HistoryMoves[piece][toSquare];
                    }

                    AddMove(list, score, EncodeMove(fromSquare, toSquare, piece, captured, 0, flag));
                }
            }
        }
    }
}
