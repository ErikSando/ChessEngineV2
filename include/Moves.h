#pragma once

#include <array>

#define MAX_MOVES 256

struct MoveList {
    std::array<int, MAX_MOVES> moves;
    int length = 0;
};