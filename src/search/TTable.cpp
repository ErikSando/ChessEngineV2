#include <cmath>
#include <iostream>
#include <string.h>

#include "Data.h"
#include "Debug.h"
#include "TTable.h"

TTable::TTable(const int MB) {
    Resize(MB);
}

TTable::~TTable() {
    free(entries);
}

void TTable::Resize(const int MB) {
    size_t bytes = MB * 0x100000;
    size = bytes / sizeof(TTEntry);

    free(entries);
    entries = static_cast<TTEntry*>(malloc(bytes));
    
    if (!entries) {
        size = 0;
        std::cout << "Failed to initialise transposition table.\n";
        return;
    }

    Clear();

    debug(std::cout << "Initialised transposition table with " << size << " entries.\n");
}

void TTable::Clear() {
    age = 0;
    if (!entries) return;
    memset(entries, 0, size * sizeof(TTEntry));
}

void TTable::StoreEntry(Board& board, int move, int score, int flag, int depth) {
    int index = board.hashKey % size;
    TTEntry* entry = &entries[index];

    bool replace = entry->hashKey == 0ULL || /*entry->age < age ||*/ entry->depth < depth;

    if (!replace) return;

    if (score > MATE_SCORE) score += board.ply;
    else if (score < -MATE_SCORE) score -= board.ply;

    entry->hashKey = board.hashKey;
    entry->move = move;
    entry->score = score;
    entry->flag = flag;
    entry->depth = depth;
    //entry->age = age;
}

int TTable::GetEntry(Board& board, int& pvMove, int alpha, int beta, int depth) const {
    int index = board.hashKey % size;
    const TTEntry* entry = &entries[index];

    if (board.hashKey != entry->hashKey) return NO_SCORE;

    pvMove = entry->move;

    if (entry->depth < depth) return NO_SCORE;

    int score = entry->score;

    if (score > MATE_SCORE) score -= board.ply;
    else if (score < -MATE_SCORE) score += board.ply;

    switch (entry->flag) {
        case EXACT_FLAG: return score;
        case ALPHA_FLAG: if (score <= alpha) return alpha; break;
        case BETA_FLAG: if (score >= beta) return beta;
    }

    return NO_SCORE;
}

int TTable::GetPVMove(U64 hashKey) const {
    int index = hashKey % size;
    const TTEntry* entry = &entries[index];

    return entry->move * (hashKey == entry->hashKey);
}

void TTable::GetPVLine(Board& board, int* pvLine, int depth) const {
    int ply;

    for (ply = 0; ply < depth && ply < MAX_DEPTH; ply++) {
        int move = GetPVMove(board.hashKey);
        if (!move || !board.MakeMove(move)) break;
        pvLine[ply] = move;
    }

    if (ply < MAX_DEPTH) pvLine[ply] = 0;

    while (board.ply) board.TakeMove();
}