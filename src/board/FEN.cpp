#include <iostream>
#include <string.h>

#include "Board.h"
#include "HashKeys.h"

void Board::ParseFEN(const char* fen) {
    int file = FILE_A;
    int rank = RANK_8;
    // int numKings[2] = { 0, 0 };

    U64 new_bitboards[12]{};
    U64 new_occupancy[3]{};

    int new_side;
    int new_castlingPerms = 0;
    int new_fiftyMoveCount;
    int new_enPassant = NO_SQUARE;

    const char* ptr = fen;

    for (;;) {
        char c = *ptr++; // chracters are one byte, so the memory address can be incremented by 1 to get the next character
        if (c == ' ') break;

        // if (rank < Rank1) {
        //     std::cout << "Invalid FEN string: invalid piece positions\n";
        //     return;
        // }

        int square = GetSquare(file, rank);

        file++;

        if (file > FILE_H) {
            file = FILE_A;
            rank--;
        }

        switch (c) {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                file += c - '1'; // file is always incremented, so here it is incremented by 1 less than the jump
                
                if (file > FILE_H) {
                    file = FILE_A;
                    rank--;
                }

                // if (rank < Rank1) {
                //     std::cout << "Invalid FEN string: invalid piece positions\n";
                //     return;
                // }
            break;

            case '/':
                file--;
            break;

            default:
                std::cout << "Invalid FEN string: invalid piece\n";
            return;

            case 'P':
            case 'N':
            case 'B':
            case 'R':
            case 'Q':
            case 'K':
            case 'p':
            case 'n':
            case 'b':
            case 'r':
            case 'q':
            case 'k':
                int piece = PieceID(c);
                SetBit(new_bitboards[piece], square);
                SetBit(new_occupancy[PIECE_SIDE[piece]], square);
            break;
        }
    }

    new_occupancy[BOTH] = new_occupancy[WHITE] | new_occupancy[BLACK];

    // if (numKings[White] != 1 || numKings[Black] != 1) {
    //     std::cout << "Invalid FEN string: invalid number of king pieces\n";
    //     return;
    // }

    char sideChar = *ptr;

    switch (sideChar) {
        case 'w': new_side = WHITE; break;
        case 'b': new_side = BLACK; break;

        default:
            std::cout << "Invalid FEN string: invalid side\n";
        return;
    }

    ptr += 2;

    for (int i = 0; i < 4; i++) {
        char c = *ptr;
        
        if (c == ' ') break;

        if (c == '-') {
            ptr++;
            break;
        }

        ptr++;

        switch (c) {
            case 'K': new_castlingPerms |= WKC; break;
            case 'Q': new_castlingPerms |= WQC; break;
            case 'k': new_castlingPerms |= BKC; break;
            case 'q': new_castlingPerms |= BQC; break;

            default:
                std::cout << "Invalid FEN string: invalid castling permissions\n";
            return;
        }
    }

    ptr++;

    if (*ptr != '-') {
        char enPassantFile = *ptr++ - 'a';
        char enPassantRank = *ptr++ - '1';

        std::cout << enPassantFile << enPassantRank << "\n";

        new_enPassant = GetSquare(enPassantFile, enPassantRank);

        if (new_enPassant < 0 || new_enPassant >= 64) {
            std::cout << "Invalid FEN string: invalid en passant square\n";
            return;
        }
    }

    new_fiftyMoveCount = atoi(ptr + 1);

    if (new_fiftyMoveCount < 0) {
        std::cout << "Invalid FEN string: invalid fifty move rule counter\n";
        return;
    }

    fiftyMoveCount = new_fiftyMoveCount;
    enPassant = new_enPassant;
    castlingPerms = new_castlingPerms;
    side = new_side;
    memcpy(bitboards, new_bitboards, 12 * sizeof(U64));
    memcpy(occupancy, new_occupancy, 3 * sizeof(U64));

    if (!CheckValidQuiet()) {
        std::cout << "Invalid FEN string: failed validity check\n";
    }

    HashKeys::GenerateHashKey(this);
}

const char* GenerateFEN() {
    return "";
}