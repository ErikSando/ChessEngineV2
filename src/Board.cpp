#include <iostream>
#include <string.h>

#include "Board.h"
#include "Globals.h"

Board::Board() {
    for (int piece = WP; piece <= BK; piece++) {
        m_bitboards[piece] = 0ULL;
    }

    ParseFEN(START_FEN);
}

void Board::Print() {
    for (int rank = Rank::Eight; rank >= Rank::One; rank--) {
        for (int file = File::A; file <= File::H; file++) {
            int square = GetSquare(file, rank);
            int piece = NO_PIECE;

            if (IsBitSet(m_occupancy[Both], square)) {
                for (int pce = WP; pce <= BK; pce++) {
                    if (IsBitSet(m_bitboards[pce], square)) {
                        piece = pce;
                        break;
                    }
                }
            }

            std::cout << " " << PIECE_CHAR[piece];
        }

        std::cout << "\n";
    }

    char castlingPerms[5] = { '-', '\0', '\0', '\0', '\0' };
    int i = 0;

    if (m_castlingPerms & Castling::WKC) castlingPerms[i++] = 'K';
    if (m_castlingPerms & Castling::WQC) castlingPerms[i++] = 'Q';
    if (m_castlingPerms & Castling::BKC) castlingPerms[i++] = 'k';
    if (m_castlingPerms & Castling::BQC) castlingPerms[i++] = 'q';

    std::cout << "Side to move: " << (m_side == White ? "white" : "black") << "\n";
    std::cout << "Castling permissions: " << castlingPerms << "\n";
    std::cout << "En passant: " << (m_enPassant != NO_SQUARE ? ToSquareString(m_enPassant) : "none") << "\n";
    std::cout << "Fifty move counter: " << m_fiftyMoveCounter << "\n";
}

bool Board::CheckDraw() {
    if (m_fiftyMoveCounter >= 50) return true;

    // check material draw
    // check three fold repetition

    return false;
}

void Board::GeneratePositionKey() {
    U64 key = 0ULL;



    m_key = key;
}

void Board::ParseFEN(const char* fen) {
    int file = File::A;
    int rank = Rank::Eight;
    // int numKings[2] = { 0, 0 };

    U64 bitboards[12]{};
    U64 occupancy[3]{};

    int fiftyMoveCounter;
    int enPassant = NO_SQUARE;
    int side;
    int castlingPerms = 0;

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
                SetBit(bitboards[piece], square);
                SetBit(occupancy[PIECE_SIDE[piece]], square);
            break;
        }
    }

    occupancy[Both] = occupancy[White] | occupancy[Black];

    // if (numKings[White] != 1 || numKings[Black] != 1) {
    //     std::cout << "Invalid FEN string: invalid number of king pieces\n";
    //     return;
    // }

    char sideChar = *ptr;

    switch (sideChar) {
        case 'w': side = White; break;
        case 'b': side = Black; break;

        default:
            std::cout << "Invalid FEN string: invalid side\n";
        return;
    }

    ptr += 2;

    for (int i = 0; i < 4; i++) {
        char c = *ptr++;
        if (c == ' ') break;

        switch (c) {
            case 'K': castlingPerms |= Castling::WKC; break;
            case 'Q': castlingPerms |= Castling::WQC; break;
            case 'k': castlingPerms |= Castling::BKC; break;
            case 'q': castlingPerms |= Castling::BQC; break;

            default:
                std::cout << "Invalid FEN string: invalid castling permissions\n";
            return;
        }
    }

    ptr++;

    if (*ptr != '-') {
        char enPassantFile = *ptr++ - 'a';
        char enPassantRank = *ptr++ - '1';

        enPassant = GetSquare(enPassantFile, enPassantRank);

        if (enPassant < 0 || enPassant >= 64) {
            std::cout << "Invalid FEN string: invalid en passant square\n";
            return;
        }
    }

    fiftyMoveCounter = atoi(ptr + 1);

    if (fiftyMoveCounter < 0) {
        std::cout << "Invalid FEN string: invalid fifty move rule counter\n";
        return;
    }

    m_fiftyMoveCounter = fiftyMoveCounter;
    m_enPassant = enPassant;
    m_castlingPerms = castlingPerms;
    m_side = side;
    GeneratePositionKey();
    memcpy(m_bitboards, bitboards, 12 * sizeof(U64));
    memcpy(m_occupancy, occupancy, 3 * sizeof(U64));
}

const char* GenerateFEN() {
    return "";
}