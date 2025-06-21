#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>

#include "Board.h"
#include "Searcher.h"
#include "TTable.h"

extern std::condition_variable cv;
extern std::mutex mutex;

extern bool engine_running; // I dont think this or stop_perft need to be atomic, but probably good prectise
extern bool search_requested;
extern bool perft_requested;
extern bool stop_perft;
extern int perft_depth;

extern void WorkerThread(Board& board, Searcher& searcher, SearchInfo& search_info);

extern void CommandLoop();
extern void UCILoop(Board& board, Searcher& searcher, SearchInfo& search_info, std::thread& search_thread);