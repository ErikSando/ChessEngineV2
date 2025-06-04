#pragma once

#include "TTable.h"

constexpr int MAX_DEPTH = 64;
constexpr int INFINITY = 30000;

struct SearchInfo {
    int depth = MAX_DEPTH;
    int startTime;
    int stopTime;
    bool timeSet = false;
};

class Searcher {
    public:

    Searcher(TTable& t_table) : ttable(t_table) {}

    TTable& ttable;

    void Search(Board& board, const SearchInfo& info);
};