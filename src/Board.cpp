#include "Board.h"

Board::Board() {
    for (int piece = Piece::WP; piece <= Piece::BK; piece++) {
        bitboards[piece] = 0ULL;
    }
}

bool Board::CheckDraw() {
    return false;
}