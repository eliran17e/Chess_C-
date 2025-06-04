//
// Created by User on 05/04/2025.
//

#ifndef CHESS_PIECE_H
#define CHESS_PIECE_H
#include <iostream>
#include <string>
using namespace std;
/**
 *  The Piece class is an abstract base class representing a generic chess piece.
 *  It provides common attributes and methods for all chess pieces
 */

class Piece {
protected:
    char symbol;
    bool isWhite;
    int col;
    int row;
    int value;

public:
    /**
     *
     * @param isWhite(color of the piece)
     * @param symbol (which piece is it)
     * @param col (location of the piece)
     * @param row (location of the piece)
     */
    Piece(bool isWhite,char symbol,int col,int row);
    /**
     *
     * clone function for all the pieces
     */
    virtual Piece* clone() const = 0;
    virtual ~Piece() = default;
    Piece(const Piece& other);
    Piece& operator=(const Piece& other);
    char getSymbol() const ;
    bool getIsWhite() const ;
    int getCol() const ;
    int getRow() const ;
    void setCol(int col);
    void setRow(int row);
    virtual bool isValidMove(int toRow, int toCol) const = 0;
    int getValue() const ;
    virtual bool canAttack(int toRow, int toCol) const {
        return isValidMove(toRow, toCol);
    }

};


#endif //CHESS_PIECE_H
