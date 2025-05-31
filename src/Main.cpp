#include "Board.h"

int main() {
    Board board;
    board.Print();

    MoveList list;
    board.GenerateMoves(list);

    return 0;
}
