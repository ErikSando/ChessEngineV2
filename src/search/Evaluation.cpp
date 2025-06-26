#include <string.h>

#include "Evaluation.h"
#include "EvalParams.h"
#include "Utils.h"

namespace ErikEngine {
    using namespace EvalParams;

    namespace Evaluation {
        int PieceTablesMg[12][64];
        int PieceTablesEg[12][64];

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

        const int* MgPieceTables[6] = {
            PawnTableMg, KnightTableMg, BishopTableMg, RookTableMg, QueenTableMg, KingTableMg
        };

        const int* EgPieceTables[6] = {
            PawnTableEg, KnightTableEg, BishopTableEg, RookTableEg, QueenTableEg, KingTableEg
        };

        void InitPieceSquareTables() {
            for (int piece = WP; piece <= WK; piece++) {
                for (int square = 0; square < 64; square++) {
                    PieceTablesMg[piece][square] = PieceValuesMg[piece] + MgPieceTables[piece][MirrorSquare(square)];
                    PieceTablesEg[piece][square] = PieceValuesEg[piece] + EgPieceTables[piece][MirrorSquare(square)];

                    PieceTablesMg[piece + 6][square] = PieceValuesMg[piece] + MgPieceTables[piece][square];
                    PieceTablesEg[piece + 6][square] = PieceValuesEg[piece] + EgPieceTables[piece][square];
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

                    Masks::StackedPawn[square] = fileMask & ~(1ULL << square);
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

            if (!rooks[WHITE] && !rooks[BLACK] && !queens[WHITE] && !queens[BLACK]) {
                if (!bishops[WHITE] && !bishops[BLACK]) {
                    if (knights[WHITE] < 3 && knights[BLACK] < 3) return true;
                }
                else if (!knights[WHITE] && !knights[BLACK]) {
                    if (abs(bishops[WHITE] - bishops[BLACK]) < 2) return true;
                }
                else if ((knights[WHITE] < 3 && !bishops[WHITE]) || (bishops[BLACK] == 1 && !knights[BLACK])) {
                    if ((knights[BLACK] < 3 && !bishops[BLACK]) || (bishops[BLACK] == 1 && !knights[BLACK])) return true;
                }
            }
            else if (!queens[WHITE] && !queens[BLACK]) {
                if (rooks[WHITE] == 1 && rooks[BLACK] == 1) {
                    if ((knights[WHITE] + bishops[WHITE] < 2) && (knights[BLACK] + bishops[BLACK] < 2)) return true;
                }
                else if (rooks[WHITE] == 1 && !rooks[BLACK]) {
                    if ((knights[WHITE] + bishops[WHITE] == 0) && ((knights[BLACK] + bishops[BLACK] == 1) || (knights[BLACK] + bishops[BLACK] == 2))) return true;
                }
                else if (rooks[BLACK] == 1 && !rooks[WHITE]) {
                    if ((knights[BLACK] + bishops[BLACK] == 0) && ((knights[WHITE] + bishops[WHITE] == 1) || (knights[WHITE] + bishops[WHITE] == 2))) return true;
                }
            }

            return false;
        }

        inline int ManhattenDistance(int square1, int square2) {
            return std::abs(GetFile(square2) - GetFile(square1)) + std::abs(GetRank(square2) - GetRank(square1));
        }

        int Evaluate(Board& board) {
            if (MaterialDraw(board)) return 0;

            int mgScore = 0;
            int egScore = 0;
            int score = 0; // score that doesnt depend on game phase
            int mgPhase = 0;

            int pawns = 0;
            int bishops[2] = { 0, 0 };

            int kingSquare[2];

            int piece = WP;
            U64 bitboard = board.bitboards[piece];

            while (bitboard) {
                int square = PopFirstBit(bitboard);

                pawns++;

                mgScore += PieceTablesMg[piece][square];
                egScore += PieceTablesEg[piece][square];

                mgPhase += PhaseInc[piece];

                if (Masks::StackedPawn[square] & board.bitboards[WP]) score -= StackedPawnPenalty;
                if (!(Masks::IsolatedPawn[square] & board.bitboards[WP])) score -= IsolatedPawnPenalty;
                if (!(Masks::PassedPawnWhite[square] & board.bitboards[BP])) {
                    score += PassedPawnValue[GetRank(square)];
                }

                if (Attacks::PawnCaptures[BLACK][square] & board.bitboards[WP]) score += DefendedPawnBonus;
            }

            piece = BP;
            bitboard = board.bitboards[piece];

            while (bitboard) {
                int square = PopFirstBit(bitboard);

                pawns++;

                mgScore -= PieceTablesMg[piece][square];
                egScore -= PieceTablesEg[piece][square];

                mgPhase += PhaseInc[piece];

                if (Masks::StackedPawn[square] & board.bitboards[BP]) score += StackedPawnPenalty;
                if (!(Masks::IsolatedPawn[square] & board.bitboards[BP])) score += IsolatedPawnPenalty;
                if (!(Masks::PassedPawnBlack[square] & board.bitboards[WP])) {
                    score -= PassedPawnValue[RANK_8 - GetRank(square)];
                }

                if (Attacks::PawnCaptures[WHITE][square] & board.bitboards[BP]) score -= DefendedPawnBonus;
            }

            // std::cout << "After pawns:\n";
            // std::cout << mgScore << ", " << egScore << ", " << score << "\n";

            piece = WN;
            bitboard = board.bitboards[piece];

            while (bitboard) {
                int square = PopFirstBit(bitboard);

                mgScore += PieceTablesMg[piece][square];
                egScore += PieceTablesEg[piece][square];

                mgPhase += PhaseInc[piece];

                U64 attacks = Attacks::KnightAttacks[square] & ~board.occupancy[WHITE];
                int mobility = CountBits(attacks);

                mgScore += mobility * MobilityValueMg[piece];
                egScore += mobility * MobilityValueEg[piece];
            }

            piece = BN;
            bitboard = board.bitboards[piece];

            while (bitboard) {
                int square = PopFirstBit(bitboard);

                mgScore -= PieceTablesMg[piece][square];
                egScore -= PieceTablesEg[piece][square];

                mgPhase += PhaseInc[piece];

                U64 attacks = Attacks::KnightAttacks[square] & ~board.occupancy[BLACK];
                int mobility = CountBits(attacks);

                mgScore -= mobility * MobilityValueMg[piece];
                egScore -= mobility * MobilityValueEg[piece];
            }

            // std::cout << "After knights:\n";
            // std::cout << mgScore << ", " << egScore << ", " << score << "\n";

            piece = WB;
            bitboard = board.bitboards[piece];

            while (bitboard) {
                int square = PopFirstBit(bitboard);

                bishops[WHITE]++;

                mgScore += PieceTablesMg[piece][square];
                egScore += PieceTablesEg[piece][square];

                mgPhase += PhaseInc[piece];

                U64 attacks = Attacks::GetBishopAttacks(square, board.occupancy[BOTH]) & ~board.occupancy[WHITE];
                int mobility = CountBits(attacks);

                mgScore += mobility * MobilityValueMg[piece];
                egScore += mobility * MobilityValueEg[piece];
            }

            piece = BB;
            bitboard = board.bitboards[piece];

            while (bitboard) {
                int square = PopFirstBit(bitboard);

                bishops[BLACK]++;

                mgScore -= PieceTablesMg[piece][square];
                egScore -= PieceTablesEg[piece][square];

                mgPhase += PhaseInc[piece];

                U64 attacks = Attacks::GetBishopAttacks(square, board.occupancy[BOTH]) & ~board.occupancy[BLACK];
                int mobility = CountBits(attacks);

                mgScore -= mobility * MobilityValueMg[piece];
                egScore -= mobility * MobilityValueEg[piece];
            }

            // std::cout << "After bishops:\n";
            // std::cout << mgScore << ", " << egScore << ", " << score << "\n";

            piece = WR;
            bitboard = board.bitboards[piece];

            while (bitboard) {
                int square = PopFirstBit(bitboard);

                mgScore += PieceTablesMg[piece][square];
                egScore += PieceTablesEg[piece][square];

                mgPhase += PhaseInc[piece];

                U64 attacks = Attacks::GetRookAttacks(square, board.occupancy[BOTH]) & ~board.occupancy[WHITE];
                int mobility = CountBits(attacks);

                mgScore += mobility * MobilityValueMg[piece];
                egScore += mobility * MobilityValueEg[piece];

                if (!(Masks::StackedPawn[square] & (board.bitboards[WP] | board.bitboards[BP]))) {
                    score += RookOpenFileBonus;
                }
                else if (!(Masks::StackedPawn[square] & board.bitboards[WP])) {
                    score += RookSemiOpenFileBonus;
                }
            }

            piece = BR;
            bitboard = board.bitboards[piece];

            while (bitboard) {
                int square = PopFirstBit(bitboard);

                mgScore -= PieceTablesMg[piece][square];
                egScore -= PieceTablesEg[piece][square];

                mgPhase += PhaseInc[piece];

                U64 attacks = Attacks::GetRookAttacks(square, board.occupancy[BOTH]) & ~board.occupancy[BLACK];
                int mobility = CountBits(attacks);

                mgScore -= mobility * MobilityValueMg[piece];
                egScore -= mobility * MobilityValueEg[piece];

                if (!(Masks::StackedPawn[square] & (board.bitboards[WP] | board.bitboards[WP]))) {
                    score -= RookOpenFileBonus;
                }
                else if (!(Masks::StackedPawn[square] & board.bitboards[BP])) {
                    score -= RookSemiOpenFileBonus;
                }
            }

            // std::cout << "After rooks:\n";
            // std::cout << mgScore << ", " << egScore << ", " << score << "\n";

            piece = WQ;
            bitboard = board.bitboards[piece];

            while (bitboard) {
                int square = PopFirstBit(bitboard);

                mgScore += PieceTablesMg[piece][square];
                egScore += PieceTablesEg[piece][square];

                mgPhase += PhaseInc[piece];

                U64 attacks = Attacks::GetQueenAttacks(square, board.occupancy[BOTH]) & ~board.occupancy[WHITE];
                int mobility = CountBits(attacks);

                mgScore += mobility * MobilityValueMg[piece];
                egScore += mobility * MobilityValueEg[piece];

                if (!(Masks::StackedPawn[square] & (board.bitboards[WP] | board.bitboards[BP]))) {
                    score += QueenOpenFileBonus;
                }
                else if (!(Masks::StackedPawn[square] & board.bitboards[WP])) {
                    score += QueenSemiOpenFileBonus;
                }
            }

            piece = BQ;
            bitboard = board.bitboards[piece];

            while (bitboard) {
                int square = PopFirstBit(bitboard);

                mgScore -= PieceTablesMg[piece][square];
                egScore -= PieceTablesEg[piece][square];

                mgPhase += PhaseInc[piece];

                U64 attacks = Attacks::GetQueenAttacks(square, board.occupancy[BOTH]) & ~board.occupancy[BLACK];
                int mobility = CountBits(attacks);

                mgScore -= mobility * MobilityValueMg[piece];
                egScore -= mobility * MobilityValueEg[piece];

                if (!(Masks::StackedPawn[square] & (board.bitboards[WP] | board.bitboards[BP]))) {
                    score -= QueenOpenFileBonus;
                }
                else if (!(Masks::StackedPawn[square] & board.bitboards[BP])) {
                    score -= QueenSemiOpenFileBonus;
                }
            }

            // std::cout << "After queens:\n";
            // std::cout << mgScore << ", " << egScore << ", " << score << "\n";

            piece = WK;
            bitboard = board.bitboards[piece];

            while (bitboard) {
                int square = PopFirstBit(bitboard);

                mgScore += PieceTablesMg[piece][square];
                egScore += PieceTablesEg[piece][square];

                kingSquare[WHITE] = square;

                int virtualAttacks = CountBits(Attacks::GetQueenAttacks(square, board.occupancy[BOTH]) & ~board.occupancy[BOTH]);
                int closePawnShield = CountBits(Masks::ClosePawnShieldWhite[square] & board.bitboards[WP]);
                int farPawnShield = CountBits(Masks::FarPawnShieldWhite[square] & board.bitboards[WP]);

                mgScore -= virtualAttacks * KingVirtualAttackPenalty;
                mgScore += closePawnShield * ClosePawnShieldBonus;
                mgScore += farPawnShield * FarPawnShieldBonus;
                
                if (!(Masks::StackedPawn[square] & board.bitboards[WP])) mgScore -= KingOpenFilePenalty;
            }

            piece = BK;
            bitboard = board.bitboards[piece];

            while (bitboard) {
                int square = PopFirstBit(bitboard);

                mgScore -= PieceTablesMg[piece][square];
                egScore -= PieceTablesEg[piece][square];

                kingSquare[BLACK] = square;

                int virtualAttacks = CountBits(Attacks::GetQueenAttacks(square, board.occupancy[BOTH]) & ~board.occupancy[BOTH]);
                int closePawnShield = CountBits(Masks::ClosePawnShieldBlack[square] & board.bitboards[BP]);
                int farPawnShield = CountBits(Masks::FarPawnShieldBlack[square] & board.bitboards[BP]);

                mgScore += virtualAttacks * KingVirtualAttackPenalty;
                mgScore -= closePawnShield * ClosePawnShieldBonus;
                mgScore -= farPawnShield * FarPawnShieldBonus;
                
                if (!(Masks::StackedPawn[square] & board.bitboards[BP])) mgScore += KingOpenFilePenalty;
            }

            // std::cout << "After kings:\n";
            // std::cout << mgScore << ", " << egScore << ", " << score << "\n";

            if (pawns == 0) {
                if (egScore > 0) {
                    score += 4.7 * CentreManhattenDistance[kingSquare[BLACK]];
                    score += 1.6 * (14 - ManhattenDistance(kingSquare[WHITE], kingSquare[BLACK]));
                }
                else {
                    score -= 4.7 * CentreManhattenDistance[kingSquare[BLACK]];
                    score -= 1.6 * (14 - ManhattenDistance(kingSquare[WHITE], kingSquare[BLACK]));
                }
            }

            // std::cout << "After mop up:\n";
            // std::cout << mgScore << ", " << egScore << ", " << score << "\n";

            if (bishops[WHITE] > 1) score += BishopPairBonus;
            if (bishops[BLACK] > 1) score -= BishopPairBonus;

            // std::cout << "After bishop pair:\n";
            // std::cout << mgScore << ", " << egScore << ", " << score << "\n";

            if (mgPhase > 24) mgPhase = 24;
            int egPhase = 24 - mgPhase;

            int eval = score + (mgScore * mgPhase + egScore * egPhase) / 24;

            // std::cout << "Evaluation: " << eval << "\n";

            return (board.side == WHITE ? eval : -eval) + TempoBonus;
        }
    }
}