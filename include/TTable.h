#pragma once

#include "Globals.h"

struct TEntry {
    U64 positionKey;
    int eval;
};

class TTable {
    public:

    TTable(const int MB);

    void Reset();
    void StoreEntry();
    
    int GetEntry(U64 positionKey);

    private:

    int size;
    TEntry* entries;
};