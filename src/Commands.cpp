#include <iostream>
#include <sstream>
#include <vector>

#include "Board.h"
#include "CLI.h"

void CommandLoop(Board& board) {
    std::istream input();
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
        else if (cmd == "help") {
            std::cout << "help\n - Shows this menu.\n";
            std::cout << "print\n - Prints the board to the console. White pieces are uppercased, black pieces are lowercased.\n";
            std::cout << "reset\n - Load the starting position.\n";
            std::cout << "perft [depth]\n - Traverses through the game tree to a specified depth, counting the number of nodes.\n";
            std::cout << "take\n - Undo the last move.\n";
            std::cout << "uci\n - Enter UCI mode.\n";
        }
        else if (cmd == "print") {
            board.Print();
        }
        else if (cmd == "perft") {
            if (args.size() < 2) {
                std::cout << "Insufficient arguments" << std::endl;
                std::cout << "Usage: perft [depth]" << std::endl;
                continue;
            }

            int depth = std::stoi(args.at(1));

            board.PerftTest(depth);
        }
        else if (cmd == "undo" || cmd == "take") {
            if (board.ply <= 0) {
                std::cout << "Cannot take move: no previous board states to revert to.\n";
                continue;
            }

            board.TakeMove();
        }
        else if (cmd == "reset") {
            board.ParseFEN(START_FEN);
            board.Print();
        }
        else if (cmd == "search" || cmd == "go") {
        }
        else if (cmd == "uci") {
            UCILoop(board);
        }
        else {
            if (cmd.size() > 0) {
                std::cout << "Unknown command: '" << cmd << "'. Use 'help' for a list of commands.\n";
            }
        }
    }
}