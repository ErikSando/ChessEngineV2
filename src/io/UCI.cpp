#include <iostream>
#include <sstream>
#include <vector>

#include "CLI.h"
#include "Utils.h"

void UCILoop(Board& board, Searcher& searcher, SearchInfo& search_info, std::thread& search_thread) {
    std::cout << "uciok\n";

    search_info.postType = UCI;

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

        std::string cmd = args.at(0);

        if (cmd == "exit" || cmd == "quit") {
            engine_running = false;
            return;
        }
        else if (cmd == "ret" || cmd == "return") {
            std::cout << "Returning to default command line.\n";
            return;
        }
        else if (cmd == "stop") {
            search_info.stopped = true;
        }
        else if (cmd == "go") {
            int depth = MAX_DEPTH;
            int movetime = -1;
            int inc = 0;
            int movestogo = 30;

            int time = -1;

            bool timeSet = false;

            for (size_t i = 1; i < args.size() - 1; i++) {
                std::string& arg = args[i];
                std::string& val = args[i + 1];

                if (arg == "depth") depth = std::stoi(val);
                else if (arg == "movetime") movetime = std::stoi(val);
                else if (board.side == WHITE && arg == "wtime") time = std::stoi(val);
                else if (board.side == BLACK && arg == "btime") time = std::stoi(val);
                else if (board.side == WHITE && arg == "winc") inc = std::stoi(val);
                else if (board.side == BLACK && arg == "binc") inc = std::stoi(val);
                else if (arg == "movestogo") movestogo = std::stoi(val);
            }

            search_info.Reset();

            if (movetime != -1) { // movetime takes precedence over wtime/btime
                time = movetime;
                movestogo = 1;
            }

            if (time != -1) {
                time /= movestogo;
                time += inc;
                time -= 50; // safety

                search_info.timeSet = true;
                search_info.startTime = Utils::GetTimeMS();
                search_info.stopTime = search_info.startTime + time;
            }

            search_info.depth = depth;

            search_requested = true;
            cv.notify_one();
        }
        else if (cmd == "position") {
            for (size_t i = 1; i < args.size() - 1; i++) {
                std::string& arg = args[i];

                if (arg == "startpos") {
                    board.ParseFEN(START_FEN);
                }
                else if (arg == "fen") {
                    std::string fenstr;

                    for (i++; i < args.size(); i++) {
                        std::string& val = args[i];
                        if (val == "moves") {
                            i--;
                            break;
                        }

                        fenstr += val + " ";
                    }

                    board.ParseFEN(fenstr.c_str());
                }
                else if (arg == "moves") {
                    for (i++; i < args.size(); i++) {
                        std::string& movestr = args[i];

                        if (!Utils::ParseMove(board, movestr)) {
                            std::cout << "Invalid move: '" << movestr << "'\n";
                        }
                    }
                }
            }
        }
        else if (cmd == "ucinewgame") {
            searcher.ClearTTable();
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
    }
}