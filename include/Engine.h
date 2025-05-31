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

    void GenerateMoves(const Board& board, MoveList& list);
    void GenerateCaptures(const Board& board, MoveList& list);

    int Evaluate(const Board& board);

    int Quiescence();
    void Search(Board& board, const SearchInfo& info);
};