#include <cmath>
#include <cstring>
#include <iostream>

#include "Data.h"
#include "Debug.h"
#include "TTable.h"

namespace ErikEngine {
    TTable::TTable(const int MB) {
        m_age = 0;
        m_size = 0;
        m_entries = nullptr;

        Resize(MB);
    }

    TTable::~TTable() {
        free(m_entries);
    }

    void TTable::Resize(const int MB) {
        size_t bytes = MB * 0x100000;
        m_size = bytes / sizeof(TTEntry);

        m_entries = static_cast<TTEntry*>(realloc(m_entries, bytes));

        if (!m_entries) {
            m_size = 0;
            std::cout << "Failed to initialise transposition table.\n";
            return;
        }

        Clear();

        EE_DEBUG(std::cout << "Initialised transposition table with " << m_size << " entries.\n");
    }

    void TTable::Clear() {
        m_age = 0;
        if (!m_entries) return;
        memset(m_entries, 0, m_size * sizeof(TTEntry));
    }

    void TTable::StoreEntry(Board& board, int move, int score, int flag, int depth) {
        int index = board.hashKey % m_size;
        TTEntry* entry = &m_entries[index];

        bool replace = entry->hashKey == 0ULL
                    || entry->age < m_age
                    || entry->depth < depth;

        if (!replace) return;

        if (score > MATE_SCORE) score += board.ply;
        else if (score < -MATE_SCORE) score -= board.ply;

        entry->hashKey = board.hashKey;
        entry->move = move;
        entry->score = score;
        entry->flag = flag;
        entry->depth = depth;
        entry->age = m_age;
    }

    int TTable::GetEntry(Board& board, int& pvMove, int alpha, int beta, int depth) const {
        int index = board.hashKey % m_size;
        const TTEntry* entry = &m_entries[index];

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
        int index = hashKey % m_size;
        const TTEntry* entry = &m_entries[index];

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
}