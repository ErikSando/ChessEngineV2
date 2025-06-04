#include "TTable.h"

TTable::TTable(const int MB) {
    size_t bytes = MB * 0xFFFFF;
    size = bytes / sizeof(TEntry);
    entries = (TEntry*) malloc(bytes);
}

void TTable::StoreEntry() {
}

int TTable::GetEntry(U64 positionKey) {
    int index = positionKey % size;
    return 0;
}