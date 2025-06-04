//
// Created by User on 05/04/2025.
//

#ifndef CHESS_QUEEN_H
#define CHESS_QUEEN_H
#include "Piece.h"

class Queen: public Piece{
public:
    Queen(bool isWhite, char symbol, int col, int row);
/**
 * isValid function checks diagonal and horizontal movement only
 * @param toRow(location of the queen)
 * @param toCol (location of the queen)
 * @return true if the square  it wants to move is horizontal or diagonal to the queen
 */
    bool isValidMove( int toRow, int toCol) const override;
    Piece* clone() const override { return new Queen(*this); }

};


#endif //CHESS_QUEEN_H
