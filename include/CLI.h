#pragma once

#include "Board.h"
#include "Searcher.h"
#include "TTable.h"

extern void CommandLoop();
extern void UCILoop(Board& board, Searcher& searcher);