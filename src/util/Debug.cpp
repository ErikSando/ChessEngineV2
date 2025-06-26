#include <string_view>

#include "Debug.h"

namespace ErikEngine {
    const char* FILE_NAME(std::string_view path) {
        /* ./src/.../name.cpp
                ^ want here onwards */
        size_t src_slash = find_nth_occurence(2, "/\\", path);
        return src_slash == std::string_view::npos ? path.data() : path.substr(src_slash + 1).data();
    }
}