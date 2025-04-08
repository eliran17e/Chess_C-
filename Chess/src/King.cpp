//
// Created by User on 05/04/2025.
//

#include "../include/King.h"
King::King(bool isWhite, char symbol, int col, int row) : Piece(isWhite, symbol, col, row) {}

bool King::isValidMove( int toRow, int toCol) {
    if (abs(getRow() - toRow) <=1 && abs(getCol() - toCol) <=1) {
        return true;
    }

    return false;
}