#include "Board.h"
#include "Knight.h"
#include "Bishop.h"
#include "Rook.h"
#include "Queen.h"
#include "King.h"
#include "Pawn.h"
#include "PriorityQueue.h"
#include "Exception_InvalidMove.h"
#include "MyExceptions.h"
#include <cmath>
#include <algorithm>
constexpr int ERR_NO_PIECE         = 11;
constexpr int ERR_OPPONENT_PIECE   = 12;
constexpr int ERR_DEST_SAME_COLOR  = 13;
constexpr int ERR_INVALID_PATTERN  = 21;
constexpr int ERR_MOVE_CAUSES_CHECK = 31;
constexpr int MOVE_LEGAL           = 42;
constexpr int MOVE_LEGAL_CHECK     = 41;

Board::Board(const std::string &board) {
    if (board.size() != 64) {
        throw MyExceptions("Board size must be 64");
    }
    whiteCastling = {false, false, false};
    blackCastling = {false, false, false};
    whiteTurn = true;
    repetitionDraw = false;
    countMoves = 0;
    int col = 0;
    int row = 0;
    chessBoard.resize(8, std::vector<Piece*>(8, nullptr));
    for(char c : board) {
        switch (c) {
            case 'P': chessBoard[row][col] = new Pawn(true, 'P', col, row); break;
            case 'p': chessBoard[row][col] = new Pawn(false, 'p', col, row); break;
            case 'N': chessBoard[row][col] = new Knight(true, 'N', col, row); break;
            case 'n': chessBoard[row][col] = new Knight(false, 'n', col, row); break;
            case 'B': chessBoard[row][col] = new Bishop(true, 'B', col, row); break;
            case 'b': chessBoard[row][col] = new Bishop(false, 'b', col, row); break;
            case 'R': chessBoard[row][col] = new Rook(true, 'R', col, row); break;
            case 'r': chessBoard[row][col] = new Rook(false, 'r', col, row); break;
            case 'Q': chessBoard[row][col] = new Queen(true, 'Q', col, row); break;
            case 'q': chessBoard[row][col] = new Queen(false, 'q', col, row); break;
            case 'K': chessBoard[row][col] = new King(true, 'K', col, row); break;
            case 'k': chessBoard[row][col] = new King(false, 'k', col, row); break;
            case '#': chessBoard[row][col] = nullptr; break;
            default: break;
        }
        col++;
        if (col == 8) {
            col = 0;
            row++;
        }
    }
}

Board::Board(const Board& other) {
    whiteTurn = other.whiteTurn;
    repetitionDraw = other.repetitionDraw;
    countMoves = other.countMoves;


    whiteCastling = other.whiteCastling;
    blackCastling = other.blackCastling;

    // Copy position counts for repetition detection
    positionCounts = other.positionCounts;

    // Copy the board
    chessBoard.resize(8, std::vector<Piece*>(8, nullptr));
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if (other.chessBoard[r][c]) {
                chessBoard[r][c] = other.chessBoard[r][c]->clone();
            }
        }
    }
}

Board& Board::operator=(const Board& other) {
    if (this == &other) {
        return *this; // Self-assignment check
    }

    // Clean up existing board
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            delete chessBoard[i][j];
            chessBoard[i][j] = nullptr;
        }
    }

    // Copy all members
    whiteTurn = other.whiteTurn;
    repetitionDraw = other.repetitionDraw;
    countMoves = other.countMoves;

    // Copy castling rights
    whiteCastling = other.whiteCastling;
    blackCastling = other.blackCastling;

    // Copy position counts
    positionCounts = other.positionCounts;

    // Copy the board
    chessBoard.resize(8, std::vector<Piece*>(8, nullptr));
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if (other.chessBoard[r][c]) {
                chessBoard[r][c] = other.chessBoard[r][c]->clone();
            }
        }
    }

    return *this;
}

Board::~Board() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            delete chessBoard[i][j];
        }
    }
}

int Board::checkMove(const std::string &command, bool isAI) {
    // Handle your custom castling notation
    if (command == "a5a3" || command == "a5a7" || command == "h5h3" || command == "h5h7") {
        return handleCastlingMove(command);
    }

    // Regular move handling
    int fromRow = command[0] - 'a';
    int fromCol = command[1] - '1';
    int toRow = command[2] - 'a';
    int toCol = command[3] - '1';

    int result = isLegalMove(fromRow, fromCol, toRow, toCol);
    if (result == MOVE_LEGAL || result == MOVE_LEGAL_CHECK) {
        makeMove(fromRow, fromCol, toRow, toCol, isAI);
    } else {
        throw Exception_InvalidMove(result);
    }
    return result;
}



