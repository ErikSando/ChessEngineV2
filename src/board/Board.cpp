#include <iostream>
#include <string.h>

#include "Attacks.h"
#include "Board.h"
#include "Globals.h"
#include "Utils.h"

Board::Board() {
    Clear();
}

void Board::Clear() {
    memset(bitboards, 0, sizeof(bitboards));
    memset(occupancy, 0, sizeof(bitboards));
    memset(history, 0, sizeof(history));
    memset(bigPieces, 0, sizeof(bigPieces));

    hashKey = 0ULL;
    side = BOTH;
    enPassant = NO_SQUARE;
    castlingPerms = 0;
    fiftyMoveCount = 0;
    ply = 0;
}

void Board::Print() {
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        std::cout << " " << (rank + 1) << "  ";

        for (int file = FILE_A; file <= FILE_H; file++) {
            int square = GetSquare(file, rank);
            int piece = NO_PIECE;

            if (IsBitSet(occupancy[BOTH], square)) {
                for (int pce = WP; pce <= BK; pce++) {
                    if (IsBitSet(bitboards[pce], square)) {
                        piece = pce;
                        break;
                    }
                }
            }

            std::cout << " " << PIECE_CHAR[piece];
        }

        std::cout << "\n";
    }

    std::cout << "\n     a b c d e f g h\n";

    std::string castlingPermsStr = "----";

    if (castlingPerms & WKC) castlingPermsStr[0] = 'K';
    if (castlingPerms & WQC) castlingPermsStr[1] = 'Q';
    if (castlingPerms & BKC) castlingPermsStr[2] = 'k';
    if (castlingPerms & BQC) castlingPermsStr[3] = 'q';

    std::cout << "Side to move:        " << SIDE_CHAR[side] << "\n";
    std::cout << "Castling:            " << castlingPermsStr << "\n";
    std::cout << "En passant:          " << (enPassant != NO_SQUARE ? Utils::ToSquareString(enPassant) : "none") << "\n";
    std::cout << "50 move counter:     " << fiftyMoveCount << "\n";
    std::cout << "Hash key:            " << std::hex << hashKey << std::dec << "\n";
}