#include "Attacks.h"
#include "Board.h"
#include "Globals.h"
#include "Move.h"
#include "MoveGen.h"

namespace MoveGen {
    void GenerateCaptures(const Board& board, MoveList& list) {
        int side = board.side;
        int enemy = side ^ 1;
        int direction = enemy ? 8 : -8;
        int promotionRank = enemy ? RANK_8 : RANK_1;
        int captureStart = enemy ? BP : WP;

        // pawns
        int pieceType = P;
        int piece = enemy ? WP : BP;
        U64 bitboard = board.bitboards[piece];

        while (bitboard) {
            int fromSquare = PopFirstBit(bitboard);
            U64 captures = Attacks::PawnCaptures[side][fromSquare] & board.occupancy[enemy];

            if (board.enPassant != NO_SQUARE) {
                if (IsBitSet(Attacks::PawnCaptures[side][fromSquare], board.enPassant)) {
                    AddMove(list, 0, EncodeMove(fromSquare, board.enPassant, piece, 0, 0, ENPASSANT_FLAG));
                }
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

        for (; pieceType <= K; piece++, pieceType++) {
            bitboard = board.bitboards[piece];

            while (bitboard) {
                int fromSquare = PopFirstBit(bitboard);
                U64 attacks = Attacks::GetPieceAttacks(pieceType, fromSquare, board.occupancy[BOTH]) & board.occupancy[enemy];
            
                while (attacks) {
                    int toSquare = PopFirstBit(attacks);
                    int captured = captureStart;

                    for (; captured < captureStart + 6; captured++) {
                        if (IsBitSet(board.bitboards[captured], toSquare)) break;
                    }

                    AddMove(list, 0, EncodeMove(fromSquare, toSquare, piece, captured, 0, CAPTURE_FLAG));
                }
            }
        }
    }
}