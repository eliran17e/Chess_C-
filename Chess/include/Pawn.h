//
// Created by User on 05/04/2025.
//

#ifndef CHESS_PAWN_H
#define CHESS_PAWN_H
#include "Piece.h"

class Pawn: public Piece {
public:
    Pawn(bool isWhite, char symbol, int col, int row);
    /**
     *  check the pawn movement as if he was alone in the board
     * @param toRow
     * @param toCol
     * @return if the move is valid
     */
    bool isValidMove( int toRow, int toCol) const override;
    bool canAttack(int toRow, int toCol) const override;
    Piece* clone() const override { return new Pawn(*this); }
};


#endif //CHESS_PAWN_H
