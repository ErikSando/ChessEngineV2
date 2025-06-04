#include <iostream>
#include <sstream>
#include <vector>

#include "CLI.h"
#include "Perft.h"
#include "Utils.h"

void CommandLoop() {
    Board board;
    TTable ttable(64);
    Searcher searcher(ttable);

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
            std::cout << "Type a move without a command to make a move, e.g. g7g8q.\n";
            std::cout << "help\n - Shows this menu.\n";
            std::cout << "exit | quit\n - Exit the program.\n";
            std::cout << "print\n - Prints the board to the console. White pieces are uppercased, black pieces are lowercased.\n";
            //std::cout << "make | play [move]\n - Play a move. Must be written in from-to-promotion form, e.g. g7g8q.\b";
            std::cout << "take | undo\n - Undo the last move.\n";
            std::cout << "reset\n - Load the starting position.\n";
            std::cout << "loadfen [fen]\n - Parse the given FEN string.\n";
            std::cout << "perft [depth]\n - Traverses through the game tree to a specified depth, counting the number of nodes.\n";
            std::cout << "search | go [...options]\n - Enter UCI mode. Options are: -depth [depth] -time [time]. By default, five seconds is used.\n";
            std::cout << "eval\n - Calculate and print the static evaluation of the position.\n";
            std::cout << "uci\n - Enter UCI mode.\n";
        }
        else if (cmd == "print") {
            board.Print();
        }
        // else if (cmd == "make" || cmd == "play") {
        //     if (args.size() < 2) {
        //         std::cout << "Insufficient arguments" << std::endl;
        //         std::cout << "Usage: " << cmd << " [move]" << std::endl;
        //         continue;
        //     }

        //     std::string move = args.at(1);

        //     if (move.size() < 4) {
        //         std::cout << "Invalid move: not enough characters.\n";
        //         continue;
        //     }

        //     if (!board.ParseMove(move)) {
        //         std::cout << "Invalid/illegal move.\n";
        //         continue;
        //     }

        //     board.Print();
        // }
        else if (cmd == "take" || cmd == "undo") {
            if (board.ply <= 0) {
                std::cout << "Cannot take move: no previous board states to revert to.\n";
                continue;
            }

            board.TakeMove();
            board.Print();
        }
        else if (cmd == "reset") {
            board.ParseFEN(START_FEN);
            board.Print();
        }
        else if (cmd == "loadfen") {
            std::string fenstr;

            for (int i = 1; i < args.size(); i++) {
                fenstr += args.at(i) + " ";
            }

            board.ParseFEN(fenstr.c_str());
            board.Print();
        }
        else if (cmd == "perft") {
            if (args.size() < 2) {
                std::cout << "Insufficient arguments" << std::endl;
                std::cout << "Usage: perft [depth]" << std::endl;
                continue;
            }

            int depth = std::stoi(args.at(1));

            Perft::PerftTest(board, depth);
        }
        else if (cmd == "search" || cmd == "go") {
            int time = 5;
            int depth = 0;

            bool timeSet = false;
            bool useDefault = true;

            for (int i = 1; i < args.size(); i++) {
                std::string arg = args.at(i);

                if (arg.at(0) == '-') {
                    if (arg == "-time") {
                        timeSet = true;
                        useDefault = false;

                        if (i < args.size() - 1) {
                            time = std::stoi(args.at(i + 1));
                        }
                    }
                    else if (arg == "-depth") {
                        useDefault = false;

                        if (i < args.size() - 1) {
                            depth = std::stoi(args.at(i + 1));
                        }
                    }
                }
            }

            SearchInfo info;

            if (depth > 0) info.depth = depth;

            if (timeSet || useDefault) {
                info.timeSet = true;
                info.startTime = Utils::GetTimeMS();
                info.stopTime = info.startTime + time * 1000;
            }

            searcher.Search(board, info);
        }
        else if (cmd == "eval") {
        }
        else if (cmd == "uci") {
            UCILoop(board, ttable, searcher);
        }
        // debug commands
        #ifndef NDEBUG

        else if (cmd == "sqatt") {
            if (args.size() < 2) {
                std::cout << "Insufficient arguments" << std::endl;
                std::cout << "Usage: sqatt [square]" << std::endl;
                continue;
            }

            std::string squarestr = args.at(1);
            int square = Utils::ToSquare(squarestr);

            std::cout << (board.IsSquareAttacked(square) ? "True" : "False") << "\n";
        }

        #endif
        
        else {
            // if (cmd.size() > 0) {
            //     std::cout << "Unknown command: '" << cmd << "'. Use 'help' for a list of commands.\n";
            // }

            std::string move = cmd;

            if (move.size() < 4) {
                std::cout << "Invalid move: not enough characters.\n";
                continue;
            }

            if (!Utils::ParseMove(board, move)) {
                //std::cout << "Invalid/illegal move.\n";
                continue;
            }

            board.Print();
        }
    }
}