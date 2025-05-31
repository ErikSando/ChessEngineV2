#include <iostream>
#include <string.h>

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

void Board::ParseFEN(const char* fen) {
    int file = File::A;
    int rank = Rank::Eight;
    // int numKings[2] = { 0, 0 };

    U64 new_bitboards[12]{};
    U64 new_occupancy[3]{};

    int new_fiftyMoveCounter;
    int new_enPassant = NO_SQUARE;
    int new_side;
    int new_castlingPerms = 0;

    const char* ptr = fen;

    for (;;) {
        char c = *ptr++; // chracters are one byte, so the memory address can be incremented by 1 to get the next character
        if (c == ' ') break;

        if (rank < 0) {
            std::cout << "Invalid FEN string: invalid piece positions\n";
            return;
        }

        int square = GetSquare(file, rank);

        file++;

        if (file > File::H) {
            file = File::A;
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
                
                if (file > File::H) {
                    file = File::A;
                    rank--;
                }

                if (rank < 0) {
                    std::cout << "Invalid FEN string: invalid piece positions\n";
                    return;
                }
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

    new_occupancy[Both] = new_occupancy[White] | new_occupancy[Black];

    // if (numKings[White] != 1 || numKings[Black] != 1) {
    //     std::cout << "Invalid FEN string: invalid number of king pieces\n";
    //     return;
    // }

    char sideChar = *ptr;

    switch (sideChar) {
        case 'w': new_side = White; break;
        case 'b': new_side = Black; break;

        default:
            std::cout << "Invalid FEN string: invalid side\n";
        return;
    }

    ptr += 2;

    for (int i = 0; i < 4; i++) {
        char c = *ptr++;
        if (c == ' ') break;

        switch (c) {
            case 'K': new_castlingPerms |= Castling::WKC; break;
            case 'Q': new_castlingPerms |= Castling::WQC; break;
            case 'k': new_castlingPerms |= Castling::BKC; break;
            case 'q': new_castlingPerms |= Castling::BQC; break;

            default:
                std::cout << "Invalid FEN string: invalid castling permissions\n";
            return;
        }
    }

    ptr++;

    if (*ptr != '-') {
        char enPassantFile = *ptr++ - 'a';
        char enPassantRank = *ptr++ - '1';

        new_enPassant = GetSquare(enPassantFile, enPassantRank);

        if (new_enPassant < 0 || new_enPassant >= 64) {
            std::cout << "Invalid FEN string: invalid en passant square\n";
            return;
        }
    }

    new_fiftyMoveCounter = atoi(ptr + 1);

    if (new_fiftyMoveCounter < 0) {
        std::cout << "Invalid FEN string: invalid fifty move rule counter\n";
        return;
    }

    fiftyMoveCounter = new_fiftyMoveCounter;
    enPassant = new_enPassant;
    castlingPerms = new_castlingPerms;
    side = new_side;
    GeneratePositionKey();
    memcpy(bitboards, new_bitboards, 12 * sizeof(U64));
    memcpy(occupancy, new_occupancy, 3 * sizeof(U64));
}

const char* GenerateFEN() {
    return "";
}