// I am considering making a simple tuning framework
// For now I want to implement self play

#pragma once

#include "Board.h"
#include "Searcher.h"

namespace ErikEngine {
    namespace Tuning {
        int SelfPlay(Board& board, Searcher& searcher, int depth = 6);
    }
}