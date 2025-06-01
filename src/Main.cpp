#include <iostream>

#include "Board.h"
#include "Moves.h"

int main() {
    Board board;
    board.Print();

    //board.ParseFEN("rnbqkbnr/ppp2ppp/8/3pp3/3PP3/8/PPP2PPP/RNBQKBNR w KQkq d6 0 3");
    //board.ParseFEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 0");
    // board.ParseFEN("4k2r/p2bp2p/1r1p1q2/8/5B2/2QB3P/P3P1P1/4K3 b KQkq c5 0 1");
    // board.Print();

    // MoveList list;
    // board.GenerateMoves(list);

    // for (int i = 0; i < 10; i++) {
    //     MoveList list;
    //     board.GenerateMoves(list);
    //     board.MakeMove(list.moves[15]);
    //     board.Print();
    //     std::cout << "\n";
    // }
    // for (int i = 0; i < 10; i++) {
    //     board.TakeMove();
    //     board.Print();
    //     std::cout << "\n";
    // }

    // for (int i = 0; i < list.length; i++) {
    //     int move = list.moves[i];
    //     int from = GetFromSquare(move);
    //     int to = GetToSquare(move);
    //     std::cout << ToSquareString(from) << ToSquareString(to) << "\n";
    // }

    // board.MakeMove(list.moves[1]);
    // board.Print();
    // board.TakeMove();
    // board.Print();

    // board.Print();

    //std::cout << "About to run perft" << std::endl;

    board.PerftTest(2);

    return 0;
}
