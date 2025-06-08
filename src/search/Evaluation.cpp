#include <string.h>

#include "Evaluation.h"
#include "EvalParams.h"
#include "Utils.h"

namespace Evaluation {
    int MgTables[12][64];
    int EgTables[12][64];

    namespace Masks {
        U64 Files[8];
        U64 Ranks[8];
        U64 PassedPawnWhite[64];
        U64 PassedPawnBlack[64];
        U64 IsolatedPawn[64]; // file indexing can be used but square indexing is preferred
        U64 StackedPawn[64];
        U64 ClosePawnShieldWhite[64];
        U64 FarPawnShieldWhite[64];
        U64 ClosePawnShieldBlack[64];
        U64 FarPawnShieldBlack[64];

        constexpr U64 CentreMask = 0x0000001818000000ULL;
        constexpr U64 ExtendedCentreMask = 0x00003C24243C0000ULL;

        inline U64 Square(int square) {
            return square >= a1 && square <= h8 ? 1ULL << square : 0ULL;
        }
    }

    const int* PieceMgTables[6] = {
        PawnMgTable, KnightMgTable, BishopMgTable, RookMgTable, QueenMgTable, KingMgTable
    };

    const int* PieceEgTables[6] = {
        PawnEgTable, KnightEgTable, BishopEgTable, RookEgTable, QueenEgTable, KingEgTable
    };

    void InitPieceSquareTables() {
        for (int piece = WP; piece <= WK; piece++) {
            for (int square = 0; square < 64; square++) {
                MgTables[piece][square] = MgValues[piece] + PieceMgTables[piece][MirrorSquare(square)];
                EgTables[piece][square] = EgValues[piece] + PieceEgTables[piece][MirrorSquare(square)];

                MgTables[piece + 6][square] = MgValues[piece] + PieceMgTables[piece][square];
                EgTables[piece + 6][square] = EgValues[piece] + PieceEgTables[piece][square];
            }
        }
    }

    void InitMasks() {
        Masks::Files[FILE_A] = 0x0101010101010101ULL;
        Masks::Ranks[RANK_1] = 0x00000000000000FFULL;

        for (int i = 1; i < 8; i++) {
            Masks::Files[i] = Masks::Files[FILE_A] << i;
            Masks::Ranks[i] = Masks::Ranks[RANK_1] << i * 8;
        }

        U64 NotFileA = ~Masks::Files[FILE_A];
        U64 NotFileH = ~Masks::Files[FILE_H];

        U64 NotRank1 = ~Masks::Ranks[RANK_1];
        U64 NotRank2 = ~Masks::Ranks[RANK_2];
        U64 NotRank7 = ~Masks::Ranks[RANK_7];
        U64 NotRank8 = ~Masks::Ranks[RANK_8];

        for (int file = FILE_A; file <= FILE_H; file++) {
            U64 fileMask = Masks::Files[file];
            U64 leftFileMask = file > FILE_A ? Masks::Files[file - 1] : 0ULL;
            U64 rightFileMask = file < FILE_H ? Masks::Files[file + 1] : 0ULL;

            for (int rank = RANK_1; rank <= RANK_8; rank++) {
                int square = GetSquare(file, rank);

                Masks::StackedPawn[square] = fileMask;
                Masks::IsolatedPawn[square] = leftFileMask | rightFileMask;

                Masks::ClosePawnShieldWhite[square] = ((Masks::Square(square + 7) & NotFileH) | Masks::Square(square + 8) | (Masks::Square(square + 9) & NotFileA)) & NotRank1;
                Masks::ClosePawnShieldBlack[square] = ((Masks::Square(square - 7) & NotFileA) | Masks::Square(square - 8) | (Masks::Square(square - 9) & NotFileH)) & NotRank8;

                Masks::FarPawnShieldWhite[square] = ((Masks::Square(square + 15) & NotFileH) | Masks::Square(square + 16) | (Masks::Square(square + 17) & NotFileA)) & (NotRank1 | NotRank2);
                Masks::FarPawnShieldBlack[square] = ((Masks::Square(square - 15) & NotFileA) | Masks::Square(square - 16) | (Masks::Square(square - 17) & NotFileH)) & (NotRank7 | NotRank8);
            
                U64 squaresBelow = 0ULL;
                U64 squaresAbove = 0ULL;

                int rb = rank; // rank below
                int ra = rank; // rank above

                while (rb >= RANK_1) {
                    squaresBelow |= Masks::Ranks[rb];
                    rb--;
                }

                while (ra <= RANK_8) {
                    squaresAbove |= Masks::Ranks[ra];
                    ra++;
                }

                U64 files = fileMask | leftFileMask | rightFileMask;

                Masks::PassedPawnWhite[square] = files & ~squaresBelow;
                Masks::PassedPawnBlack[square] = files & ~squaresAbove;
            }
        }
    }

    void Init() {
        InitPieceSquareTables();
        InitMasks();
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

        return (board.side == WHITE ? eval : -eval) + Params::TempoBonus;
    }
}