#include "Evaluation.h"

// Based on https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function

namespace Evaluation {
    void Init() {
        
    }

    int Evaluate(Board& board) {
        return 0;

        int pawns[2] = { 0, 0 };
        int knights[2] = { 0, 0 };
        int bishops[2] = { 0, 0 };
        int rooks[2] = { 0, 0 };
        int queens[2] = { 0, 0 };

        int tPawns = 0, tKnights = 0, tBishops = 0, tRooks = 0, tQueens = 0;

        int mg[2] = { 0, 0 };
        int eg[2] = { 0, 0 };
        int phase = 0;

        int piece = WP;
        U64 bitboard = board.bitboards[piece];

        while (bitboard) {
            int square = PopFirstBit(bitboard);
            
            pawns[WHITE]++;

            mg[WHITE] += PieceMgTables[WP][square];
            eg[WHITE] += PieceEgTables[WP][square];

            tPawns++;
        }

        piece = BP;
        bitboard = board.bitboards[piece];

        while (bitboard) {
            int square = PopFirstBit(bitboard);
            
            pawns[BLACK]++;

            tPawns++;
        }

        piece = WN;
        bitboard = board.bitboards[piece];

        while (bitboard) {
            int square = PopFirstBit(bitboard);
            
            knights[WHITE]++;

            tKnights++;
        }

        piece = BN;
        bitboard = board.bitboards[piece];

        while (bitboard) {
            int square = PopFirstBit(bitboard);
            
            knights[BLACK]++;

            tKnights++;
        }

        int mgEval = mg[WHITE] - mg[BLACK];
        int egEval = eg[WHITE] - eg[BLACK];
    }
}