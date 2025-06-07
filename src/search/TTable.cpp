#include <cmath>
#include <iostream>
#include <string.h>

#include "Data.h"
#include "TTable.h"

TTable::TTable(const int MB) {
    size_t bytes = MB * 0x100000;
    size = bytes / sizeof(TTEntry);

    entries = static_cast<TTEntry*>(malloc(bytes));
    
    if (!entries) {
        size = 0;
        std::cout << "Failed to initialise transposition table.\n";
        return;
    }

    Clear();

    std::cout << "Initialised transposition table with " << size << " entries.\n";
}

TTable::~TTable() {
    free(entries);
}

void TTable::Clear() {
    age = 0;
    if (!entries) return;
    memset(entries, 0, size * sizeof(TTEntry));
}

void TTable::StoreEntry(U64 hashKey, int move, int score, int flag, int depth) {
    int index = hashKey % size;
    TTEntry* entry = &entries[index];

    bool replace = entry->hashKey == 0ULL || entry->age < age || entry->depth < depth;

    if (!replace) return;

    entry->hashKey = hashKey;
    entry->move = move;
    entry->score = score;
    entry->flag = flag;
    entry->depth = depth;
    entry->age = age;
}

int TTable::GetEntry(U64 hashKey, int& pvMove, int alpha, int beta, int depth) {
    int index = hashKey % size;
    TTEntry* entry = &entries[index];

    if (hashKey != entry->hashKey) return NO_SCORE;

    pvMove = entry->move;

    if (entry->depth < depth) return NO_SCORE;

    int score = entry->score;

    switch (entry->flag) {
        case EXACT_FLAG: return score;
        case ALPHA_FLAG: if (score <= alpha) return alpha; break;
        case BETA_FLAG: if (score >= beta) return beta;
    }

    return NO_SCORE;
}