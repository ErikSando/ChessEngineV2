#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "TTable.h"

constexpr int MAX_DEPTH = 64;
constexpr int INFINITY = 30000;

struct SearchInfo {
    int nodes = 0;
    int depth = MAX_DEPTH;
    bool timeSet = false;
    int startTime;
    int stopTime;
    std::atomic<bool> stopped = false;
};

class Searcher {
    public:

    Searcher(TTable& ttable);

    void Search(Board& board, SearchInfo& sinfo);

    private:

    TTable& ttable;

    int Quiescence(Board& board, SearchInfo& info, int alpha, int beta);
    int AlphaBeta(Board& board, SearchInfo& info, int depth, int alpha, int beta);
};