#pragma once

#include "Attacks.h"
#include "Data.h"
#include "Globals.h"

// MakeMove.h and TakeMove.h included at the end

namespace ErikEngine {
    struct UndoInfo {
        U64 hashKey;
        int move;
        int castlingPerms;
        int fiftyMoveCount;
        int enPassant;
    };

    enum class MoveContext {
        Search, Perft
    };

    class Board {
        public:

        Board();

        void Clear();
        void Print() const;
        void ParseFEN(const char* fen);

        const char* GenerateFEN() const;

        void AssertValid() const;
        bool IsValid() const;

        template<MoveContext moveContext = MoveContext::Search, bool pseudoLegal = true>
        bool MakeMove(const int move);

        // bool MakeMove(const int move, bool pseudoLegal = true);

        // inline bool MakeMovePL(const int move) {
        //     return MakeMove(move, true);
        // }

        template<MoveContext moveContext = MoveContext::Search>
        void TakeMove();

        void MakeNullMove();
        void TakeNullMove();

        U64 bitboards[12];
        U64 occupancy[3];

        U64 hashKey;

        UndoInfo history[MAX_GAME_MOVES];

        int ply;
        int fiftyMoveCount;
        int enPassant;
        int side;
        int castlingPerms;

        int fullMoveCount; // used to generate an FEN string

        int bigPieces[2];

        inline U64 GetAttackedSquares(const int attacker_side) const {
            U64 attacked = 0ULL;

            int attackerPawns = attacker_side * 6;
            int kingPiece = (attackerPawns ^ 6) + 5;

            U64 blockers = occupancy[BOTH];
            ClearBit(blockers, FirstBitIndex(bitboards[kingPiece]));

            U64 pawns = bitboards[attackerPawns];

            while (pawns) {
                int square = PopFirstBit(pawns);
                attacked |= Attacks::PawnCaptures[attacker_side][square];
            }

            for (int pieceType = N; pieceType <= K; pieceType++) {
                U64 pieces = bitboards[attackerPawns + pieceType];

                while (pieces) {
                    int square = PopFirstBit(pieces);
                    attacked |= Attacks::GetAttackMask(pieceType, square, blockers);
                }
            }

            return attacked;
        }

        inline U64 GetAttackedSquares() const {
            return GetAttackedSquares(side ^ 1);
        }

        //bool IsSquareAttacked(const int square, const int attacker_side) const;

        inline bool IsSquareAttacked(const int square, const int attacker_side) const {
            int piece = attacker_side * 6;
        
            U64 bishopAttacks = Attacks::GetBishopAttacks(square, occupancy[BOTH]);
            U64 rookAttacks = Attacks::GetRookAttacks(square, occupancy[BOTH]);

            if ((bishopAttacks | rookAttacks) & bitboards[piece + 4]) return true;
            if (bishopAttacks & bitboards[piece + 2]) return true;
            if (rookAttacks & bitboards[piece + 3]) return true;
        
            if (Attacks::KnightAttacks[square] & bitboards[piece + 1]) return true;
            if (Attacks::PawnCaptures[attacker_side ^ 1][square] & bitboards[piece]) return true;
            if (Attacks::KingAttacks[square] & bitboards[piece + 5]) return true;
        
            return false;
        }

        inline bool IsSquareAttacked(const int square) const {
            return IsSquareAttacked(square, side ^ 1);
        }
    };
}

#include "MoveGen.h"
#include "MakeMove.h"
#include "TakeMove.h"