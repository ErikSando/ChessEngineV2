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
        std::cout << " " << (rank + 1);

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

    std::cout << "   A B C D E F G H\n";

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


bool Board::IsSquareAttacked(int square, int side) {
    int enemy = side ^ 1;
    int piece = enemy * 6;

    U64 squareMask = GetSquareMask(square);

    // if (pawnCaptures[enemy][square] & bitboards[piece]) return true;
    // if (knightAttacks[square] & bitboards[piece + 1]) return true;
    // if (GetBishopAttacks(square, occupancy[Both]) & bitboards[piece + 2]) return true;
    // if (GetRookAttacks(square, occupancy[Both]) & bitboards[piece + 3]) return true;
    // if (GetQueenAttacks(square, occupancy[Both]) & bitboards[piece + 4]) return true;

    if (pawnCaptures[enemy][square] & bitboards[piece]) return true;
    if (knightAttacks[square] & bitboards[piece + 1]) return true;
    U64 bishopAttacks = GetBishopAttacks(square, occupancy[Both]);
    if (bishopAttacks & bitboards[piece + 2]) return true;
    U64 rookAttacks = GetRookAttacks(square, occupancy[Both]);
    if (rookAttacks & bitboards[piece + 3]) return true;
    if ((bishopAttacks | rookAttacks) & bitboards[piece + 4]) return true;

    return false;
}

// bool Board::IsSquaresAttacked(U64 squaresMask, int side) {
//     int enemy = side ^ 1;
//     int piece = side * 6;

//     return (pawnCaptures[enemy][square] & squaresMask) | ;

//     return false;
// }

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