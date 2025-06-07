#pragma once

#include <iostream>

#include "Board.h"
#include "Globals.h"
#include "Searcher.h"

#ifdef _WIN32
    #include <Windows.h>
#elif defined(__APPLE__)
    #include <mach/mach_time.h>
    #include <sys/select.h>
    #include <unistd.h>
#else
    #include <time.h>
#endif

namespace Utils {
    bool ParseMove(Board& board, const int fromSquare, const int toSquare, const char promoted);
    bool ParseMove(Board& board, const char* move);
    bool ParseMove(Board& board, const std::string& move);

    int ToSquare(const char* square_string);
    int ToSquare(std::string& square_string);

    std::string ToSquareString(int square);
    std::string ToMoveString(int move);

    void PrintBitboard(U64 bitboard);

    inline int GetTimeMS() {
        #ifdef _WIN32
            return static_cast<int>(GetTickCount());
        #elif defined(__APPLE__)
            static mach_timebase_info_data_t timebase = [] {
                mach_timebase_info_data_t tb;
                mach_timebase_info(&tb);
                return tb;
            }();
    
            uint64_t now = mach_absolute_time();
            uint64_t ns = now * timebase.numer / timebase.denom;
            return static_cast<int>(ns / 1000000);
        #else
            timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            return static_cast<int>(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
        #endif
    }
}