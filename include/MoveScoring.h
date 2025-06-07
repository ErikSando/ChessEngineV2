#include <algorithm>
#include <string.h>

#include "Data.h"

namespace MoveScoring {
    extern int KillerMoves[2][64];
    extern int HistoryMoves[12][64];
    extern int CounterMoves[64][64];

    inline void Reset() {
        memset(KillerMoves, 0, sizeof(KillerMoves));
        memset(HistoryMoves, 0, sizeof(HistoryMoves));
    }

    constexpr int MAX_HISTORY_SCORE = 16384;
    constexpr int MAX_COUNTERMOVE_SCORE = 16384;

    inline void UpdateHistory(int& score, int depth) {
        int bonus = depth * depth;
        score += bonus - (score * bonus) / MAX_HISTORY_SCORE;
        score = std::clamp(score, 0, MAX_HISTORY_SCORE);
    }

    inline void UpdateCounterMove(int& score, int depth) {
        int bonus = depth * depth;
        score += bonus - (score * bonus) / MAX_COUNTERMOVE_SCORE;
        score = std::clamp(score, 0, MAX_COUNTERMOVE_SCORE);
    }

    namespace Heuristics {
        constexpr int MvvLvaScore[12][6] = {
            { 105, 104, 103, 102, 101, 100 },
            { 205, 204, 203, 202, 201, 200 },
            { 305, 304, 303, 302, 301, 300 },
            { 405, 404, 403, 402, 401, 400 },
            { 505, 504, 503, 502, 501, 500 },
            { 605, 604, 603, 602, 601, 600 },
            { 105, 104, 103, 102, 101, 100 },
            { 205, 204, 203, 202, 201, 200 },
            { 305, 304, 303, 302, 301, 300 },
            { 405, 404, 403, 402, 401, 400 },
            { 505, 504, 503, 502, 501, 500 },
            { 605, 604, 603, 602, 601, 600 }
        };
        
        constexpr int BasePromotionScore = 600;
        
        constexpr int PromotionScore[5] = {
            0,
            BasePromotionScore + 100,
            BasePromotionScore + 110,
            BasePromotionScore + 120,
            BasePromotionScore + 130
        };
        
        constexpr int KillerScore0 = 900;
        constexpr int KillerScore1 = 800;

        constexpr int CounterMoveScore = 90;
    }
}