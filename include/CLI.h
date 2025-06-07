#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>

#include "Board.h"
#include "Searcher.h"
#include "TTable.h"

extern std::condition_variable cv;
extern std::mutex mutex;

extern bool engine_running;
extern bool search_requested;

extern void SearchThread(Board& board, Searcher& searcher, SearchInfo& search_info);

extern void CommandLoop();
extern void UCILoop(Board& board, Searcher& searcher, SearchInfo& search_info, std::thread& search_thread);