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

    return 0;
}