#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#include "Attacks.h"
#include "CLI.h"
#include "Evaluation.h"
#include "Perft.h"
#include "Tuning.h"
#include "Utils.h"

namespace ErikEngine {
    std::condition_variable cv;
    std::mutex mutex;

    bool engine_running = true;
    bool search_requested = false;
    bool perft_requested = false;
    bool stop_perft = false;
    int perft_depth = 1;

    void CleanUp(SearchInfo& search_info, std::thread& worker_thread) {
        search_info.stopped = true;
        search_info.quitting = true;
        stop_perft = true;
        cv.notify_one();
        worker_thread.join();
    }

    #include "MoveGen.h"

    void CommandLoop() {
        Board board;
        TTable ttable(64);
        Searcher searcher(ttable);
        SearchInfo search_info;

        board.ParseFEN(START_FEN);

        std::thread worker_thread(WorkerThread, std::ref(board), std::ref(searcher), std::ref(search_info));

        std::string command;

        for (;;) {
            std::getline(std::cin, command);

            std::istringstream iss(command);
            std::vector<std::string> args;
            std::string arg;

            while (iss >> arg) {
                args.emplace_back(arg);
            }

            if (args.size() < 1) continue;

            std::string& cmd = args[0];

            if (cmd == "exit" || cmd == "quit") {
                engine_running = false;
                CleanUp(search_info, worker_thread);
                break;
            }
            else if (cmd == "help") {
                std::cout << "Type a move on its own to play it, e.g. g7g8q.\n";
                std::cout << "Use 'ret' or 'return' to return to this command line mode if in a seperate command line mode e.g. UCI mode.\n";
                std::cout << "help\n - Shows this menu.\n";
                std::cout << "exit | quit\n - Exit the program.\n";
                std::cout << "print\n - Prints the board to the console. White pieces are uppercased, black pieces are lowercased.\n";
                std::cout << "reset\n - Load the starting position.\n";
                std::cout << "pos [fen]\n - Parse the given FEN string.\n";
                std::cout << "fen\n - Generate and print the FEN string of the current position.\n";
                std::cout << "take | undo\n - Undo the last move.\n";
                std::cout << "perft [depth]\n - Traverses through the game tree to the specified depth, counting the number of nodes.\n";
                std::cout << "search | go [...options]\n - Search for the best move. Options are: depth [ply] time [time in ms]. If no options are given, a time of 5 seconds is used.\n";
                std::cout << "stop\n - Cancel the search and print the best move found so far.\n";
                std::cout << "post [true/false]\n - Set whether the engine will print it's search results at each depth to the console.\n";
                std::cout << "eval\n - Calculate and print the static evaluation of the position.\n";
                std::cout << "qeval\n - Find and print the quiescent evaluation of the position.\n";
                std::cout << "selfplay [no. games]\n - The computer will play the specified number of games agaisnt itself, and print the results once complete.\n";
                std::cout << "uci\n - Enter UCI mode.\n";
            }
            else if (cmd == "uci") {
                UCILoop(board, searcher, search_info);

                if (!engine_running) {
                    CleanUp(search_info, worker_thread);
                    break;
                }

                search_info.postType = DEFAULT;
            }
            else if (cmd == "print") {
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
                searcher.ClearTTable();
                board.ParseFEN(START_FEN);
                board.Print();
            }
            else if (cmd == "pos") {
                std::string fenstr;

                for (size_t i = 1; i < args.size(); i++) {
                    fenstr += args[i] + " ";
                }

                board.ParseFEN(fenstr.c_str());
                board.Print();
            }
            else if (cmd == "fen") {
                const char* fenstr = board.GenerateFEN();
                std::cout << fenstr << "\n";
            }
            else if (cmd == "perft") {
                if (args.size() < 2) {
                    std::cout << "Insufficient arguments\n";
                    std::cout << "Usage: perft [depth]\n";
                    continue;
                }

                int depth = std::stoi(args[1]);

                if (depth < 1) {
                    std::cout << "Perft depth must be at least 1.\n";
                    continue;
                }

                stop_perft = false;
                perft_depth = depth;
                perft_requested = true;
                cv.notify_one();
            }
            else if (cmd == "search" || cmd == "go") {
                U64 time = 5000;
                int depth = MAX_DEPTH;

                bool timeSet = false;

                for (size_t i = 1; i < args.size() - 1; i++) {
                    std::string& arg = args[i];

                    if (arg == "time" || arg == "movetime") {
                        timeSet = true;
                        time = std::stoull(args[i + 1]);
                    }
                    else if (arg == "depth") {
                        depth = std::stoi(args[i + 1]);
                    }
                }

                search_info.depth = std::min(depth, MAX_DEPTH);
                search_info.timeSet = timeSet;
                search_info.startTime = Utils::GetTimeMS();
                search_info.stopTime = search_info.startTime + time;

                search_requested = true;
                cv.notify_one();
            }
            else if (cmd == "stop") {
                search_info.stopped = true;
                stop_perft = true;
            }
            else if (cmd == "post") {
                if (args.size() < 2) {
                    std::cout << "Insufficient arguments\n";
                    std::cout << "Usage: post [true/false]\n";
                    continue;
                }

                std::string& val = args[1];

                if (val == "true" || val == "1" || val == "on" || val == "enable" || val == "enabled") {
                    search_info.post = true;
                    std::cout << "Engine thinking enabled\n";
                }
                else if (val == "false" || val == "0" || val == "off" || val == "disable" || val == "disabled") {
                    search_info.post = false;
                    std::cout << "Engine thinking disabled\n";
                }
                else {
                    std::cout << "Unrecognised option '" << val << "'\n";
                }
            }
            else if (cmd == "eval") {
                int eval = Evaluation::Evaluate(board);

                std::cout << "Static evaluation: " << eval << "\n";
            }
            else if (cmd == "qeval") {
                search_info.timeSet = false;

                int eval = searcher.Quiescence(board, search_info, -INF, INF);

                std::cout << "Quiescent evaluation: " << eval << "\n";
            }
            else if (cmd == "magics") {
                std::cout << "WARNING: magic number generation is currently broken!\n";
                Attacks::FindMagics();
                std::cout << "WARNING: magic number generation is currently broken!\n";
            }
            // // command for testing legal move gen
            // else if (cmd == "moves") {
            //     MoveList list;
            //     MoveGen::GenerateMoves(board, list);

            //     for (int i = 0; i < list.length; i++) {
            //         std::cout << Utils::ToMoveString(list.moves[i].move) << "\n";
            //     }

            //     std::cout << "Moves: " << list.length << "\n";
            // }
            // // commands for testing capture gens
            // else if (cmd == "caps") {
            //     MoveList list;
            //     MoveGen::GenerateCapturesPL(board, list);

            //     for (int i = 0; i < list.length; i++) {
            //         std::cout << Utils::ToMoveString(list.moves[i].move) << "\n";
            //     }

            //     std::cout << "Moves: " << list.length << "\n";
            // }
            // else if (cmd == "lcaps") {
            //     MoveList list;
            //     MoveGen::GenerateCaptures(board, list);

            //     for (int i = 0; i < list.length; i++) {
            //         std::cout << Utils::ToMoveString(list.moves[i].move) << "\n";
            //     }

            //     std::cout << "Moves: " << list.length << "\n";
            // }
            // else if (cmd == "selfplay") {
            //     if (args.size() < 2) {
            //         std::cout << "Insufficient arguments\n";
            //         std::cout << "Usage: selfplay [no. games]\n";
            //         continue;
            //     }

            //     int games = std::stoi(args[1]);

            //     int results[3] = {0};

            //     for (int i = 0; i < games; i++) {
            //         std::cout << "Game " << (i + 1) << "/" << games << "\n";
            //         results[Tuning::SelfPlay(board, searcher, 6)]++;
            //     }

            //     std::cout << "White: " << results[WHITE] << "\n";
            //     std::cout << "Black: " << results[BLACK] << "\n";
            //     std::cout << "Draws: " << results[BOTH] << "\n";
            // }
            else {
                std::string& movestr = cmd;

                if (movestr.size() < 4) {
                    //std::cout << "Invalid move: not enough characters.\n";
                    continue;
                }

                int move = Utils::ParseMove(board, movestr);

                // if (!Utils::MoveExists(board, move)) {
                //     //std::cout << "Invalid move.\n";
                //     continue;
                // }

                // if (!board.MakeMove(move, true)) {
                //     //std::cout << "Illegal move.\n";
                //     continue;
                // }

                if (!Utils::MoveExists(board, move) || !board.MakeMove(move)) continue;

                board.Print();
            }
        }
    }
}