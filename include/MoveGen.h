#pragma once

#include "Attacks.h"
#include "Globals.h"
#include "Move.h"
#include "MoveScoring.h"

namespace ErikEngine {
    constexpr size_t MAX_MOVES = 256;

    struct Move {
        int move;
        int score;
    };

    struct MoveList {
        inline Move& operator[](size_t index) { return m_moves[index]; }

        inline size_t size() { return m_length; }
        inline void clear() { m_length = 0; }

        // inline Move& at(size_t index) { return m_moves[index]; }

        // these could be references, but i think readability is better if
        // these are read only and the move list is edited with [index].move/score
        inline int move_at(size_t index) { return m_moves[index].move; }
        inline int score_at(size_t index) { return m_moves[index].score; }

        inline void add(int score, int move) {
            // I realise that a buffer overflow is certainly possible by setting bits on the piece
            // bitboards in the same bits for different bitboards to end up with a lot of moves generated
            m_moves[m_length].move = move;
            m_moves[m_length].score = score;
            m_length++;
        }

        inline Move* begin() { return &m_moves[0]; }
        inline Move* end() { return &m_moves[0] + m_length; }

        inline const Move* cbegin() const { return &m_moves[0]; }
        inline const Move* cend() const { return &m_moves[0] + m_length; }

        private:

        Move m_moves[MAX_MOVES];
        size_t m_length;
    };

    enum class MoveType {
        Legal, PseudoLegal
    };

    namespace MoveGen {
        constexpr U64 RANK_1_MASK = 0x00000000000000FFULL;

        constexpr U64 WKC_EMPTY_MASK = (1ULL << f1) | (1ULL << g1);
        constexpr U64 WKC_SAFE_MASK = (1ULL << e1) | (1ULL << f1) | (1ULL << g1);
        constexpr U64 WQC_EMPTY_MASK = (1ULL << b1) | (1ULL << c1) | (1ULL << d1);
        constexpr U64 WQC_SAFE_MASK = (1ULL << e1) | (1ULL << c1) | (1ULL << d1);
        constexpr U64 BKC_EMPTY_MASK = (1ULL << f8) | (1ULL << g8);
        constexpr U64 BKC_SAFE_MASK = (1ULL << e8) | (1ULL << f8) | (1ULL << g8);
        constexpr U64 BQC_EMPTY_MASK = (1ULL << d8) | (1ULL << c8) | (1ULL << b8);
        constexpr U64 BQC_SAFE_MASK = (1ULL << e8) | (1ULL << c8) | (1ULL << d8);

        // MOVE REPRESENTATION
        // 0              0            0              0           0000      0000      0000   000000  000000  (28 bits)
        // is en passant  is castling  is pawn start  is capture  promoted  captured  piece  to      from
        inline int EncodeMove(int fromSquare, int toSquare, int moved, int captured, int promoted, int flag) {
            return (fromSquare) | (toSquare << 6) | (moved << 12) | (captured << 16) | (promoted << 20) | flag;
        }

        inline int GetScore(const Board& board, int move, int piece, int toSquare) {
            int score = 0;

            if (move == MoveScoring::KillerMoves[0][board.ply]) {
                score = MoveScoring::KillerScore0;
            }
            else if (move == MoveScoring::KillerMoves[1][board.ply]) {
                score = MoveScoring::KillerScore1;
            }
            else {
                // int lastMove = board.history[board.ply - 1].move;
                // if (move == CounterMoves[GetFromSquare(lastMove)][GetToSquare(lastMove)]) score = CounterMoveScore;
                // else score = HistoryMoves[piece][toSquare];
                score = MoveScoring::HistoryMoves[piece][toSquare];
            }

            return score;
        }

