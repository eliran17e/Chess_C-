//
// Created by User on 05/04/2025.
//

#include "../include/Rook.h"

Rook::Rook(bool isWhite, char symbol, int col, int row) : Piece(isWhite, symbol, col, row) {}

bool Rook::isValidMove( int toRow, int toCol) {
    // Rook can move only in straight lines: either in the same column or the same row
    return (getCol() == toCol || getRow() == toRow);
}
