//
// Created by User on 05/04/2025.
//

#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H
#include "Piece.h"
#include <vector>
#include "Pawn.h"
#include <ostream>

struct Move {
    std::string notation;
    int score = 0;
    int fromRow, fromCol;
    int toRow, toCol;
    int operator*() const { return score; }
    bool operator<(const Move& other) const {
        return score < other.score;
    }

    bool operator>(const Move& other) const {
        return score > other.score;
    }
};
std::ostream& operator<<(std::ostream& os, const Move& m);
/**
 * The Board class represents a chessboard and manages the game state,
 * including move validation, turn management,
 * and enforcing some of the chess rules like check or capture.
 */

class Board {
private:
    std::vector<struct Move> currentMoves;
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
    /**
     * Evaluate a chess move and giving it a score.
     * @param move
     * @param depth
     * @return
     */
    double evaluateMove(const Move& move, int depth);
    /**
     * Checks if a square is protected by another piece
     * @param row
     * @param col
     * @param byWhite
     * @return
     */
    bool isSquareProtected(int row, int col, bool byWhite);
    /**
     * Evaluate Board position and giving score
     * if pieces moving from their starting squares
     * @param isWhite
     * @return
     */
    double evaluateBoardPosition(bool isWhite);
    /**
     * GameStage is a function that checks what part of the game we at now
     * for better suggestions in TopMoves (not used yet)
     * @return
     */
    int getGameStage();
    /**
     *  Checks the king safety and giving score
     * @param row
     * @param col
     * @param isWhite
     * @param gameStage
     * @return
     */
    double evaluateKingSafety(int row, int col, bool isWhite, int gameStage);
    /**
     * Handles a promotion .
     * @param row
     * @param col
     * @param isWhite
     */
    void handlePromotion(int row, int col, bool isWhite);
    void dumpBoard();
    double evaluateMoveInternal(const Move& move, int depth);

public:
    /**
 *
 * @param board
 * builds the chess board with a 8x8 matrix vector.
 */
    explicit Board(const std::string& board);
    Board(const Board& other);
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
    /**
     * Gets all the valid moves and creates a vector of all the moves.
     * @param turn
     * @return
     */
    std::vector<Move> getAllValidMoves(bool turn);
    /**
     * Evaluate all the moves based on depth that the user choose.
     * @param depth
     */
    void evaluateAllMoves(int depth);
    /**
     *
     * @param count
     * @param depth
     * @return a vector of the top moves using PriorityQueue
     */
    std::vector<Move> getTopMoves(int count, int depth);
    /**
     *
     * @param count
     * @param depth
     * @param numThreads
     * @return a vector of the top moves using threads for faster and more efficient results
     */
    std::vector<Move> getTopMovesMultithreaded(int count, int depth, int numThreads);
    /**
     *
     * @return a string that represents the current board state
     */
    std::string toString() const;

};

#endif //CHESS_BOARD_H
