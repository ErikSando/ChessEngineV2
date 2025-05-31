#include <iostream>

#include "Board.h"
#include "Globals.h"

Board::Board() {
    for (int piece = WP; piece <= BK; piece++) {
        bitboards[piece] = 0ULL;
    }

    InitAttackTables();

    ParseFEN(START_FEN);
}

void Board::Print() {
    for (int rank = Rank::Eight; rank >= Rank::One; rank--) {
        for (int file = File::A; file <= File::H; file++) {
            int square = GetSquare(file, rank);
            int piece = NO_PIECE;

            if (IsBitSet(occupancy[Both], square)) {
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

    char castlingPermsStr[5] = { '-', '\0', '\0', '\0', '\0' };
    int i = 0;

    if (castlingPerms & Castling::WKC) castlingPermsStr[i++] = 'K';
    if (castlingPerms & Castling::WQC) castlingPermsStr[i++] = 'Q';
    if (castlingPerms & Castling::BKC) castlingPermsStr[i++] = 'k';
    if (castlingPerms & Castling::BQC) castlingPermsStr[i++] = 'q';

    std::cout << "Side to move: " << (side == White ? "white" : "black") << "\n";
    std::cout << "Castling permissions: " << castlingPermsStr << "\n";
    std::cout << "En passant: " << (enPassant != NO_SQUARE ? ToSquareString(enPassant) : "none") << "\n";
    std::cout << "Fifty move counter: " << fiftyMoveCounter << "\n";
}

bool Board::CheckDraw() {
    if (fiftyMoveCounter >= 50) return true;

    // check material draw
    // check three fold repetition

    return false;
}

void Board::GeneratePositionKey() {
    U64 key = 0ULL;



    positionKey = key;
}