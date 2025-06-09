#include <iostream>

#include "Attacks.h"
#include "CLI.h"
#include "Evaluation.h"
#include "HashKeys.h"
#include "Searcher.h"

int main(int argc, char** argv) {
    Attacks::Init();
    Evaluation::Init();
    HashKeys::Init();
    Searcher::Init();

    CommandLoop();

    return 0;
}