#include "Evaluation.h"
#include "Utils.h"

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
                MgTables[piece][square] = MgValues[piece] + PieceMgTables[piece][MirrorSquare(square)];
                EgTables[piece][square] = EgValues[piece] + PieceEgTables[piece][MirrorSquare(square)];

                MgTables[piece + 6][square] = MgValues[piece] + PieceMgTables[piece][square];
                EgTables[piece + 6][square] = EgValues[piece] + PieceEgTables[piece][square];
            }
        }
    }

    inline bool MaterialDraw(Board& board) {
        int pawns[2] = { CountBits(board.bitboards[WP]), CountBits(board.bitboards[BP]) };
        if (pawns[WHITE] || pawns[BLACK]) return false;

        int knights[2] = { CountBits(board.bitboards[WN]), CountBits(board.bitboards[BN]) };
        int bishops[2] = { CountBits(board.bitboards[WB]), CountBits(board.bitboards[BB]) };
        int rooks[2] = { CountBits(board.bitboards[WR]), CountBits(board.bitboards[BR]) };
        int queens[2] = { CountBits(board.bitboards[WQ]), CountBits(board.bitboards[BQ]) };

        

        return false;
    }

    int Evaluate(Board& board) {
        if (MaterialDraw(board)) return 0;

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

        int eval = (mgEval * mgPhase + egEval * egPhase) / 24;

        return board.side == WHITE ? eval + Heuristics::Tempo : -eval + Heuristics::Tempo;
    }
}