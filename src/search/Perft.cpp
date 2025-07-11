#include <chrono>
#include <iostream>

#include "Debug.h"
#include "CLI.h"
#include "MoveGen.h"
#include "Perft.h"
#include "Utils.h"

namespace ErikEngine {
    namespace Perft {
        U64 CountNodes(Board& board, int depth) {
            EE_DEBUG(Debug::AssertValid(board));

            if (stop_perft || !depth) return 1;

            MoveList list;
            MoveGen::GenerateMoves<MoveType::Legal>(board, list);

            if (depth == 1) return list.size();

            U64 n = 0;

            for (size_t i = 0; i < list.size(); i++) {
                board.MakeMove<MoveContext::Perft, false>(list.move_at(i));
                n += CountNodes(board, depth - 1);
                board.TakeMove();
            }

            return n;
        }

        void Perft(Board& board, int depth) {
            U64 nodes = 0;

            MoveList list;
            MoveGen::GenerateMoves<MoveType::Legal>(board, list);

            std::cout << "Running perft to depth " << depth << "...\n";

            int start = Utils::GetTimeMS();

            for (size_t i = 0; i < list.size(); i++) {
                int move = list.move_at(i);

                U64 newNodes = 0;

                board.MakeMove<MoveContext::Perft, false>(move);
                newNodes = CountNodes(board, depth - 1);
                board.TakeMove();

                nodes += newNodes;

                if (stop_perft) {
                    if (engine_running) std::cout << "Stopping perft.\n";
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
}