//
// Created by User on 05/04/2025.
//

#ifndef CHESS_KING_H
#define CHESS_KING_H
#include "Piece.h"
#include "Board.h"

class King: public Piece {
public:
    King(bool isWhite, char symbol, int col, int row);
    /**
     *  simple function that just checks king movement as if he was alone on the board
     * @param toRow
     * @param toCol
     * @return if the move is valid
     */
    bool isValidMove( int toRow, int toCol) const override;
    Piece* clone() const override { return new King(*this); }
    bool canAttack(int toRow, int toCol, const Board& board) const override;

};


#endif //CHESS_KING_H