int Board::isLegalMove(int fromRow, int fromCol, int toRow, int toCol)const {
    Piece* movingPiece = chessBoard[fromRow][fromCol];
    if (!movingPiece) return ERR_NO_PIECE;

    Piece* target = chessBoard[toRow][toCol];

    // Can't capture enemy king directly
    if (target && dynamic_cast<King*>(target) && target->getIsWhite() != movingPiece->getIsWhite()) {
        return ERR_NO_PIECE;  // or a unique code
    }

    if (movingPiece->getIsWhite() != whiteTurn) return ERR_OPPONENT_PIECE;
    if (target && target->getIsWhite() == movingPiece->getIsWhite()) return ERR_DEST_SAME_COLOR;
    if (!movingPiece->isValidMove(toRow, toCol)) return ERR_INVALID_PATTERN;

    // Pawn-specific logic
    if (auto* pawn = dynamic_cast<Pawn*>(movingPiece)) {
        int dir = pawn->getIsWhite() ? 1 : -1;
        int rowDiff = toRow - fromRow;
        int colDiff = toCol - fromCol;

        if (abs(colDiff) == 1 && rowDiff == dir) {
            if (target && target->getIsWhite() != pawn->getIsWhite()) {
                return wouldBeInCheck(fromRow, fromCol, toRow, toCol) ? ERR_MOVE_CAUSES_CHECK : MOVE_LEGAL;
            }
            return ERR_INVALID_PATTERN;
        }

        if (colDiff == 0) {
            if (target) return ERR_INVALID_PATTERN;
            if (rowDiff == dir ||
                (rowDiff == 2 * dir && ((pawn->getIsWhite() && fromRow == 1) || (!pawn->getIsWhite() && fromRow == 6)))) {
                if (rowDiff == 2 * dir && chessBoard[fromRow + dir][fromCol]) return ERR_INVALID_PATTERN;
                return wouldBeInCheck(fromRow, fromCol, toRow, toCol) ? ERR_MOVE_CAUSES_CHECK : MOVE_LEGAL;
            }
        }

        return ERR_INVALID_PATTERN;
    }

    // For non-knights, path must be clear
    if (!dynamic_cast<Knight*>(movingPiece) && !isPathClear(fromRow, fromCol, toRow, toCol)) {
        return ERR_INVALID_PATTERN;
    }

    if (wouldBeInCheck(fromRow, fromCol, toRow, toCol)) return ERR_MOVE_CAUSES_CHECK;
    if (wouldCauseCheck(fromRow, fromCol, toRow, toCol)) return MOVE_LEGAL_CHECK;

    return MOVE_LEGAL;
}


bool Board::isPathClear(int fromRow, int fromCol, int toRow, int toCol) const{
    // Horizontal movement
    if (fromRow == toRow) {
        int step = (toCol > fromCol) ? 1 : -1;
        for (int col = fromCol + step; col != toCol; col += step) {
            if (chessBoard[fromRow][col] != nullptr) {
                return false; // Path is blocked
            }
        }
        return true;
    }

    // Vertical movement
    if (fromCol == toCol) {
        int step = (toRow > fromRow) ? 1 : -1;
        for (int row = fromRow + step; row != toRow; row += step) {
            if (chessBoard[row][fromCol] != nullptr) {
                return false; // Path is blocked
            }
        }
        return true;
    }

    // Diagonal movement
    if (abs(toRow - fromRow) == abs(toCol - fromCol)) {
        int rowStep = (toRow > fromRow) ? 1 : -1;
        int colStep = (toCol > fromCol) ? 1 : -1;

        int row = fromRow + rowStep;
        int col = fromCol + colStep;

        while (row != toRow && col != toCol) {
            if (chessBoard[row][col] != nullptr) {
                return false; // Path is blocked
            }
            row += rowStep;
            col += colStep;
        }
        return true;
    }
    // If we get here, the move is neither horizontal, vertical, nor diagonal
    return false;
}

