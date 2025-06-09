#include <condition_variable>
#include <mutex>

#include <iostream>

#include "Board.h"
#include "CLI.h"
#include "Perft.h"
#include "Searcher.h"

void WorkerThread(Board& board, Searcher& searcher, SearchInfo& info) {
    while (true) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [] { return search_requested || perft_requested || !engine_running; });

        if (!engine_running) break;

        if (search_requested) {
            searcher.Search(board, info);
            search_requested = false;
            continue;
        }

        if (perft_requested) {
            Perft::Perft(board, perft_depth);
            perft_requested = false;
            continue;
        }
    }
}