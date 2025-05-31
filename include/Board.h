#pragma once

#include "Globals.h"

struct BoardInfo {

    U64 key;
};

class Board {
    public:

    Board();

    U64 bitboards[12];

    // functions here are currently a to do list

    void LoadFEN(const char* fen);
    void Print();

    const char* GenerateFEN();

    bool CheckDraw();
};