bool Board::wouldBeInCheck(int fromRow, int fromCol, int toRow, int toCol) const {
    Board tempBoard = *this; // Create a deep copy of the current board

    Piece* movingPiece = tempBoard.chessBoard[fromRow][fromCol];


    // Simulate the move on the TEMP board
    tempBoard.makeMove(fromRow, fromCol, toRow, toCol, true); // Use true for isAI to avoid human input for promotion

    // Find the king's position on the TEMP board *after* the move
    bool isWhiteKing = movingPiece->getIsWhite(); // This bool is from the original piece


    std::pair<int, int> kingPos;
    try {
        kingPos = tempBoard.findKing(isWhiteKing); // Find the king of the color that just moved
    } catch (const MyExceptions& e) {
        // Handle king not found error if it ever happens in simulation
        std::cerr << "Error: " << e.what() << " during wouldBeInCheck simulation.\n";
        return true; // Treat as if it would cause check if King disappears.
    }

    // Check if the king on the TEMP board is under attack by the opponent's pieces
    // The opponent's pieces on tempBoard are !isWhiteKing (their turn just started)
    return tempBoard.isSquareUnderAttack(kingPos.first, kingPos.second, !isWhiteKing);
}

bool Board::wouldCauseCheck(int fromRow, int fromCol, int toRow, int toCol) const {
    Board tempBoard = *this; // Create a deep copy

    Piece* movingPiece = tempBoard.chessBoard[fromRow][fromCol]; // This piece is from the temp board

    // Simulate move on TEMP board
    tempBoard.makeMove(fromRow, fromCol, toRow, toCol, true); // Use true for isAI

    // Check if the opponent's king on the TEMP board is now in check
    // The opponent's king is of color !movingPiece->getIsWhite() (from original board perspective)
    // On tempBoard, it's now their turn, so we check if *they* are in check.
    return tempBoard.isCheck(!movingPiece->getIsWhite()); // Use the new isCheck function
}

bool Board::isSquareUnderAttack(int row, int col, bool byWhite) const {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Piece* piece = chessBoard[i][j];
            if (piece != nullptr && piece->getIsWhite() == byWhite) {
                // Simply ask the piece if it can attack the target square.
                // Each piece's canAttack method handles its specific movement rules and path clearance.
                if (piece->canAttack(row, col, *this)) {
                    return true;
                }
            }
        }
    }
    return false;
}

std::pair<int, int> Board::findKing(bool isWhite) const {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = chessBoard[row][col];
            if (piece != nullptr && piece->getIsWhite() == isWhite &&
                dynamic_cast<King*>(piece) != nullptr) {
                return {row, col};
            }
        }
    }
    throw MyExceptions("King not found for " + std::string(isWhite ? "White" : "Black"));
}

void Board::makeMove(int fromRow, int fromCol, int toRow, int toCol,bool isAI) {
    Piece* movingPiece = chessBoard[fromRow][fromCol];
    Piece* capturedPiece = chessBoard[toRow][toCol]; // Store captured piece temporarily

    // 1. Delete the captured piece if there was one
    if (capturedPiece != nullptr) {
        delete capturedPiece; // Delete only the captured piece
        chessBoard[toRow][toCol] = nullptr; // Clear the target square temporarily
        countMoves = 0; // Reset 50-move rule on capture
    } else {
        countMoves++; // Increment for non-capture moves
    }

    // 2. Move the piece
    chessBoard[fromRow][fromCol] = nullptr;
    chessBoard[toRow][toCol] = movingPiece;

    if (movingPiece != nullptr) {
        movingPiece->setRow(toRow);
        movingPiece->setCol(toCol);
    }
    if (movingPiece->getSymbol() == 'K') {
        if (movingPiece->getIsWhite())
            whiteCastling.kingMoved = true;
        else
            blackCastling.kingMoved = true;
    }
    else if (movingPiece->getSymbol() == 'R') {
        if (movingPiece->getIsWhite()) {
            if (fromCol == 0) whiteCastling.leftRookMoved = true;
            if (fromCol == 7) whiteCastling.rightRookMoved = true;
        } else {
            if (fromCol == 0) blackCastling.leftRookMoved = true;
            if (fromCol == 7) blackCastling.rightRookMoved = true;
        }
    }
    // 3. Handle pawn moves (reset 50-move rule)
    if (dynamic_cast<Pawn*>(movingPiece)) {
        countMoves = 0; // Reset 50-move rule on pawn move
        if ((movingPiece->getIsWhite() && toRow == 7) || (!movingPiece->getIsWhite() && toRow == 0)) {
            if (isAI) {
                delete chessBoard[toRow][toCol];
                chessBoard[toRow][toCol] = new Queen(movingPiece->getIsWhite(), movingPiece->getIsWhite() ? 'Q' : 'q', toCol, toRow);
            } else {
                handlePromotion(toRow, toCol, movingPiece->getIsWhite());
            }
        }
    }

    // 5. Switch turns
    whiteTurn = !whiteTurn;

    // 6. Repetition draw check
    std::string posKey = toString();
    positionCounts[posKey]++;
    if (positionCounts[posKey] >= 3) {
        repetitionDraw = true;
    }
}

