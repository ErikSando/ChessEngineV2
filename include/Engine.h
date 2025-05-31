#pragma once

#include "Board.h"
#include "Globals.h"
#include "Moves.h"
#include "Search.h"

enum class Protocol {
    UCI, XBoard
};

class Engine {
    public:

    Engine(Protocol protocol);

    void GenerateMoves(Board& board, MoveList& list);
    void GenerateCaptures(Board& board, MoveList& list);

    int Evaluate(Board& board);

    int Quiescence();
    void Search(Board& board, SearchInfo& info);
};