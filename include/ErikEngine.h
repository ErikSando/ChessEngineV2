#pragma once

#include "Attacks.h"
#include "CLI.h"
#include "Evaluation.h"
#include "HashKeys.h"

namespace ErikEngine {
    inline void Init() {
        Attacks::Init();
        Evaluation::Init();
        HashKeys::Init();
    }

    inline void Run() {
        CommandLoop();
    }
}