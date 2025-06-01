#include <chrono>
#include <iostream>

#include "Board.h"

U64 Board::Perft(int depth) {
    if (depth == 0) return 1;

    MoveList list;
    GenerateMoves(list);

    // for (int i = 0; i < list.length; i++) {
    //     int move = list.moves[i];
    //     int from = GetFromSquare(move);
    //     int to = GetToSquare(move);
    //     std::cout << "Move: " << ToMoveString(move) << " / " << ToSquareString(from) << ToSquareString(to) << " {" << move << "}" << "\n";
    //     int piece = GetMovedPiece(move);
    //     int captured = GetCapturedPiece(move);
    //     int promoted = GetPromotedPiece(move);
    //     int flag = GetFlag(move);
    //     std::cout << "from:      " << ToSquareString(from) << " (" << from << ")\n";
    //     std::cout << "to:        " << ToSquareString(to) << " (" << to << ")\n";
    //     std::cout << "moved:     " << PIECE_CHAR[piece] << " (" << piece << ")\n";
    //     std::cout << "promoted:  " << PIECE_CHAR[promoted] << " (" << promoted << ")\n";
    //     if (flag) std::cout << "Has flag: " << (flag >> 24) << "\n";
    // }

    U64 n = 0;

    for (int i = 0; i < list.length; i++) {
        if (!MakeMove(list.moves[i])) continue;
        n += Perft(depth - 1);
        //Print();
        TakeMove();
    }

    return n;
}

void Board::PerftTest(int depth) {
    //std::cout << "About to run perft for real\n";

    U64 nodes = 0;

    MoveList list;
    GenerateMoves(list);

    // for (int i = 0; i < list.length; i++) {
    //     int move = list.moves[i];
    //     int from = GetFromSquare(move);
    //     int to = GetToSquare(move);
    //     std::cout << "Move: " << ToMoveString(move) << " / " << ToSquareString(from) << ToSquareString(to) << " {" << move << "}" << "\n";
    //     int piece = GetMovedPiece(move);
    //     int captured = GetCapturedPiece(move);
    //     int promoted = GetPromotedPiece(move);
    //     int flag = GetFlag(move);
    //     std::cout << "from:      " << ToSquareString(from) << " (" << from << ")\n";
    //     std::cout << "to:        " << ToSquareString(to) << " (" << to << ")\n";
    //     std::cout << "moved:     " << PIECE_CHAR[piece] << " (" << piece << ")\n";
    //     std::cout << "promoted:  " << PIECE_CHAR[promoted] << " (" << promoted << ")\n";
    //     if (flag) std::cout << "Has flag: " << (flag >> 24) << "\n";
    // }

    std::cout << "Running perft to depth " << depth << "...\n";

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < list.length; i++) {
        int move = list.moves[i];
        //std::cout << "Leaf: " << ToMoveString(move) << " {" << move << "}" << "\n";

        U64 newNodes = 0;

        if (!MakeMove(move)) continue;
        newNodes = Perft(depth - 1);
        //Print();
        TakeMove();

        nodes += newNodes;

        std::cout << ToMoveString(move) << ": " << newNodes << "\n";
    }

    //Print();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    int nps = 0;

    double seconds = (double) duration / 1000;

    if (seconds > 0) {
        nps = (int) (nodes / seconds);
    }

    std::cout << "Total nodes:         " << nodes << "\n";
    std::cout << "Duration:            " << duration << " ms\n";
    std::cout << "Nodes per second:    ";
    nps != 0 ? std::cout << nps : std::cout << "undefined";
    std::cout << "\n";
}