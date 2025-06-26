#pragma once

#include <atomic>

#include "Globals.h"
#include "TTable.h"

namespace ErikEngine {
    enum PostType {
        DEFAULT, UCI
    };

    struct SearchInfo {
        int nodes = 0;
        U64 startTime;
        U64 stopTime;
        int depth = MAX_DEPTH;
        int postType = DEFAULT;
        bool timeSet = false;
        bool post = true;
        bool postBestMove = true;
        bool quitting = false;
        std::atomic<bool> stopped = false;
    };

    class Searcher {
        public:

        Searcher(TTable& t_table) : ttable(t_table) {}

        void ClearTTable();

        int Quiescence(Board& board, SearchInfo& info, int alpha, int beta);
        int AlphaBeta(Board& board, SearchInfo& info, int depth, int alpha, int beta, bool doNull);

        void Search(Board& board, SearchInfo& info);

        TTable& ttable;
    };
}