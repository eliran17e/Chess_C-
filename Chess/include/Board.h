//
// Created by User on 05/04/2025.
//

#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H
#include "Piece.h"
#include <vector>
#include <ostream>
#include <map>
/**
 * Move struct to implement evalatuion for the moves.
 */
struct Move {
    std::string notation;
    double score = -99995.0;
    int fromRow, fromCol;
    int toRow, toCol;
    bool isItWhite;
    bool operator<(const Move& other) const {
        return score < other.score;
    }

    bool operator>(const Move& other) const {
        return score > other.score;
    }
};
/**
 * Struct to check castling right incase of a rook or a king movement.
 */
struct CastlingRights {
    bool kingMoved = false;
    bool leftRookMoved = false;
    bool rightRookMoved = false;
};

std::ostream& operator<<(std::ostream& os, const Move& m);
/**
 * The Board class represents a chessboard and manages the game state,
 * including move validation, turn management,
 * and enforcing some of the chess rules like check or capture.
 */

class Board {
private:
    std::map<std::string, int> positionCounts;
    std::vector<struct Move> currentMoves;
    std::vector<std::vector<Piece*>> chessBoard;
    bool whiteTurn;
    bool repetitionDraw;
    int countMoves;
    CastlingRights whiteCastling;
    CastlingRights blackCastling;



public:
    /**
 *
 * @param board
 * builds the chess board with a 8x8 matrix vector.
 */
    explicit Board(const std::string& board);
    /**
     * Copy constructor
     */
    Board(const Board& other);
    /**
     * Assignment operator
    */
    Board& operator=(const Board& other);
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
    int checkMove(const std::string& command,bool isAI = false);
    /**
 *
 * @param fromRow (current piece position)
 * @param fromCol (current piece position)
 * @param toRow (where the piece wants to move)
 * @param toCol (where the piece wants to move)
 * @return a number based on the code response of chess class
 */
    int isLegalMove(int fromRow, int fromCol, int toRow, int toCol) const;
/**
 *  makes a chess move
 * @param fromRow (current piece position)
 * @param fromCol (current piece position)
 * @param toRow (where the piece wants to move)
 * @param toCol (where the piece wants to move)
 */
    void makeMove(int fromRow, int fromCol, int toRow, int toCol,bool isAI = false);
    /**
     * Gets all the valid moves and creates a vector of all the moves.
     * @param turn
     * @return
     */
    std::vector<Move> getAllValidMoves(bool turn) const;
    /**
     * Evaluate all the moves based on depth that the user choose.
     * @param depth
     */
    void evaluateAllMoves(int depth);

    /**
     *
     * @return a string that represents the current board state
     */
    std::string toString() const;
    /**
     * Evaluate a chess move and giving it a score.
     * @param move
     * @param depth
     * @return
     */
    double evaluateMove(const Move& move, int depth) const;
    bool isWhiteTurn() const ;
    /**
     * Simple boolean function to check if the game in checkmate position.
     */
    bool isCheckmate() const ;
    /**
     * boolean function to check if any of the stalemate conditions happens.
     */
    bool isStalemate() const;
    /**
*
* @param fromRow (current piece position)
* @param fromCol (current piece position)
* @param toRow (where the piece wants to move)
* @param toCol (where the piece wants to move)
* @return boolean based if the path is clear
*/
    bool isPathClear(int fromRow, int fromCol, int toRow, int toCol) const;
    /**
 *Checks if the king would be in check after the move
 * @param fromRow (current piece position)
 * @param fromCol (current piece position)
 * @param toRow (where the piece wants to move)
 * @param toCol (where the piece wants to move)
 * @Returns true if the king would be in check after the move, otherwise false.
 */
    bool wouldBeInCheck(int fromRow, int fromCol, int toRow, int toCol) const;
    /**
 *
 * @param fromRow (current piece position)
 * @param fromCol (current piece position)
 * @param toRow (where the piece wants to move)
 * @param toCol (where the piece wants to move)
 * @return true if the move would cause check on the king
 */
    bool wouldCauseCheck(int fromRow, int fromCol, int toRow, int toCol) const;
    /**
    *

    * @param toRow (where the piece wants to move)
    * @param toCol (where the piece wants to move)
    * @param byWhite color of the piece
    * @return true if the square we want to go to is under attack
    */
    bool isSquareUnderAttack(int row, int col, bool byWhite) const;
    /**
    *
    * @param isWhite
    * @return the king current position
    */
    std::pair<int, int> findKing(bool isWhite) const;


    /**
     * Handles a promotion .
     * @param row
     * @param col
     * @param isWhite
     */
    void handlePromotion(int row, int col, bool isWhite);
    /**
     * helper function for the stalemate function to determine if there is a draw condition.
     * @return
     */
    bool isDrawByInsufficientMaterial() const;
    /**
     * static evaluate function to help give score to the moves.
     */
    double staticEvaluate() const;
    /**
     * Minimax function that recourseive goes over the moves and giving them score by the move and depth given.
     * @param depth
     * @param maximizingPlayer
     * @return
     */
    double minimax(int depth, bool maximizingPlayer) const;
    bool isCheck(bool check) const;
    double captureBonus(const Move& move) const;
    /**
     * Function to check if castle is possible and if it is, it will also do the move.
     * @param moveStr
     * @return response code
     */
    int handleCastlingMove(const std::string& moveStr);

};

#endif //CHESS_BOARD_H
