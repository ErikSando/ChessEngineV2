#pragma once

#include "Board.h"

namespace ErikEngine {
    namespace Evaluation {
        void Init();
        int Evaluate(Board& board);
    }
}