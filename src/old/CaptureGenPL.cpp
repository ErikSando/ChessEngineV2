// #include "Attacks.h"
// #include "Board.h"
// #include "Globals.h"
// #include "Move.h"
// #include "[old]MoveGen.h"

// namespace ErikEngine {
//     namespace MoveGen {
//         void GenerateCapturesPL(const Board& board, MoveList& list) {
//             list.length = 0;

//             int side = board.side;
//             int enemy = side ^ 1;
//             int promotionRank = enemy ? RANK_8 : RANK_1;
//             int captureStart = enemy ? BP : WP;

//             // pawns
//             int pieceType = P;
//             int piece = enemy ? WP : BP;
//             U64 bitboard = board.bitboards[piece];

//             while (bitboard) {
//                 int fromSquare = PopFirstBit(bitboard);
//                 U64 captures = Attacks::PawnCaptures[side][fromSquare] & board.occupancy[enemy];

//                 if (board.enPassant != NO_SQUARE) {
//                     if (IsBitSet(Attacks::PawnCaptures[side][fromSquare], board.enPassant)) {
//                         list.add(MoveScoring::MvvLvaScore[P][P], EncodeMove(fromSquare, board.enPassant, piece, 0, 0, ENPASSANT_FLAG));
//                     }
//                 }

//                 while (captures) {
//                     int toSquare = PopFirstBit(captures);
//                     int captured = captureStart;

//                     for (; captured < captureStart + 6; captured++) {
//                         if (IsBitSet(board.bitboards[captured], toSquare)) break;
//                     }

//                     int mvvlva = MoveScoring::MvvLvaScore[captured][P];

//                     if (GetRank(toSquare) == promotionRank) {
//                         list.add(mvvlva + MoveScoring::PromotionScore[Q], EncodeMove(fromSquare, toSquare, piece, captured, piece + 4, CAPTURE_FLAG));
//                         list.add(mvvlva + MoveScoring::PromotionScore[R], EncodeMove(fromSquare, toSquare, piece, captured, piece + 3, CAPTURE_FLAG));
//                         list.add(mvvlva + MoveScoring::PromotionScore[B], EncodeMove(fromSquare, toSquare, piece, captured, piece + 2, CAPTURE_FLAG));
//                         list.add(mvvlva + MoveScoring::PromotionScore[N], EncodeMove(fromSquare, toSquare, piece, captured, piece + 1, CAPTURE_FLAG));
//                         continue;
//                     }

//                     list.add(mvvlva, EncodeMove(fromSquare, toSquare, piece, captured, 0, CAPTURE_FLAG));
//                 }
//             }

//             piece++;
//             pieceType++;

//             for (; pieceType <= K; piece++, pieceType++) {
//                 bitboard = board.bitboards[piece];

//                 while (bitboard) {
//                     int fromSquare = PopFirstBit(bitboard);
//                     U64 attacks = Attacks::GetPieceAttacks(pieceType, fromSquare, board.occupancy[BOTH]) & board.occupancy[enemy];

//                     while (attacks) {
//                         int toSquare = PopFirstBit(attacks);
//                         int captured = captureStart;

//                         for (; captured < captureStart + 6; captured++) {
//                             if (IsBitSet(board.bitboards[captured], toSquare)) break;
//                         }

//                         list.add(
//                             MoveScoring::MvvLvaScore[captured][pieceType],
//                             EncodeMove(fromSquare, toSquare, piece, captured, 0, CAPTURE_FLAG)
//                         );
//                     }
//                 }
//             }
//         }
//     }
// }