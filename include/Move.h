#pragma once

#include "Globals.h"
#include "HashKeys.h"

enum MoveFlags {
    CAPTURE_FLAG = 0x1000000,
    PAWNSTART_FLAG = 0x2000000,
    CASTLING_FLAG = 0x4000000,
    ENPASSANT_FLAG = 0x8000000
};

inline int GetFromSquare(int move) {
    return move & 0x3F;
}

inline int GetToSquare(int move) {
    return (move & 0xFC0) >> 6;
}

inline int GetMovedPiece(int move) {
    return (move & 0xF000) >> 12;
}

inline int GetCapturedPiece(int move) {
    return (move & 0xF0000) >> 16;
}

inline int GetPromotedPiece(int move) {
    return (move & 0xF00000) >> 20;
}

inline int GetFlag(int move) {
    return move & 0xF000000;
}