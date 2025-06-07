#include <cmath>
#include <iostream>
#include <string.h>

#include "TTable.h"

TTable::TTable(const int MB) {
    size_t bytes = MB * 0x100000;
    size = bytes / sizeof(TEntry);

    if ((size & (size - 1)) != 0) { // round down to power of 2 for fast hashing
        size = std::pow(2, std::floor(std::log2(size)));
        bytes = size * sizeof(TEntry);

        std::cout << "Rounding transposition table size down to " << bytes << " bytes.\n";
    }

    entries = static_cast<TEntry*>(malloc(bytes));
    
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
    if (!entries) return;
    memset(entries, 0, size * sizeof(TEntry));
}

void TTable::StoreEntry(U64 hashKey) {
    int index = hashKey & (size - 1);
}

int TTable::GetEntry(U64 hashKey) {
    int index = hashKey & (size - 1);
    return 0;
}