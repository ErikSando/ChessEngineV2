// #include "Board.h"
// #include "MoveGen.h"
// #include "Searcher.h"
// #include "Tuning.h"

// namespace ErikEngine {
//     namespace Tuning {
//         int SelfPlay(Board& board, Searcher& searcher, int depth) {
//             board.ParseFEN(START_FEN);

//             SearchInfo info;

//             MoveList list;
//             MoveGen::GenerateMoves(board, list);

//             while (list.length) {
//                 board.ply = 0;

//                 searcher.AlphaBeta(board, info, depth, -INF, INF, true);
//                 int pvMove = searcher.ttable.GetPVMove(board.hashKey);

//                 board.MakeMove(pvMove);

//                 MoveGen::GenerateMoves(board, list);
//             }

//             board.Print();

//             bool checkmate = board.IsSquareAttacked(FirstBitIndex(board.bitboards[board.side == WHITE ? WK : BK]));

//             if (checkmate) return board.side ^ 1;
//             else return BOTH;
//         }
//     }
// }