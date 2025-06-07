#pragma once

#include "Board.h"
#include "Globals.h"

enum TTFlag {
    EXACT_FLAG, ALPHA_FLAG, BETA_FLAG
};

struct TTEntry {
    U64 hashKey;
    int move;
    int score;
    int flag;
    int depth;
    int age;
};

class TTable {
    public:

    TTable(const int MB);
    ~TTable();

    void Clear();

    void StoreEntry(U64 hashKey, int move, int score, int flag, int depth);
    int GetEntry(U64 hashKey, int& pvMove, int alpha, int beta, int depth) const;

    void GetPVLine(Board& board, int* pvLine, int depth) const;

    int age;

    private:

    int GetPVMove(U64 hashKey) const;
    
    int size;
    TTEntry* entries;
};