        template<MoveType moveType = MoveType::PseudoLegal>
        inline void GenerateMoves(Board& board, MoveList& list) {
            list.clear();

            int side = board.side;
            int enemy = side ^ 1;
            int direction = enemy ? 8 : -8;
            int promotionRank = enemy ? RANK_8 : RANK_1;
            int captureStart = enemy ? BP : WP;
            int pieceType = P;
            int piece = enemy ? WP : BP;

            if constexpr (moveType == MoveType::PseudoLegal) {
                // castling
                if (enemy) { // equivalent to if side == WHITE
                    if (board.castlingPerms & WKC) {
                        if (!(board.occupancy[BOTH] & WKC_EMPTY_MASK) &&
                            !board.IsSquareAttacked(e1) &&
                            !board.IsSquareAttacked(f1) &&
                            !board.IsSquareAttacked(g1)
                        ) {
                            int move = EncodeMove(e1, g1, WK, 0, 0, CASTLING_FLAG);
                            int score = GetScore(board, move, WK, g1);
                            list.add(score, move);
                        }
                    }

                    if (board.castlingPerms & WQC) {
                        if (!(board.occupancy[BOTH] & WQC_EMPTY_MASK) &&
                            !board.IsSquareAttacked(e1) &&
                            !board.IsSquareAttacked(d1) &&
                            !board.IsSquareAttacked(c1)
                        ) {
                            int move = EncodeMove(e1, c1, WK, 0, 0, CASTLING_FLAG);
                            int score = GetScore(board, move, WK, c1);
                            list.add(score, move);
                        }
                    }
                }
                else {
                    if (board.castlingPerms & BKC) {
                        if (!(board.occupancy[BOTH] & BKC_EMPTY_MASK) &&
                            !board.IsSquareAttacked(e8) &&
                            !board.IsSquareAttacked(f8) &&
                            !board.IsSquareAttacked(g8)
                        ) {
                            int move = EncodeMove(e8, g8, BK, 0, 0, CASTLING_FLAG);
                            int score = GetScore(board, move, BK, g8);
                            list.add(score, move);
                        }
                    }

                    if (board.castlingPerms & BQC) {
                        if (!(board.occupancy[BOTH] & BQC_EMPTY_MASK) &&
                            !board.IsSquareAttacked(e8) &&
                            !board.IsSquareAttacked(d8) &&
                            !board.IsSquareAttacked(c8)
                        ) {
                            int move = EncodeMove(e8, c8, BK, 0, 0, CASTLING_FLAG);
                            int score = GetScore(board, move, BK, c8);
                            list.add(score, move);
                        }
                    }
                }

                U64 bitboard = board.bitboards[piece];

                if (board.enPassant != NO_SQUARE) {
                    U64 fromSquares = Attacks::PawnCaptures[enemy][board.enPassant];

                    while (fromSquares) {
                        int fromSquare = PopFirstBit(fromSquares);

                        if (IsBitSet(bitboard, fromSquare)) {
                            list.add(MoveScoring::MvvLvaScore[P][P], EncodeMove(fromSquare, board.enPassant, piece, 0, 0, ENPASSANT_FLAG));
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
                            list.add(MoveScoring::PromotionScore[Q], EncodeMove(fromSquare, toSquare, piece, 0, piece + 4, 0));
                            list.add(MoveScoring::PromotionScore[R], EncodeMove(fromSquare, toSquare, piece, 0, piece + 3, 0));
                            list.add(MoveScoring::PromotionScore[B], EncodeMove(fromSquare, toSquare, piece, 0, piece + 2, 0));
                            list.add(MoveScoring::PromotionScore[N], EncodeMove(fromSquare, toSquare, piece, 0, piece + 1, 0));
                            continue;
                        }

                        int flag = 0;

                        if (toSquare - fromSquare == direction * 2) {
                            if (IsBitSet(board.occupancy[BOTH], fromSquare + direction)) continue;
                            flag = PAWNSTART_FLAG;
                        }

                        int move = EncodeMove(fromSquare, toSquare, piece, 0, 0, flag);
                        int score = GetScore(board, move, piece, toSquare);

                        list.add(score, move);
                    }

                    while (captures) {
                        int toSquare = PopFirstBit(captures);
                        int captured = captureStart;

                        for (; captured < captureStart + 6; captured++) {
                            if (IsBitSet(board.bitboards[captured], toSquare)) break;
                        }

                        int mvvlva = MoveScoring::MvvLvaScore[captured][P];

                        if (GetRank(toSquare) == promotionRank) {
                            list.add(mvvlva + MoveScoring::PromotionScore[Q], EncodeMove(fromSquare, toSquare, piece, captured, piece + 4, CAPTURE_FLAG));
                            list.add(mvvlva + MoveScoring::PromotionScore[R], EncodeMove(fromSquare, toSquare, piece, captured, piece + 3, CAPTURE_FLAG));
                            list.add(mvvlva + MoveScoring::PromotionScore[B], EncodeMove(fromSquare, toSquare, piece, captured, piece + 2, CAPTURE_FLAG));
                            list.add(mvvlva + MoveScoring::PromotionScore[N], EncodeMove(fromSquare, toSquare, piece, captured, piece + 1, CAPTURE_FLAG));
                            continue;
                        }

                        list.add(mvvlva, EncodeMove(fromSquare, toSquare, piece, captured, 0, CAPTURE_FLAG));
                    }
                }

                piece++;
                pieceType++;

                for (; pieceType <= K; piece++, pieceType++) {
                    bitboard = board.bitboards[piece];

                    while (bitboard) {
                        int fromSquare = PopFirstBit(bitboard);
                        U64 attacks = Attacks::GetAttackMask(pieceType, fromSquare, board.occupancy[BOTH]) & ~board.occupancy[side];

                        while (attacks) {
                            int toSquare = PopFirstBit(attacks);
                            int score;
                            int move;
                            int captured = captureStart;

                            if (IsBitSet(board.occupancy[enemy], toSquare)) {
                                for (; captured < captureStart + 6; captured++) {
                                    if (IsBitSet(board.bitboards[captured], toSquare)) break;
                                }

                                move = EncodeMove(fromSquare, toSquare, piece, captured, 0, CAPTURE_FLAG);
                                score = MoveScoring::MvvLvaScore[captured][pieceType];
                            }
                            else {
                                move = EncodeMove(fromSquare, toSquare, piece, captured, 0, 0);
                                score = GetScore(board, move, piece, toSquare);
                            }

                            list.add(score, move); 
                        }
                    }
                }
            }
            else {
                U64 attacked = board.GetAttackedSquares();
                U64 captureMask = 0xFFFFFFFFFFFFFFFFULL;
                U64 blockMask = 0xFFFFFFFFFFFFFFFFULL;

                int kingPiece = piece + 5;
                U64 bitboard = board.bitboards[kingPiece];
                
                int kingSquare = PopFirstBit(bitboard);
                U64 attacks = Attacks::KingAttacks[kingSquare] & ~board.occupancy[side] & ~attacked;

                int enemyPawn = piece ^ 6;

                U64 attackers = (Attacks::PawnCaptures[side][kingSquare] & board.bitboards[enemyPawn]) |
                                (Attacks::KnightAttacks[kingSquare] & board.bitboards[enemyPawn + N]) |
                                (Attacks::GetBishopAttacks(kingSquare, board.occupancy[BOTH]) & board.bitboards[enemyPawn + B]) |
                                (Attacks::GetRookAttacks(kingSquare, board.occupancy[BOTH]) & board.bitboards[enemyPawn + R]) |
                                (Attacks::GetQueenAttacks(kingSquare, board.occupancy[BOTH]) & board.bitboards[enemyPawn + Q]);

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

                    list.add(0, EncodeMove(kingSquare, toSquare, kingPiece, captured, 0, flag));
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
                        if (!(board.occupancy[BOTH] & WKC_EMPTY_MASK) &&
                            !(attacked & WKC_SAFE_MASK)
                        ) {
                            int move = EncodeMove(e1, g1, WK, 0, 0, CASTLING_FLAG);
                            list.add(0, move);
                        }
                    }

                    if (board.castlingPerms & WQC) {
                        if (!(board.occupancy[BOTH] & WQC_EMPTY_MASK) &&
                            !(attacked & WQC_SAFE_MASK)
                        ) {
                            int move = EncodeMove(e1, c1, WK, 0, 0, CASTLING_FLAG);
                            list.add(0, move);
                        }
                    }
                }
                else {
                    if (board.castlingPerms & BKC) {
                        if (!(board.occupancy[BOTH] & BKC_EMPTY_MASK) &&
                            !(attacked & BKC_SAFE_MASK)
                        ) {
                            int move = EncodeMove(e8, g8, BK, 0, 0, CASTLING_FLAG);
                            list.add(0, move);
                        }
                    }

                    if (board.castlingPerms & BQC) {
                        if (!(board.occupancy[BOTH] & BQC_EMPTY_MASK) &&
                            !(attacked & BQC_SAFE_MASK)
                        ) {
                            int move = EncodeMove(e8, c8, BK, 0, 0, CASTLING_FLAG);
                            list.add(0, move);
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

                        list.add(0, EncodeMove(fromSquare, board.enPassant, piece, 0, 0, ENPASSANT_FLAG));
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
                            list.add(0, EncodeMove(fromSquare, toSquare, piece, 0, piece + 4, 0));
                            list.add(0, EncodeMove(fromSquare, toSquare, piece, 0, piece + 3, 0));
                            list.add(0, EncodeMove(fromSquare, toSquare, piece, 0, piece + 2, 0));
                            list.add(0, EncodeMove(fromSquare, toSquare, piece, 0, piece + 1, 0));
                            continue;
                        }

                        int flag = 0;

                        if (toSquare - fromSquare == direction * 2) {
                            if (IsBitSet(board.occupancy[BOTH], fromSquare + direction)) continue;
                            flag = PAWNSTART_FLAG;
                        }

                        list.add(0, EncodeMove(fromSquare, toSquare, piece, 0, 0, flag));
                    }

                    while (captures) {
                        int toSquare = PopFirstBit(captures);
                        int captured = captureStart;

                        for (; captured < captureStart + 6; captured++) {
                            if (IsBitSet(board.bitboards[captured], toSquare)) break;
                        }

                        if (GetRank(toSquare) == promotionRank) {
                            list.add(0, EncodeMove(fromSquare, toSquare, piece, captured, piece + 4, CAPTURE_FLAG));
                            list.add(0, EncodeMove(fromSquare, toSquare, piece, captured, piece + 3, CAPTURE_FLAG));
                            list.add(0, EncodeMove(fromSquare, toSquare, piece, captured, piece + 2, CAPTURE_FLAG));
                            list.add(0, EncodeMove(fromSquare, toSquare, piece, captured, piece + 1, CAPTURE_FLAG));
                            continue;
                        }

                        list.add(0, EncodeMove(fromSquare, toSquare, piece, captured, 0, CAPTURE_FLAG));
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

                        U64 attacks = Attacks::GetAttackMask(pieceType, fromSquare, board.occupancy[BOTH]) & ~board.occupancy[side] & (blockMask | captureMask) & ray;
                    
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

                            list.add(0, EncodeMove(fromSquare, toSquare, piece, captured, 0, flag));
                        }
                    }
                }
            }
        }

        template<MoveType moveType = MoveType::PseudoLegal>
        inline void GenerateCaptures(Board& board, MoveList& list) {
            list.clear();

            int side = board.side;
            int enemy = side ^ 1;
            int direction = enemy ? 8 : -8;
            int promotionRank = enemy ? RANK_8 : RANK_1;
            int captureStart = enemy ? BP : WP;
            int pieceType = P;
            int piece = enemy ? WP : BP;

            if constexpr (moveType == MoveType::PseudoLegal) {
                U64 bitboard = board.bitboards[piece];

                while (bitboard) {
                    int fromSquare = PopFirstBit(bitboard);
                    U64 captures = Attacks::PawnCaptures[side][fromSquare] & board.occupancy[enemy];

                    if (board.enPassant != NO_SQUARE) {
                        if (IsBitSet(Attacks::PawnCaptures[side][fromSquare], board.enPassant)) {
                            list.add(MoveScoring::MvvLvaScore[P][P], EncodeMove(fromSquare, board.enPassant, piece, 0, 0, ENPASSANT_FLAG));
                        }
                    }

                    while (captures) {
                        int toSquare = PopFirstBit(captures);
                        int captured = captureStart;

                        for (; captured < captureStart + 6; captured++) {
                            if (IsBitSet(board.bitboards[captured], toSquare)) break;
                        }

                        int mvvlva = MoveScoring::MvvLvaScore[captured][P];

                        if (GetRank(toSquare) == promotionRank) {
                            list.add(mvvlva + MoveScoring::PromotionScore[Q], EncodeMove(fromSquare, toSquare, piece, captured, piece + 4, CAPTURE_FLAG));
                            list.add(mvvlva + MoveScoring::PromotionScore[R], EncodeMove(fromSquare, toSquare, piece, captured, piece + 3, CAPTURE_FLAG));
                            list.add(mvvlva + MoveScoring::PromotionScore[B], EncodeMove(fromSquare, toSquare, piece, captured, piece + 2, CAPTURE_FLAG));
                            list.add(mvvlva + MoveScoring::PromotionScore[N], EncodeMove(fromSquare, toSquare, piece, captured, piece + 1, CAPTURE_FLAG));
                            continue;
                        }

                        list.add(mvvlva, EncodeMove(fromSquare, toSquare, piece, captured, 0, CAPTURE_FLAG));
                    }
                }

                piece++;
                pieceType++;

                for (; pieceType <= K; piece++, pieceType++) {
                    bitboard = board.bitboards[piece];

                    while (bitboard) {
                        int fromSquare = PopFirstBit(bitboard);
                        U64 attacks = Attacks::GetAttackMask(pieceType, fromSquare, board.occupancy[BOTH]) & board.occupancy[enemy];

                        while (attacks) {
                            int toSquare = PopFirstBit(attacks);
                            int captured = captureStart;

                            for (; captured < captureStart + 6; captured++) {
                                if (IsBitSet(board.bitboards[captured], toSquare)) break;
                            }

                            list.add(
                                MoveScoring::MvvLvaScore[captured][pieceType],
                                EncodeMove(fromSquare, toSquare, piece, captured, 0, CAPTURE_FLAG)
                            );
                        }
                    }
                }
            }
            else {
                U64 attacked = board.GetAttackedSquares();
                U64 captureMask = 0xFFFFFFFFFFFFFFFFULL;
                U64 blockMask = 0xFFFFFFFFFFFFFFFFULL;

                int kingPiece = piece + 5;
                U64 bitboard = board.bitboards[kingPiece];

                int kingSquare = PopFirstBit(bitboard);
                U64 attacks = Attacks::KingAttacks[kingSquare] & ~board.occupancy[side] & ~attacked & board.occupancy[enemy];

                int enemyPawn = piece ^ 6;

                U64 attackers = (Attacks::PawnCaptures[side][kingSquare] & board.bitboards[enemyPawn]) |
                                (Attacks::KnightAttacks[kingSquare] & board.bitboards[enemyPawn + N]) |
                                (Attacks::GetBishopAttacks(kingSquare, board.occupancy[BOTH]) & board.bitboards[enemyPawn + B]) |
                                (Attacks::GetRookAttacks(kingSquare, board.occupancy[BOTH]) & board.bitboards[enemyPawn + R]) |
                                (Attacks::GetQueenAttacks(kingSquare, board.occupancy[BOTH]) & board.bitboards[enemyPawn + Q]);

                int numAttackers = CountBits(attackers);

                while (attacks) {
                    int toSquare = PopFirstBit(attacks);
                    // if (!IsBitSet(board.occupancy[enemy], toSquare)) continue;

                    int captured = captureStart;

                    for (; captured < captureStart + 6; captured++) {
                        if (IsBitSet(board.bitboards[captured], toSquare)) break;
                    }

                    list.add(0, EncodeMove(kingSquare, toSquare, kingPiece, captured, 0, CAPTURE_FLAG));
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

                        // U64 sliderAttacks = Attacks::GetAttackMask(pieceType, attackerSquare, board.occupancy[BOTH]) & ~board.occupancy[enemy];
                        // U64 blockers = ray & sliderAttacks & board.occupancy[side];

                        U64 blockers = ray & board.occupancy[BOTH];

                        if (CountBits(blockers) == 1) {
                            int pinnedSquare = FirstBitIndex(blockers);
                            pinned |= (1ULL << pinnedSquare);
                            pinRays[pinnedSquare] = ray | (1ULL << attackerSquare);
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

                        list.add(0, EncodeMove(fromSquare, board.enPassant, piece, 0, 0, ENPASSANT_FLAG));
                    }
                }

                while (bitboard) {
                    int fromSquare = PopFirstBit(bitboard);

                    U64 ray = 0xFFFFFFFFFFFFFFFFULL;

                    if (GetBit(pinned, fromSquare)) {
                        if (pieceType == N) break;
                        ray = pinRays[fromSquare];
                    }
                    
                    U64 captures = Attacks::PawnCaptures[side][fromSquare] & board.occupancy[enemy] & captureMask & ray;

                    while (captures) {
                        int toSquare = PopFirstBit(captures);
                        if (!IsBitSet(board.occupancy[enemy], toSquare)) continue;

                        int captured = captureStart;

                        for (; captured < captureStart + 6; captured++) {
                            if (IsBitSet(board.bitboards[captured], toSquare)) break;
                        }

                        if (GetRank(toSquare) == promotionRank) {
                            list.add(0, EncodeMove(fromSquare, toSquare, piece, captured, piece + 4, CAPTURE_FLAG));
                            list.add(0, EncodeMove(fromSquare, toSquare, piece, captured, piece + 3, CAPTURE_FLAG));
                            list.add(0, EncodeMove(fromSquare, toSquare, piece, captured, piece + 2, CAPTURE_FLAG));
                            list.add(0, EncodeMove(fromSquare, toSquare, piece, captured, piece + 1, CAPTURE_FLAG));
                            continue;
                        }

                        list.add(0, EncodeMove(fromSquare, toSquare, piece, captured, 0, CAPTURE_FLAG));
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

                        U64 attacks = Attacks::GetAttackMask(pieceType, fromSquare, board.occupancy[BOTH]) & board.occupancy[enemy] & (blockMask | captureMask) & ray;
                    
                        while (attacks) {
                            int toSquare = PopFirstBit(attacks);
                            int captured = captureStart;

                            for (; captured < captureStart + 6; captured++) {
                                if (IsBitSet(board.bitboards[captured], toSquare)) break;
                            }

                            list.add(0, EncodeMove(fromSquare, toSquare, piece, captured, 0, CAPTURE_FLAG));
                        }
                    }
                }
            }
        }
    }
}