std::vector<Move> Board::getAllValidMoves(bool turn) const{
    std::vector<Move> moves;
    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            Piece* piece = chessBoard[fromRow][fromCol];
            if (piece && piece->getIsWhite() == turn) {
                for (int toRow = 0; toRow < 8; ++toRow) {
                    for (int toCol = 0; toCol < 8; ++toCol) {
                        int res = isLegalMove(fromRow, fromCol, toRow, toCol);
                        if (res == MOVE_LEGAL || res == MOVE_LEGAL_CHECK) {
                            Move m;
                            m.fromRow = fromRow;
                            m.fromCol = fromCol;
                            m.toRow = toRow;
                            m.toCol = toCol;
                            m.isItWhite= turn;

                            m.notation = std::string(1, 'a' + fromRow) + std::to_string(fromCol + 1)
                                         + std::string(1, 'a' + toRow) + std::to_string(toCol + 1);
                            moves.push_back(m);
                        }
                    }
                }
            }
        }
    }
    return moves;
}

void Board::evaluateAllMoves(int depth) {
    currentMoves.clear();
    currentMoves = getAllValidMoves(whiteTurn);
    for (Move& move : currentMoves) {
        move.score = evaluateMove(move, depth);
    }
}

double Board::evaluateMove(const Move& move, int depth) const {
    Board copy = *this;
    copy.makeMove(move.fromRow, move.fromCol, move.toRow, move.toCol, true);

    // Give a bonus for capturing (immediate tactical reward)
    double score = captureBonus(move);

    // Terminal state checks
    if (copy.isCheckmate()) return 10000.0 + score;
    if (copy.isStalemate() || copy.isDrawByInsufficientMaterial()) return score;


    score += copy.minimax(depth - 1, !move.isItWhite);

    return score;
}



std::ostream& operator<<(std::ostream& os, const Move& m) {
    os << m.notation << " (score: " << m.score << ")";
    return os;
}



void Board::handlePromotion(int row, int col, bool isWhite) {
    char choice;
    while (true) {
        std::cout << "Pawn promotion! Choose (q)ueen, (r)ook, (b)ishop, (n)ight: ";
        std::cin >> choice;
        choice = std::tolower(choice);

        Piece* promoted = nullptr;
        switch (choice) {
            case 'q':
                promoted = new Queen(isWhite, isWhite ? 'Q' : 'q', col, row);
                break;
            case 'r':
                promoted = new Rook(isWhite, isWhite ? 'R' : 'r', col, row);
                break;
            case 'b':
                promoted = new Bishop(isWhite, isWhite ? 'B' : 'b', col, row);
                break;
            case 'n':
                promoted = new Knight(isWhite, isWhite ? 'N' : 'n', col, row);
                break;
            default:
                std::cout << "Invalid input. Try again.\n";
                continue;
        }

        //  Safe deletion before assigning new piece
        if (chessBoard[row][col]) {
            delete chessBoard[row][col];
        }
        chessBoard[row][col] = promoted;
        break;
    }

}



std::string Board::toString() const {
    std::string result;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (chessBoard[row][col]) {
                result += chessBoard[row][col]->getSymbol();
            } else {
                result += '#';
            }
        }
    }
    return result;
}

bool Board::isWhiteTurn() const {
    return whiteTurn;
}


bool Board::isCheckmate()const {
    bool inCheck = isSquareUnderAttack(findKing(whiteTurn).first,
                                       findKing(whiteTurn).second,
                                       !whiteTurn);

    if (!inCheck) return false;

    std::vector<Move> legalMoves = getAllValidMoves(whiteTurn);
    return legalMoves.empty();
}

