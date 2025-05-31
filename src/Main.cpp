#include "Board.h"

int main() {
    Board board;
    board.Print();
    board.ParseFEN("rnbqkbnr/pp1ppppp/8/2p5/4PP2/8/PPPP2PP/RNBQKBNR b KQkq f3 0 2");
    board.Print();

    return 0;
}
