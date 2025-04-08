//
// Created by User on 05/04/2025.
//

#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H
#include "Piece.h"
#include <vector>
#include "Pawn.h"

/**
 * The Board class represents a chessboard and manages the game state,
 * including move validation, turn management,
 * and enforcing some of the chess rules like check or capture.
 */

class Board {
private:
    std::vector<std::vector<Piece*>> chessBoard;
    bool whiteTurn;

    // Helper methods
    /**
 *
 * @param fromRow (current piece position)
 * @param fromCol (current piece position)
 * @param toRow (where the piece wants to move)
 * @param toCol (where the piece wants to move)
 * @return boolean based if the path is clear
 */
    bool isPathClear(int fromRow, int fromCol, int toRow, int toCol);
    /**
 *Checks if the king would be in check after the move
 * @param fromRow (current piece position)
 * @param fromCol (current piece position)
 * @param toRow (where the piece wants to move)
 * @param toCol (where the piece wants to move)
 * @Returns true if the king would be in check after the move, otherwise false.
 */
    bool wouldBeInCheck(int fromRow, int fromCol, int toRow, int toCol);
    /**
 *
 * @param fromRow (current piece position)
 * @param fromCol (current piece position)
 * @param toRow (where the piece wants to move)
 * @param toCol (where the piece wants to move)
 * @return true if the move would cause check on the king
 */
    bool wouldCauseCheck(int fromRow, int fromCol, int toRow, int toCol);
    /**
    *

    * @param toRow (where the piece wants to move)
    * @param toCol (where the piece wants to move)
    * @param byWhite color of the piece
    * @return true if the square we want to go to is under attack
    */
    bool isSquareUnderAttack(int row, int col, bool byWhite);
    /**
    *
    * @param isWhite
    * @return the king current position
    */
    std::pair<int, int> findKing(bool isWhite);

public:
    /**
 *
 * @param board
 * builds the chess board with a 8x8 matrix vector.
 */
    explicit Board(const std::string& board);
    /**
 *  Destructor
 */
    ~Board();

    // Main methods
    /**
 *
 * @param command
 * @return a number based on the code response of chess class
 */
    int checkMove(const std::string& command);
    /**
 *
 * @param fromRow (current piece position)
 * @param fromCol (current piece position)
 * @param toRow (where the piece wants to move)
 * @param toCol (where the piece wants to move)
 * @return a number based on the code response of chess class
 */
    int isLegalMove(int fromRow, int fromCol, int toRow, int toCol);
/**
 *  makes a chess move
 * @param fromRow (current piece position)
 * @param fromCol (current piece position)
 * @param toRow (where the piece wants to move)
 * @param toCol (where the piece wants to move)
 */
    void makeMove(int fromRow, int fromCol, int toRow, int toCol);

};

#endif //CHESS_BOARD_H
