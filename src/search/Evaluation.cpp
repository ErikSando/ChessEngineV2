#include "Evaluation.h"

// Based on https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function

namespace Evaluation {
    const int* PieceMgTables[6] = {
        PawnMgTable, KnightMgTable, BishopMgTable, RookMgTable, QueenMgTable, KingMgTable
    };

    const int* PieceEgTables[6] = {
        PawnEgTable, KnightEgTable, BishopEgTable, RookEgTable, QueenEgTable, KingEgTable
    };

    int MgTables[12][64];
    int EgTables[12][64];

    void Init() {
        for (int piece = WP; piece <= WK; piece++) {
            for (int square = 0; square < 64; square++) {
                MgTables[piece][square] = MgValues[piece] + PieceMgTables[piece][square];
                EgTables[piece][square] = EgValues[piece] + PieceEgTables[piece][square];
            }
        }

        for (int piece = BP; piece <= BK; piece++) {
            for (int square = 0; square < 64; square++) {
                MgTables[piece][square] = MgValues[piece - 6] + PieceMgTables[piece - 6][MirrorSquare(square)];
                EgTables[piece][square] = EgValues[piece - 6] + PieceEgTables[piece - 6][MirrorSquare(square)];
            }
        }
    }

    int Evaluate(Board& board) {
        int mg[2] = { 0, 0 };
        int eg[2] = { 0, 0 };
        int mgPhase = 0;

        for (int piece = WP; piece <= BK; piece++) {
            U64 bitboard = board.bitboards[piece];
            int side = PIECE_SIDE[piece];

            while (bitboard) {
                int square = PopFirstBit(bitboard);

                mg[side] += MgTables[piece][square];
                eg[side] += EgTables[piece][square];

                mgPhase += PhaseInc[piece];
            }
        }

        int mgEval = mg[WHITE] - mg[BLACK];
        int egEval = eg[WHITE] - eg[BLACK];

        if (mgPhase > 24) mgPhase = 24;
        int egPhase = 24 - mgPhase;

        int eval = (mgEval * mgPhase + egEval + egPhase) / 24;

        return board.side == WHITE ? eval : -eval;
    }
}