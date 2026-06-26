//
// Created by User on 05/04/2025.
//

#ifndef CHESS_PIECE_H
#define CHESS_PIECE_H

#include <iostream>
#include <string>

// Forward declaration of Board to avoid circular dependency,
// since Piece methods will need to access Board.
class Board; // This line is crucial!

/**
 * The Piece class is an abstract base class representing a generic chess piece.
 * It provides common attributes and methods for all chess pieces.
 */
class Piece {
protected:
    bool isWhite;
    char symbol;
    int col;
    int row;
    double value;

public:
    /**
     * @param isWhite (color of the piece)
     * @param symbol (which piece is it)
     * @param col (location of the piece)
     * @param row (location of the piece)
     */
    Piece(bool isWhite, char symbol, int col, int row);

    /**
     * clone function for all the pieces
     */
    virtual Piece* clone() const = 0;

    /**
     * Virtual destructor for proper polymorphic deletion.
     */
    virtual ~Piece() = default;

    // Copy constructor and assignment operator (ensure deep copy if Piece has dynamic members,
    // though here they might be fine with default if only primitive members)
    Piece(const Piece& other);
    Piece& operator=(const Piece& other);

    char getSymbol() const;
    bool getIsWhite() const;
    int getCol() const;
    int getRow() const;
    void setCol(int col);
    void setRow(int row);
    virtual double getValue() const;

    /**
     * Checks if the piece can geometrically move to a target square.
     * This typically does NOT consider path obstructions or king safety.
     * It's about the piece's pattern.
     */
    virtual bool isValidMove(int toRow, int toCol) const = 0;

    /**
     * Checks if this piece can *attack* a target square.
     * This includes considering the piece's movement pattern and any obstructions
     * that prevent it from reaching the square (for sliding pieces).
     * It requires a Board reference to check for obstructions.
     * This is a pure virtual function, meaning each derived class must implement it.
     */
    virtual bool canAttack(int toRow, int toCol, const Board& board) const = 0;
};

#endif //CHESS_PIECE_H