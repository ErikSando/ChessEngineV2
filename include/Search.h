#pragma once

#ifdef _WIN32
    #include <Windows.h>
#elif defined(__APPLE__)
    #include <mach/mach_time.h>
#else
    #include <time.h>
#endif

constexpr int MAX_DEPTH = 64;

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

struct SearchInfo {
    int depth = MAX_DEPTH;
    int startTime;
    int stopTime;
    bool timeSet = false;
};