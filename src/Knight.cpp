//
// Created by User on 05/04/2025.
//

#include "Knight.h"

Knight::Knight(bool isWhite, char symbol, int col, int row) : Piece(isWhite, symbol, col, row) {
    value = 3.0;
}

bool Knight::isValidMove(int toRow, int toCol) const {
    int rowDiff = abs(getRow() - toRow);
    int colDiff = abs(getCol() - toCol);
    return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}

bool Knight::canAttack(int toRow, int toCol, const Board& /*board*/) const {
    return isValidMove(toRow, toCol);
}
