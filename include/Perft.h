#pragma once

#include <atomic>

#include "Board.h"

namespace ErikEngine {
    namespace Perft {
        void Perft(Board& board, int depth);
    }
}