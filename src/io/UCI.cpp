#include <iostream>
#include <sstream>
#include <vector>

#include "CLI.h"
#include "Utils.h"

void UCILoop(Board& board, TTable& ttable, Searcher& searcher) {
    std::cout << "uciok\n";
    std::string command;

    for (;;) {
        std::cout << "> ";
        std::getline(std::cin, command);

        std::istringstream iss(command);
        std::vector<std::string> args;
        std::string arg;

        while (iss >> arg) {
            args.emplace_back(arg);
        }

        if (args.size() < 1) continue;

        std::string cmd = args.at(0);

        if (cmd == "exit" || cmd == "quit") {
            break;
        }
        else if (cmd == "ucinewgame") {
            board.ParseFEN(START_FEN);
        }
        else if (cmd == "uci") {
            std::cout << "uciok\n";
        }
        else if (cmd == "info") {
            std::cout << "info name " << ENGINE_NAME << " v" << ENGINE_VERSION << "\n";
            std::cout << "info author " << ENGINE_AUTHOR << "\n";
        }
        else if (cmd == "ready") {
            std::cout << "readyok\n";
        }
        else if (cmd == "position") {
            // position startpos
            // position fen ...
            // position moves e2e4 d2d4 ...

        }
        else if (cmd == "go") {
            // go depth ... movetime ... wtime ... btime ... winc ... binc ... movestogo ...
        }
    }
}