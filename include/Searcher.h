#pragma once

#include <atomic>

#include "TTable.h"

constexpr int NULL_MOVE_REDUCTION = 2;
constexpr int FULL_DEPTH_MOVES = 5;
constexpr int MIN_REDUCTION_DEPTH = 3;
constexpr int ASPIRATION_WINDOW = 50;
constexpr int WINDOW_EXPANSION_FACTOR = 30;

inline int CalculateWindow(int fails) {
    return ASPIRATION_WINDOW + fails * WINDOW_EXPANSION_FACTOR;
}

enum PostType {
    DEFAULT, UCI
};

struct SearchInfo {
    int nodes = 0;
    int startTime;
    int stopTime;
    int depth = MAX_DEPTH;
    int postType = DEFAULT;
    bool timeSet = false;
    bool post = true;
    bool quitting = false;
    std::atomic<bool> stopped = false;
};

class Searcher {
    public:

    Searcher(TTable& t_table) : ttable(t_table) {}

    void ClearTTable();
    void Search(Board& board, SearchInfo& info);

    private:

    TTable& ttable;

    int Quiescence(Board& board, SearchInfo& info, int alpha, int beta);
    int AlphaBeta(Board& board, SearchInfo& info, int depth, int alpha, int beta, bool doNull);
};