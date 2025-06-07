#include <condition_variable>
#include <mutex>

#include "Board.h"
#include "CLI.h"
#include "Searcher.h"

void SearchThread(Board& board, Searcher& searcher, SearchInfo& info) {
    while (true) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [] { return search_requested || !engine_running; });

        if (!engine_running) break;

        searcher.Search(board, info);
        search_requested = false;
    }
}