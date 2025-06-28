#pragma once

#include "Globals.h"
#include "HashKeys.h"

// inline void HashPiece(Board* board, int piece, int square) {
//     board->hashKey ^= HashKeys::PieceKeys[piece][square];
// }

// inline void HashCastling(Board* board) {
//     board->hashKey ^= HashKeys::CastlingPermKeys[board->castlingPerms];
// }

// inline void HashEnPassant(Board* board) {
//     board->hashKey ^= HashKeys::EnPassantKeys[board->enPassant];
// }

// inline void HashSide(Board* board) {
//     board->hashKey ^= HashKeys::SideKey;
// }

namespace ErikEngine {
    template<bool updateHash = true>
    inline void RemovePiece(Board* board, int piece, int side, int square) {
        ClearBit(board->bitboards[piece], square);
        ClearBit(board->occupancy[side], square);
        if constexpr (updateHash) board->hashKey ^= HashKeys::PieceKeys[piece][square];
    }

    template<bool updateHash = true>
    inline void AddPiece(Board* board, int piece, int side, int square) {
        SetBit(board->bitboards[piece], square);
        SetBit(board->occupancy[side], square);
        if constexpr (updateHash) board->hashKey ^= HashKeys::PieceKeys[piece][square];
    }

    template<bool updateHash = true>
    inline void MovePiece(Board* board, int piece, int side, int fromSquare, int toSquare) {
        RemovePiece<updateHash>(board, piece, side, fromSquare);
        AddPiece<updateHash>(board, piece, side, toSquare);
    }

    // inline void RemovePiece(Board* board, int piece, int side, int square) {
    //     ClearBit(board->bitboards[piece], square);
    //     ClearBit(board->occupancy[side], square);
    //     board->hashKey ^= HashKeys::PieceKeys[piece][square];
    // }

    // inline void AddPiece(Board* board, int piece, int side, int square) {
    //     SetBit(board->bitboards[piece], square);
    //     SetBit(board->occupancy[side], square);
    //     board->hashKey ^= HashKeys::PieceKeys[piece][square];
    // }

    // inline void MovePiece(Board* board, int piece, int side, int fromSquare, int toSquare) {
    //     RemovePiece(board, piece, side, fromSquare);
    //     AddPiece(board, piece, side, toSquare);
    // }

    // inline void MovePieceNoHashing(Board* board, int piece, int side, int fromSquare, int toSquare) {
    //     ClearBit(board->bitboards[piece], fromSquare);
    //     ClearBit(board->occupancy[side], fromSquare);
    //     SetBit(board->bitboards[piece], toSquare);
    //     SetBit(board->occupancy[side], toSquare);
    // }
}