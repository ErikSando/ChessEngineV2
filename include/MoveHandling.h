#pragma once

#include "Globals.h"

inline void HashPiece(U64& key, int piece, int square) {
    key ^= PieceKeys[piece][square];
}

inline void HashCastling(U64& key, int castlingPerms) {
    key ^= CastlingKeys[castlingPerms];
}

inline void HashEnPassant(U64& key, int enPassant) {
    key ^= EnPassantKeys[enPassant];
}

inline void HashSide(U64& key) {
    key ^= SideKey;
}

inline void MoveRook(U64& hashKey, U64& bitboard, U64& occupancy, int rook, int fromSquare, int toSquare) {
    HashPiece(hashKey, rook, fromSquare);
    ClearBit(bitboard, fromSquare);
    ClearBit(occupancy, fromSquare);
    SetBit(bitboard, toSquare);
    SetBit(occupancy, toSquare);
    HashPiece(hashKey, rook, toSquare);
}