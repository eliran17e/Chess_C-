//
// Created by User on 05/04/2025.
//

#include "Bishop.h"

Bishop::Bishop(bool isWhite, char symbol, int col, int row) : Piece(isWhite, symbol, col, row) {
    value = 3.0;
}

bool Bishop::isValidMove(int toRow, int toCol) const {
    return abs(getRow() - toRow) == abs(getCol() - toCol);
}
bool Bishop::canAttack(int toRow, int toCol, const Board& board) const {
    if (abs(getRow() - toRow) == abs(getCol() - toCol)) { // Check if diagonal move
        return board.isPathClear(getRow(), getCol(), toRow, toCol); // Check path clearance
    }
    return false;
}