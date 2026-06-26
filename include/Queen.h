//
// Created by User on 05/04/2025.
//

#ifndef CHESS_QUEEN_H
#define CHESS_QUEEN_H
#include "Piece.h"
#include "Board.h"
class Queen: public Piece{
public:
    Queen(bool isWhite, char symbol, int col, int row);
/**
 * @return true if the target square is reachable by queen movement (rank, file, or diagonal)
 */
    bool isValidMove( int toRow, int toCol) const override;
    Piece* clone() const override { return new Queen(*this); }
    bool canAttack(int toRow, int toCol, const Board& board) const override;

};


#endif //CHESS_QUEEN_H
