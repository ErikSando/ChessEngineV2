#include <iostream>

#include "Attacks.h"
#include "CLI.h"
#include "Evaluation.h"
#include "HashKeys.h"

int main() {
    Attacks::Init();
    Evaluation::Init();
    HashKeys::Init();

    CommandLoop();

    // get mvv lva values

    

    return 0;
}