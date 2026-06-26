//
// Created by User on 05/04/2025.
//

#include "King.h"

King::King(bool isWhite, char symbol, int col, int row) : Piece(isWhite, symbol, col, row) {
    value = 1000.0;
}

bool King::isValidMove(int toRow, int toCol) const {
    return abs(getRow() - toRow) <= 1 && abs(getCol() - toCol) <= 1;
}
bool King::canAttack(int toRow, int toCol, const Board& /*board*/) const {
    int rowDiff = abs(getRow() - toRow);
    int colDiff = abs(getCol() - toCol);
    return rowDiff <= 1 && colDiff <= 1 && (rowDiff + colDiff > 0); // Must move at least one square
}