#include <chrono>
#include <iostream>

#include "Debug.h"
#include "MoveGen.h"
#include "Perft.h"
#include "Utils.h"

std::atomic<bool> stop_perft = false;

namespace PerfTester {
    U64 CountNodes(Board& board, int depth) {
        //debug(board.CheckValid());

        if (depth == 0) return 1;

        MoveList list;
        MoveGen::GenerateLegalMoves(board, list);

        // requires legal move generator
        if (depth == 1) return list.length;

        U64 n = 0;

        for (int i = 0; i < list.length; i++) {
            // board.MakeMove(list.moves[i].move);
            if (!board.MakeMove(list.moves[i].move)) continue;
            n += CountNodes(board, depth - 1);
            board.TakeMove();
        }

        if (stop_perft) return 0;

        return n;
    }

    void PerfTest(Board& board, int depth) {
        U64 nodes = 0;

        MoveList list;
        MoveGen::GenerateLegalMoves(board, list);

        std::cout << "Running perft to depth " << depth << "...\n";

        int start = Utils::GetTimeMS();

        for (int i = 0; i < list.length; i++) {
            int move = list.moves[i].move;

            U64 newNodes = 0;

            // board.MakeMove(move);
            if (!board.MakeMove(move)) continue;
            newNodes = CountNodes(board, depth - 1);
            board.TakeMove();

            nodes += newNodes;

            if (stop_perft) {
                std::cout << "Stopping perft.\n";
                return;
            }

            std::cout << Utils::ToMoveString(move) << ": " << newNodes << "\n";
        }

        int end = Utils::GetTimeMS();
        int duration = end - start;

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