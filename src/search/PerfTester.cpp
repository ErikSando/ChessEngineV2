#include <chrono>
#include <iostream>

#include "Debug.h"
#include "MoveGen.h"
#include "PerfTester.h"
#include "Utils.h"

namespace PerfTester {
    U64 CountNodes(Board& board, int depth) {
        // debug(Debug::CheckBoardValid(board));
        //debug(board.CheckValid());

        if (depth == 0) return 1;

        MoveList list;
        MoveGen::GenerateMoves(board, list);

        // requires legal move generator
        // if (depth == 1) return list.length;

        U64 n = 0;

        for (int i = 0; i < list.length; i++) {
            if (!board.MakeMove(list.moves[i].move)) continue;
            n += CountNodes(board, depth - 1);
            board.TakeMove();
        }

        return n;
    }

    void PerfTest(Board& board, int depth) {
        U64 nodes = 0;

        MoveList list;
        MoveGen::GenerateMoves(board, list);

        std::cout << "Running perft to depth " << depth << "...\n";

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < list.length; i++) {
            int move = list.moves[i].move;

            U64 newNodes = 0;

            if (!board.MakeMove(move)) continue;
            newNodes = CountNodes(board, depth - 1);
            board.TakeMove();

            nodes += newNodes;

            std::cout << Utils::ToMoveString(move) << ": " << newNodes << "\n";
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        int nps = 0;

        double seconds = (double) duration / 1000;

        if (seconds > 0) {
            nps = (int) (nodes / seconds);
        }

        std::cout << "Total nodes:     " << nodes << "\n";
        std::cout << "Duration:        " << duration << " ms\n";
        std::cout << "Nodes/second:    ";
        nps != 0 ? std::cout << nps : std::cout << "undefined";
        std::cout << "\n";
    }
}