bool Board::isStalemate() const{
    bool inCheck = isSquareUnderAttack(findKing(whiteTurn).first,
                                       findKing(whiteTurn).second,
                                       !whiteTurn);

    std::vector<Move> legalMoves = getAllValidMoves(whiteTurn);

    return (!inCheck && legalMoves.empty()) || countMoves>=50 || repetitionDraw || isDrawByInsufficientMaterial();
}


bool Board::isDrawByInsufficientMaterial() const{
    std::vector<char> white, black;

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* p = chessBoard[r][c];
            if (!p) continue;
            char symbol = std::tolower(p->getSymbol());
            if (symbol == 'k') continue; // skip kings

            if (p->getIsWhite()) white.push_back(symbol);
            else black.push_back(symbol);
        }
    }

    auto isInsufficient = [](const std::vector<char>& pieces) {
        if (pieces.empty()) return true;
        if (pieces.size() == 1 && (pieces[0] == 'n' || pieces[0] == 'b')) return true;
        return false;
    };

    return isInsufficient(white) && isInsufficient(black);
}


double Board::staticEvaluate() const {
    double score = 0.0;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* piece = chessBoard[r][c];
            if (piece) {
                double value = piece->getValue();
                // Material
                if (piece->getIsWhite())
                    score += value;
                else
                    score -= value;
                // Center control bonus: squares c3-c6, d3-d6, e3-e6, f3-f6 (indexes 2-5)
                if (r >= 2 && r <= 5 && c >= 2 && c <= 5) {
                    if (piece->getIsWhite())
                        score += 0.1;
                    else
                        score -= 0.1;
                }
            }
        }
    }
    return score;
}


double Board::minimax(int depth, bool maximizingPlayer) const {
    // Base Case: Depth limit reached or terminal state (checkmate/stalemate)
    if (depth == 0) {
        return staticEvaluate(); // Return the static evaluation from White's perspective
    }

    std::vector<Move> legalMoves = getAllValidMoves(maximizingPlayer);

    // Terminal State Checks (Crucial for correct scoring)
    if (legalMoves.empty()) {
        if (isCheckmate()) {
            // Current player (maximizingPlayer) is in checkmate
            return maximizingPlayer ? -1000000000.0 : 1000000000.0;
        } else {
            // Stalemate
            return 0.0; // Draw
        }
    }

    double bestScore;
    if (maximizingPlayer) {
        bestScore = -1000000001.0; // Initialize with a very low value for maximizing player
        for (const auto& move : legalMoves) {
            Board boardCopy = *this; // Create a copy of the current board
            boardCopy.makeMove(move.fromRow, move.fromCol, move.toRow, move.toCol, true); // Apply the move on the copy
            // Recursively call for the *opponent's* turn, and negate the result
            // The opponent tries to minimize our score, so their best is negative of our best.
            double score = boardCopy.minimax(depth - 1, !maximizingPlayer);
            bestScore = std::max(bestScore, score);
        }
    } else { // Minimizing player
        bestScore = 1000000001.0; // Initialize with a very high value for minimizing player
        for (const auto& move : legalMoves) {
            Board boardCopy = *this; // Create a copy of the current board
            boardCopy.makeMove(move.fromRow, move.fromCol, move.toRow, move.toCol, true); // Apply the move on the copy
            // Recursively call for the *opponent's* turn, and negate the result
            // The opponent tries to maximize our score, so their best is negative of our best.
            double score = boardCopy.minimax(depth - 1, !maximizingPlayer);
            bestScore = std::min(bestScore, score);
        }
    }

    return bestScore;
}
bool Board::isCheck(bool forWhite) const {
    //  Find the position of the King for the specified color.
    std::pair<int, int> kingPos;
    try {

        kingPos = findKing(forWhite);
    } catch (const MyExceptions& e) {
        std::cerr << "Error: " << e.what() << " during isCheck for " << (forWhite ? "White" : "Black") << " King.\n";
        return false;
    }

    // 2. Check if that square is under attack by the opponent's pieces.
    // The opponent's pieces are of the color opposite to the king being checked.
    bool byOpponentWhite = !forWhite;
    return isSquareUnderAttack(kingPos.first, kingPos.second, byOpponentWhite);
}

