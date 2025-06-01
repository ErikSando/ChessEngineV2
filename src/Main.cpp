#include <iostream>

#include "CLI.h"

int main() {
    // int repeat = 200000000;
    // int start = GetTimeMS();

    // for (int i = 0; i < repeat - 2; i++) {
    //     GetTimeMS();
    // }

    // int end = GetTimeMS();
    // std::cout << "Called GetTimeMS() " << repeat << " times in " << (end - start) << " ms.\n";

    CommandLoop();

    return 0;
}
