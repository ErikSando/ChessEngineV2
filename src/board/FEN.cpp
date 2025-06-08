#include <iostream>
#include <string.h>

#include "Board.h"
#include "HashKeys.h"

#include "Utils.h"

void Board::ParseFEN(const char* fen) {
    Clear();

    int file = FILE_A;
    int rank = RANK_8;

    while (*fen && rank >= RANK_1) {
        int piece = NO_PIECE;
        int jump = 1;

        char c = *fen;

        fen++;

        switch (c) {
            case 'P': case 'N': case 'B': case 'R': case 'Q': case 'K':
            case 'p': case 'n': case 'b': case 'r': case 'q': case 'k':
                piece = PieceID(c);
            break;

            case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
            jump = c - '0';
            break;

            case '/':
                file = FILE_A;
                jump = 0;
                rank--;
            break;

            case ' ': rank = -1; break;

            default:
                std::cout << "Invalid FEN string: invalid character '" << c << "'\n";
                Clear();
            return;
        }

        if (piece != NO_PIECE) {
            int square = GetSquare(file, rank);
            int pside = PIECE_SIDE[piece];

            SetBit(bitboards[piece], square);
            SetBit(occupancy[pside], square);

            if (IS_PIECE_BIG[piece]) bigPieces[pside]++;
        }

        file += jump;
    }

    occupancy[BOTH] = occupancy[WHITE] | occupancy[BLACK];

    // if (numKings[White] != 1 || numKings[Black] != 1) {
    //     std::cout << "Invalid FEN string: invalid number of king pieces\n";
    //     return;
    // }

    switch (*fen) {
        case 'w': side = WHITE; break;
        case 'b': side = BLACK; break;

        default:
            std::cout << "Invalid FEN string: invalid side\n";
            Clear();
        return;
    }

    fen += 2;

    for (int i = 0; i < 4; i++) {
        char c = *fen;
        
        if (c == ' ') break;

        if (c == '-') {
            fen++;
            break;
        }

        fen++;

        switch (c) {
            case 'K': castlingPerms |= WKC; break;
            case 'Q': castlingPerms |= WQC; break;
            case 'k': castlingPerms |= BKC; break;
            case 'q': castlingPerms |= BQC; break;

            default:
                std::cout << "Invalid FEN string: invalid castling permissions\n";
                Clear();
            return;
        }
    }

    fen++;

    if (*fen != '-') {
        char enPassantFile = *fen++ - 'a';
        char enPassantRank = *fen++ - '1';

        enPassant = GetSquare(enPassantFile, enPassantRank);

        if (enPassant < a1 || enPassant > h8) {
            std::cout << "Invalid FEN string: invalid en passant square\n";
            Clear();
            return;
        }
    }

    fiftyMoveCount = atoi(fen + 1);

    if (fiftyMoveCount < 0) {
        std::cout << "Invalid FEN string: invalid fifty move rule counter\n";
        Clear();
        return;
    }

    #ifdef NDEBUG
        if (!CheckValidQuiet()) {
            std::cout << "Invalid FEN string: failed validity check\n";
        }
    #else
        CheckValid();
    #endif

    hashKey = HashKeys::GenerateHashKey(this);
}

const char* GenerateFEN() {
    return "";
}