//
// Created by User on 05/04/2025.
//

#ifndef CHESS_ROOK_H
#define CHESS_ROOK_H



#include "Piece.h"

class Rook : public Piece {
public:
    Rook(bool isWhite, char symbol, int col, int row);
    /**
     * checks the horizontal movement of the rook.
     * @param toRow
     * @param toCol
     * @return true if the movement is horizontal.
     */
    bool isValidMove( int toRow, int toCol) override;
};





#endif //CHESS_ROOK_H
