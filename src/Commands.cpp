#include <iostream>
#include <sstream>
#include <vector>

#include "Board.h"
#include "CLI.h"

void CommandLoop() {
    Board board;

    // board.ParseFEN("rnbq2nr/ppppk1pp/8/4pP2/P7/P7/2PPKPPP/RNBQ1BNR b - - 0 0");
    // board.ParseFEN("rnbq2nr/ppppk2p/8/4/pPp1/P7/P7/2PPKPPP/RNBQ1BNR w - g6 0 0");
    // board.ParseFEN("rnbqkbnr/pppp1ppp/8/8/5p2/7N/PPPPP1PP/RNBQKB1R w KQkq - 0 0");
    // board.ParseFEN("rnbqkbnr/ppppppp1/7p/8/8/3P4/PPP1PPPP/RNBQKBNR w KQkq - 0 0");
    // board.ParseFEN("rnbqkbnr/p1pppppp/8/1P6/8/8/1PPPPPPP/RNBQKBNR b KQkq - 0 0");
    // board.Print();
    // // PrintBitboard(board.knightAttacks[G8] & ~board.occupancy[Black]);
    // // PrintBitboard(board.GetRookAttacks(H8, board.occupancy[Both]) & ~board.occupancy[Black]);
    // board.ParseMove("c1h6");
    // PrintBitboard(board.bitboards[BP]);
    // board.Print();

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
        else if (cmd == "make" || cmd == "play") {
            if (args.size() < 2) {
                std::cout << "Insufficient arguments" << std::endl;
                std::cout << "Usage: " << cmd << " [move]" << std::endl;
                continue;
            }

            std::string move = args.at(1);

            if (move.size() < 4) {
                std::cout << "Invalid move: not enough characters.\n";
                continue;
            }

            if (!board.ParseMove(move)) {
                std::cout << "Invalid/illegal move.\n";
                continue;
            }

            board.Print();
        }
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

            board.PerftTest(depth);
        }
        else if (cmd == "search" || cmd == "go") {
            for (int i = 1; i < args.size(); i++) {
                std::string arg = args.at(i);
                
                int time = 5;
                int depth = -1;

                bool timeSet = false;
                bool useDefault = true;

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

                SearchInfo info{ .depth = depth, .timeSet = timeSet };

                if (timeSet) {
                    info.startTime = GetTimeMS();
                    info.stopTime = info.startTime + time * 1000;
                }
            }
        }
        else if (cmd == "eval") {
        }
        else if (cmd == "uci") {
            UCILoop(board);
        }
        else {
            // if (cmd.size() > 0) {
            //     std::cout << "Unknown command: '" << cmd << "'. Use 'help' for a list of commands.\n";
            // }

            std::string move = cmd;

            if (move.size() < 4) {
                std::cout << "Invalid move: not enough characters.\n";
                continue;
            }

            if (!board.ParseMove(move)) {
                std::cout << "Invalid/illegal move.\n";
                continue;
            }

            board.Print();
        }
    }
}