#include <iostream>

#include "Attacks.h"
#include "Board.h"
#include "Globals.h"
#include "Utils.h"

Board::Board() {
    for (int piece = WP; piece <= BK; piece++) {
        bitboards[piece] = 0ULL;
    }

    ParseFEN(START_FEN);
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

    std::cout << "Side to move:       " << (side == WHITE ? "white" : "black") << "\n";
    std::cout << "Castling:           " << castlingPermsStr << "\n";
    std::cout << "En passant:         " << (enPassant != NO_SQUARE ? Utils::ToSquareString(enPassant) : "none") << "\n";
    std::cout << "50 move counter:    " << fiftyMoveCount << "\n";
    std::cout << "Hash key:           " << std::hex << hashKey << std::dec << "\n";
}

bool Board::IsSquareAttacked(int square, int side) const {
    int enemy = side ^ 1;
    int piece = enemy * 6;

    U64 bishopAttacks = Attacks::GetBishopAttacks(square, occupancy[BOTH]);
    U64 rookAttacks = Attacks::GetRookAttacks(square, occupancy[BOTH]);

    if ((bishopAttacks | rookAttacks) & bitboards[piece + 4]) return true;
    if (bishopAttacks & bitboards[piece + 2]) return true;
    if (rookAttacks & bitboards[piece + 3]) return true;

    if (Attacks::KnightAttacks[square] & bitboards[piece + 1]) return true;
    if (Attacks::PawnCaptures[side][square] & bitboards[piece]) return true;
    if (Attacks::KingAttacks[square] & bitboards[piece + 5]) return true;

    return false;
}

// bool Board::IsSquareAttacked(int square) const {
//     return IsSquareAttacked(square, side);
// }