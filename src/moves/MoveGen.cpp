#include "Attacks.h"
#include "Board.h"
#include "Globals.h"
#include "Move.h"
#include "MoveGen.h"

namespace MoveGen {
    U64 RANK_1_MASK = 0x00000000000000FFULL;

    // TODO: add move scoring so this can be used in search
    void GenerateMoves(Board& board, MoveList& list) {
        int side = board.side;
        int enemy = side ^ 1;

        // int direction = side * -16 + 8;
        // int promotionRank = side * -7 + 7;
        // int captureStart = enemy * 6;

        int direction = enemy ? 8 : -8;
        int promotionRank = enemy ? RANK_8 : RANK_1;
        int captureStart = enemy ? BP : WP;

        list.length = 0;

        U64 attacked = board.GetAttackedSquares();
        U64 captureMask = 0xFFFFFFFFFFFFFFFFULL;
        U64 blockMask = 0xFFFFFFFFFFFFFFFFULL;

        int pieceType = P;
        // int piece = side * 6;
        int piece = enemy ? WP : BP;
        int kingPiece = piece + 5;
        U64 bitboard = board.bitboards[kingPiece];

        int kingSquare = PopFirstBit(bitboard);
        U64 attacks = Attacks::KingAttacks[kingSquare] & ~board.occupancy[side] & ~attacked;

        int enemyPawn = piece ^ 6;

        U64 attackers = Attacks::PawnCaptures[side][kingSquare] & board.bitboards[enemyPawn] |
                        Attacks::KnightAttacks[kingSquare] & board.bitboards[enemyPawn + N] |
                        Attacks::GetBishopAttacks(kingSquare, board.occupancy[BOTH]) & board.bitboards[enemyPawn + B] |
                        Attacks::GetRookAttacks(kingSquare, board.occupancy[BOTH]) & board.bitboards[enemyPawn + R] |
                        Attacks::GetQueenAttacks(kingSquare, board.occupancy[BOTH]) & board.bitboards[enemyPawn + Q];

        int numAttackers = CountBits(attackers);

        while (attacks) {
            int toSquare = PopFirstBit(attacks);
            int flag = 0;
            int captured = captureStart;

            if (IsBitSet(board.occupancy[enemy], toSquare)) {
                flag = CAPTURE_FLAG;

                for (; captured < captureStart + 6; captured++) {
                    if (IsBitSet(board.bitboards[captured], toSquare)) break;
                }
            }

            AddMove(list, 0, EncodeMove(kingSquare, toSquare, kingPiece, captured, 0, flag));
        }

        if (numAttackers > 1) return;

        if (numAttackers == 1) {
            captureMask = attackers;

            int attackerSquare = PopFirstBit(attackers);
            int attacker = enemyPawn;

            for (; attacker <= enemyPawn + 4; attacker++) {
                if (IsBitSet(board.bitboards[attacker], attackerSquare)) break;
            }

            blockMask = 0ULL;

            if (IS_PIECE_SLIDER[attacker]) {
                blockMask = Attacks::GetSliderRay(kingSquare, attackerSquare);
            }
        }
        
        U64 pinned = 0ULL;
        U64 pinRays[64] = {};

        for (int sliderPiece = enemyPawn + B; sliderPiece <= enemyPawn + Q; sliderPiece++) {
            U64 pieces = board.bitboards[sliderPiece];

            while (pieces) {
                int attackerSquare = PopFirstBit(pieces);
                
                int pieceType = sliderPiece - enemyPawn;

                U64 ray = Attacks::GetSliderRay(kingSquare, attackerSquare, pieceType);
                if (!ray) continue;

                U64 blockers = ray & board.occupancy[BOTH];

                if (blockers && !(blockers & (blockers - 1))) {
                    int pinnedSquare = FirstBitIndex(blockers);
                    pinned |= (1ULL << pinnedSquare);
                    pinRays[pinnedSquare] = ray | (1ULL << attackerSquare);
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

        // pawns
        bitboard = board.bitboards[piece];

        if (board.enPassant != NO_SQUARE) {
            U64 candidates = Attacks::PawnCaptures[enemy][board.enPassant] & bitboard;

            while (candidates) {
                int fromSquare = PopFirstBit(candidates);

                if (GetBit(pinned, fromSquare)) continue;

                ClearBit(board.bitboards[piece], fromSquare);
                ClearBit(board.bitboards[enemyPawn], board.enPassant - direction);
                ClearBit(board.occupancy[BOTH], fromSquare);
                ClearBit(board.occupancy[BOTH], board.enPassant - direction);

                int rank = GetRank(kingSquare);

                U64 rankMask = RANK_1_MASK << rank * 8;
                U64 attacks = Attacks::GetQueenAttacks(kingSquare, board.occupancy[BOTH]) & rankMask;

                bool illegal = (attacks & (board.bitboards[enemyPawn + R] | board.bitboards[enemyPawn + Q]));

                SetBit(board.bitboards[piece], fromSquare);
                SetBit(board.bitboards[enemyPawn], board.enPassant - direction);
                SetBit(board.occupancy[BOTH], fromSquare);
                SetBit(board.occupancy[BOTH], board.enPassant - direction);

                if (illegal) continue; // probably could be a break, i dont think the other pawn (if there is one) could legally make the move

                AddMove(list, 0, EncodeMove(fromSquare, board.enPassant, piece, 0, 0, ENPASSANT_FLAG));
            }
        }

        while (bitboard) {
            int fromSquare = PopFirstBit(bitboard);

            U64 ray = 0xFFFFFFFFFFFFFFFFULL;

            if (GetBit(pinned, fromSquare)) {
                if (pieceType == N) break;
                ray = pinRays[fromSquare];
            }
            
            U64 moves = Attacks::PawnMoves[side][fromSquare] & ~board.occupancy[BOTH] & blockMask & ray;
            U64 captures = Attacks::PawnCaptures[side][fromSquare] & board.occupancy[enemy] & captureMask & ray;

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

                AddMove(list, 0, EncodeMove(fromSquare, toSquare, piece, 0, 0, flag));
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

        for (pieceType = N; pieceType <= Q; piece++, pieceType++) {
            bitboard = board.bitboards[piece];

            while (bitboard) {
                int fromSquare = PopFirstBit(bitboard);

                U64 ray = 0xFFFFFFFFFFFFFFFFULL;

                if (GetBit(pinned, fromSquare)) {
                    if (pieceType == N) continue;
                    ray = pinRays[fromSquare];
                }

                U64 attacks = Attacks::GetPieceAttacks(pieceType, fromSquare, board.occupancy[BOTH]) & ~board.occupancy[side] & (blockMask | captureMask) & ray;
            
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
                    }

                    AddMove(list, 0, EncodeMove(fromSquare, toSquare, piece, captured, 0, flag));
                }
            }
        }
    }
}
