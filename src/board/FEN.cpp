#include <cmath>
#include <iostream>
#include <string.h>

#include "Board.h"
#include "HashKeys.h"

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

    fen += 2;
    fiftyMoveCount = atoi(fen);

    if (fiftyMoveCount < 0) {
        std::cout << "Invalid FEN string: invalid fifty move rule counter\n";
        Clear();
        return;
    }

    while (std::isdigit(*fen)) {
        fen++;
    }

    fullMoveCount = atoi(fen + 1);
    if (fullMoveCount < 0) fullMoveCount = 0;

    if (!IsValid()) {
        std::cout << "Invalid FEN string: failed validity check\n";
    }

    hashKey = HashKeys::GenerateHashKey(this);
}

// maximum length fen i can think of (not a legal position but idc)
// r1r1b1b1/n1n1k1q1/p1p1p1p1/p1p1p1p1/1P1P1P1P/1P1P1P1P/1Q1K1N1N/1B1B1R1R w KQkq a1 99 999
// i think 90 should be enough

constexpr size_t MAX_FEN_LENGTH = 90;

#define IncrementFile \
file++;\
if (file > FILE_H) {\
    file = FILE_A;\
    rank--;\
}

#define AddJump \
if (jump) {\
    buffer[index++] = '0' + jump;\
    jump = 0;\
}

const char* Board::GenerateFEN() {
    static char buffer[MAX_FEN_LENGTH];

    if (!IsValid()) {
        std::cout << "Position is invalid, FEN string will be invalid\n";
    }

    int file = FILE_A;
    int rank = RANK_8;

    int jump = 0;
    int index = 0;

    while (rank >= RANK_1) {
        if (file == FILE_A && rank < RANK_8) {
            AddJump
            buffer[index++] = '/';
        }

        int square = GetSquare(file, rank);

        if (!IsBitSet(occupancy[BOTH], square)) {
            jump++;
            IncrementFile
            continue;
        }

        AddJump

        int piece;

        for (piece = WP; piece <= BK; piece++) {
            if (IsBitSet(bitboards[piece], square)) break;
        }

        // there should be a piece set, so I will not include the condition
        // if (piece != NO_PIECE) {
            buffer[index++] = PIECE_CHAR[piece];
        // }
        // else {
        //     jump++;
        // }

        IncrementFile
    }

    AddJump

    buffer[index++] = ' ';
    buffer[index++] = SIDE_CHAR[side];
    buffer[index++] = ' ';

    if (castlingPerms) {
        for (int i = 0; i < 4; i++) {
            if (IsBitSet(castlingPerms, i)) {
                buffer[index++] = CASTLING_CHAR[i];
            }
        }
    }
    else {
        buffer[index++] = '-';
    }
    
    buffer[index++] = ' ';

    if (enPassant != NO_SQUARE) {
        buffer[index++] = 'a' + GetFile(enPassant);
        buffer[index++] = '1' + GetRank(enPassant);
    }
    else {
        buffer[index++] = '-';
    }

    // then add fifty move and full move counters

    int fullMoves = fullMoveCount + std::floor(ply / 2);

    // sizeof(char) == 1UL
    index += snprintf(buffer + index, MAX_FEN_LENGTH - index, " %i", fiftyMoveCount);
    index += snprintf(buffer + index, MAX_FEN_LENGTH - index, " %i", fullMoves);

    buffer[index] = '\0';

    return buffer;
}