#pragma once

#include <atomic>

#include "Board.h"

extern std::atomic<bool> stop_perft;

namespace PerfTester {
    void PerfTest(Board& board, int depth);
}