#include "MoveScoring.h"

namespace MoveScoring {
    int KillerMoves[2][64];
    int HistoryMoves[12][64];
    int CounterMoves[64][64];

    void Reset() {
        memset(KillerMoves, 0, sizeof(KillerMoves));
        memset(HistoryMoves, 0, sizeof(HistoryMoves));
    }
}