// Evaluates if a capture is "safe" (not immediately recaptured)
double Board::captureBonus(const Move& move) const {
    Piece* attacker = chessBoard[move.fromRow][move.fromCol];
    Piece* target   = chessBoard[move.toRow][move.toCol];
    if (!attacker || !target) return 0.0; // Not a capture

    // Simulate the capture
    Board temp = *this;
    temp.makeMove(move.fromRow, move.fromCol, move.toRow, move.toCol, true);

    // Is our piece now attacked by the enemy?
    bool safe = !temp.isSquareUnderAttack(move.toRow, move.toCol, !attacker->getIsWhite());
    if (safe) {
        return target->getValue(); // Full value for a safe capture
    } else {
        return target->getValue() * 0.2; // Partial value for risky capture
    }
}


int Board::handleCastlingMove(const std::string& moveStr) {
    bool isWhite, isKingside;

    // Decode your castling notation
    if (moveStr == "a5a3") {      // White Queenside
        isWhite = true; isKingside = false;
    } else if (moveStr == "a5a7") { // White Kingside
        isWhite = true; isKingside = true;
    } else if (moveStr == "h5h3") { // Black Queenside
        isWhite = false; isKingside = false;
    } else if (moveStr == "h5h7") { // Black Kingside
        isWhite = false; isKingside = true;
    } else {
        return ERR_INVALID_PATTERN;
    }

    // Check if it's the correct player's turn
    if (isWhite != whiteTurn) {
        return ERR_OPPONENT_PIECE;
    }

    // Check castling rights
    if (isWhite) {
        if (whiteCastling.kingMoved) return ERR_INVALID_PATTERN;
        if (isKingside && whiteCastling.rightRookMoved) return ERR_INVALID_PATTERN;
        if (!isKingside && whiteCastling.leftRookMoved) return ERR_INVALID_PATTERN;
    } else {
        if (blackCastling.kingMoved) return ERR_INVALID_PATTERN;
        if (isKingside && blackCastling.rightRookMoved) return ERR_INVALID_PATTERN;
        if (!isKingside && blackCastling.leftRookMoved) return ERR_INVALID_PATTERN;
    }

    // Set up coordinates
    int kingRow = isWhite ? 0 : 7;
    int kingCol = 4;
    int rookCol = isKingside ? 7 : 0;
    int newKingCol = isKingside ? 6 : 2;
    int newRookCol = isKingside ? 5 : 3;

    // Verify pieces are in correct positions
    Piece* king = chessBoard[kingRow][kingCol];
    Piece* rook = chessBoard[kingRow][rookCol];

    if (!king || !rook || !dynamic_cast<King*>(king) || !dynamic_cast<Rook*>(rook)) {
        return ERR_NO_PIECE;
    }

    if (king->getIsWhite() != isWhite || rook->getIsWhite() != isWhite) {
        return ERR_OPPONENT_PIECE;
    }

    // Check if path is clear between king and rook
    int start = std::min(kingCol, rookCol) + 1;
    int end = std::max(kingCol, rookCol);
    for (int col = start; col < end; col++) {
        if (chessBoard[kingRow][col] != nullptr) {
            return ERR_DEST_SAME_COLOR;
        }
    }

    // Check if king is in check
    if (isSquareUnderAttack(kingRow, kingCol, !isWhite)) {
        return ERR_MOVE_CAUSES_CHECK;
    }

    // Check if king passes through attacked squares
    int step = (newKingCol > kingCol) ? 1 : -1;
    for (int col = kingCol + step; col != newKingCol + step; col += step) {
        if (isSquareUnderAttack(kingRow, col, !isWhite)) {
            return ERR_MOVE_CAUSES_CHECK;
        }
    }

    // Perform castling
    // Move king
    chessBoard[kingRow][kingCol] = nullptr;
    chessBoard[kingRow][newKingCol] = king;
    king->setRow(kingRow);
    king->setCol(newKingCol);

    // Move rook
    chessBoard[kingRow][rookCol] = nullptr;
    chessBoard[kingRow][newRookCol] = rook;
    rook->setRow(kingRow);
    rook->setCol(newRookCol);

    // Update castling rights
    if (isWhite) {
        whiteCastling.kingMoved = true;
        whiteCastling.leftRookMoved = true;
        whiteCastling.rightRookMoved = true;
    } else {
        blackCastling.kingMoved = true;
        blackCastling.leftRookMoved = true;
        blackCastling.rightRookMoved = true;
    }

    // Switch turns
    whiteTurn = !whiteTurn;

    // Update position tracking
    std::string posKey = toString();
    positionCounts[posKey]++;
    if (positionCounts[posKey] >= 3) {
        repetitionDraw = true;
    }

    return MOVE_LEGAL;
}
