#pragma once

#include "Board.h"
#include "Globals.h"

namespace ErikEngine {
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

        void Resize(const int MB);
        void Clear();

        void StoreEntry(Board& board, int move, int score, int flag, int depth);
        int GetEntry(Board& board, int& pvMove, int alpha, int beta, int depth) const;

        int GetPVMove(U64 hashKey) const;
        void GetPVLine(Board& board, int* pvLine, int depth) const;

        inline int age() const { return m_age; }
        inline int size() const { return m_size; }

        inline void IncAge() { m_age++; }

        private:

        int m_age;
        int m_size;

        TTEntry* m_entries;
    };
}