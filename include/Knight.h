//
// Created by User on 05/04/2025.
//

#ifndef CHESS_KNIGHT_H
#define CHESS_KNIGHT_H
#include "Piece.h"
#include "Board.h"
class Knight: public Piece {
public:
    Knight(bool isWhite, char symbol, int col, int row);
    /**
     *  function checks the knight movement as if he was alone in the board
     * @param toRow
     * @param toCol
     * @return if the move is valid
     */
    bool isValidMove( int toRow, int toCol) const override;
    Piece* clone() const override { return new Knight(*this); }
    bool canAttack(int toRow, int toCol, const Board& board) const override;

};


#endif //CHESS_KNIGHT_H
