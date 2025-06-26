#pragma once

// #include "Board.h"

#ifdef NDEBUG
    #define EE_DEBUG(x)
    #define EE_ASSERT(expr)
#else
    #include <cstdio>
    #include <cstdlib>

    #define EE_DEBUG(x) do { x; } while(0)
    #define EE_ASSERT(expr)\
        do {\
            if (!(expr)) {\
                std::fprintf(stderr, "Assert failed: %s\nFile %s, line %d\n", #expr, FILE_NAME(__FILE__), __LINE__);\
                std::abort();\
            }\
        } while (0)
#endif

namespace ErikEngine {
    constexpr size_t npos = std::string_view::npos;

    template<typename T>
    size_t find_nth_occurence(const unsigned int n, T s, const std::string_view& sv) {
        size_t pos = sv.find_first_of(s);
        if (pos == npos) return npos;

        for (int i = 0; i < n - 1; i++) {
            size_t jump = sv.substr(pos + 1).find_first_of(s);
            if (jump == npos) return npos;
            pos += jump + 1;
        }

        return pos;
    }

    extern const char* FILE_NAME(std::string_view path);

    // namespace Debug {
    //     void CheckBoardValid(Board& board);
    //     bool IsBoardValid(Board& board);
    // }
}