#pragma once

#include "Globals.h"

struct TEntry {
    U64 positionKey;
    int eval;
};

class TTable {
    public:

    TTable(const int MB);
    ~TTable();

    void Reset();

    void StoreEntry(U64 hashKey);
    int GetEntry(U64 positionKey);

    private:

    int size;
    TEntry* entries;
};