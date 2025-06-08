#include "Attacks.h"
#include "Board.h"
#include "Globals.h"
#include "Move.h"
#include "MoveGen.h"

#include "Utils.h"

namespace MoveGen {
    void GenerateLegalMoves(const Board& board, MoveList& list) {
        int side = board.side;
        int enemy = side ^ 1;

        int direction = side * -16 + 8;
        int promotionRank = side * -7 + 7;
        int captureStart = enemy * 6;

        // int direction = enemy ? 8 : -8;
        // int promotionRank = enemy ? RANK_8 : RANK_1;
        // int captureStart = enemy ? BP : WP;

        list.length = 0;

        U64 attacked = board.GetAttackedSquares();
        U64 captureMask = 0xFFFFFFFFFFFFFFFFULL;
        U64 blockMask = 0xFFFFFFFFFFFFFFFFULL;

        int pieceType = P;
        int piece = side * 6;
        // int piece = enemy ? WP : BP;
        U64 bitboard = board.bitboards[piece + 5];

        while (bitboard) {
            int fromSquare = PopFirstBit(bitboard);
            U64 attacks = Attacks::KingAttacks[fromSquare] & ~board.occupancy[side] & ~attacked;

            int enemyPawns = piece ^ 6;

            U64 attackers = Attacks::PawnCaptures[side][fromSquare] & board.bitboards[enemyPawns] |
                            Attacks::KnightAttacks[fromSquare] & board.bitboards[enemyPawns + 1] |
                            Attacks::GetBishopAttacks(fromSquare, board.occupancy[BOTH]) & board.bitboards[enemyPawns + 2] |
                            Attacks::GetRookAttacks(fromSquare, board.occupancy[BOTH]) & board.bitboards[enemyPawns + 3] |
                            Attacks::GetQueenAttacks(fromSquare, board.occupancy[BOTH]) & board.bitboards[enemyPawns + 4];

            int numAttackers = CountBits(attackers);

            while (attacks) {
                int toSquare = PopFirstBit(attacks);
                int flag = 0;
                int captured = captureStart;
                int move;

                if (IsBitSet(board.occupancy[enemy], toSquare)) {
                    flag = CAPTURE_FLAG;

                    for (; captured < captureStart + 6; captured++) {
                        if (IsBitSet(board.bitboards[captured], toSquare)) break;
                    }

                    move = EncodeMove(fromSquare, toSquare, piece, captured, 0, flag);
                }
                else {
                    move = EncodeMove(fromSquare, toSquare, piece, captured, 0, flag);
                }

                AddMove(list, 0, EncodeMove(fromSquare, toSquare, piece, captured, 0, flag));
            }

            if (numAttackers > 1) return;

            if (numAttackers == 1) {
                captureMask = attackers;

                int attackerSquare = PopFirstBit(attackers);
                int attacker = enemyPawns;

                for (; attacker <= enemyPawns + 4; attacker++) {
                    if (IsBitSet(board.bitboards[attacker], attackerSquare)) break;
                }

                blockMask = 0ULL;

                if (IS_PIECE_SLIDER[attacker]) {
                    blockMask = Attacks::SliderRays[fromSquare][attackerSquare];
                }
            }
        }

        // castling
        if (enemy) { // equivalent to if side == WHITE
            if (board.castlingPerms & WKC) {
                if (!IsBitSet(board.occupancy[BOTH], f1) &&
                    !IsBitSet(board.occupancy[BOTH], g1) &&
                    !IsBitSet(attacked, e1) &&
                    !IsBitSet(attacked, f1) &&
                    !IsBitSet(attacked, g1)
                ) {
                    int move = EncodeMove(e1, g1, WK, 0, 0, CASTLING_FLAG);
                    AddMove(list, 0, move);
                }
            }

            if (board.castlingPerms & WQC) {
                if (!IsBitSet(board.occupancy[BOTH], d1) &&
                    !IsBitSet(board.occupancy[BOTH], c1) &&
                    !IsBitSet(board.occupancy[BOTH], b1) &&
                    !IsBitSet(attacked, e1) &&
                    !IsBitSet(attacked, d1) &&
                    !IsBitSet(attacked, c1)
                ) {
                    int move = EncodeMove(e1, c1, WK, 0, 0, CASTLING_FLAG);
                    AddMove(list, 0, move);
                }
            }
        }
        else {
            if (board.castlingPerms & BKC) {
                if (!IsBitSet(board.occupancy[BOTH], f8) &&
                    !IsBitSet(board.occupancy[BOTH], g8) &&
                    !IsBitSet(attacked, e8) &&
                    !IsBitSet(attacked, f8) &&
                    !IsBitSet(attacked, g8)
                ) {
                    int move = EncodeMove(e8, g8, BK, 0, 0, CASTLING_FLAG);
                    AddMove(list, 0, move);
                }
            }

            if (board.castlingPerms & BQC) {
                if (!IsBitSet(board.occupancy[BOTH], d8) &&
                    !IsBitSet(board.occupancy[BOTH], c8) &&
                    !IsBitSet(board.occupancy[BOTH], b8) &&
                    !IsBitSet(attacked, e8) &&
                    !IsBitSet(attacked, d8) &&
                    !IsBitSet(attacked, c8)
                ) {
                    int move = EncodeMove(e8, c8, BK, 0, 0, CASTLING_FLAG);
                    AddMove(list, 0, move);
                }
            }
        }

        if (board.enPassant != NO_SQUARE) {
            U64 fromSquares = Attacks::PawnCaptures[enemy][board.enPassant];

            while (fromSquares) {
                int fromSquare = PopFirstBit(fromSquares);

                if (IsBitSet(bitboard, fromSquare)) {
                    AddMove(list, 0, EncodeMove(fromSquare, board.enPassant, piece, 0, 0, ENPASSANT_FLAG));
                }
            }
        }

        bitboard = board.bitboards[piece];

        while (bitboard) {
            int fromSquare = PopFirstBit(bitboard);
            U64 moves = Attacks::PawnMoves[side][fromSquare] & ~board.occupancy[BOTH];
            U64 captures = Attacks::PawnCaptures[side][fromSquare] & board.occupancy[enemy];

            while (moves) {
                int toSquare = PopFirstBit(moves);

                if (GetRank(toSquare) == promotionRank) {
                    AddMove(list, 0, EncodeMove(fromSquare, toSquare, piece, 0, piece + 4, 0));
                    AddMove(list, 0, EncodeMove(fromSquare, toSquare, piece, 0, piece + 3, 0));
                    AddMove(list, 0, EncodeMove(fromSquare, toSquare, piece, 0, piece + 2, 0));
                    AddMove(list, 0, EncodeMove(fromSquare, toSquare, piece, 0, piece + 1, 0));
                    continue;
                }

                int flag = 0;

                if (toSquare - fromSquare == direction * 2) {
                    if (IsBitSet(board.occupancy[BOTH], fromSquare + direction)) continue;
                    flag = PAWNSTART_FLAG;
                }

                int move = EncodeMove(fromSquare, toSquare, piece, 0, 0, flag);

                AddMove(list, 0, move);
            }

            while (captures) {
                int toSquare = PopFirstBit(captures);
                int captured = captureStart;

                for (; captured < captureStart + 6; captured++) {
                    if (IsBitSet(board.bitboards[captured], toSquare)) break;
                }

                if (GetRank(toSquare) == promotionRank) {
                    AddMove(list, 0, EncodeMove(fromSquare, toSquare, piece, captured, piece + 4, CAPTURE_FLAG));
                    AddMove(list, 0, EncodeMove(fromSquare, toSquare, piece, captured, piece + 3, CAPTURE_FLAG));
                    AddMove(list, 0, EncodeMove(fromSquare, toSquare, piece, captured, piece + 2, CAPTURE_FLAG));
                    AddMove(list, 0, EncodeMove(fromSquare, toSquare, piece, captured, piece + 1, CAPTURE_FLAG));
                    continue;
                }

                AddMove(list, 0, EncodeMove(fromSquare, toSquare, piece, captured, 0, CAPTURE_FLAG));
            }
        }

        piece++;
        pieceType++;

        for (; pieceType <= Q; piece++, pieceType++) {
            bitboard = board.bitboards[piece];

            while (bitboard) {
                int fromSquare = PopFirstBit(bitboard);
                U64 attacks = Attacks::GetPieceAttacks(pieceType, fromSquare, board.occupancy[BOTH]) & ~board.occupancy[side];
            
                while (attacks) {
                    int toSquare = PopFirstBit(attacks);
                    int flag = 0;
                    int captured = captureStart;
                    int move;

                    if (IsBitSet(board.occupancy[enemy], toSquare)) {
                        flag = CAPTURE_FLAG;

                        for (; captured < captureStart + 6; captured++) {
                            if (IsBitSet(board.bitboards[captured], toSquare)) break;
                        }

                        move = EncodeMove(fromSquare, toSquare, piece, captured, 0, flag);
                    }
                    else {
                        move = EncodeMove(fromSquare, toSquare, piece, captured, 0, flag);
                    }

                    AddMove(list, 0, EncodeMove(fromSquare, toSquare, piece, captured, 0, flag));
                }
            }
        }
    }
}