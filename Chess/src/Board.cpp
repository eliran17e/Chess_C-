#include "../include/Board.h"
#include "../include/Pawn.h"
#include "../include/Knight.h"
#include "../include/Bishop.h"
#include "../include/Rook.h"
#include "../include/Queen.h"
#include "../include/King.h"
#include "../include/PriorityQueue.h"
#include <stdexcept>
#include <cmath>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <algorithm>

using namespace std;

Board::Board(const string &board) {
    if (board.size() != 64) {
        throw MyExceptions("Board size must be 64");
    }
    whiteTurn = true;
    int col = 0;
    int row = 0;
    chessBoard.resize(8, vector<Piece*>(8, nullptr));
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
    chessBoard.resize(8, std::vector<Piece*>(8, nullptr));
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if (other.chessBoard[r][c]) {
                chessBoard[r][c] = other.chessBoard[r][c]->clone();
            }
        }
    }
}

Board::~Board() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            delete chessBoard[i][j];
        }
    }
}

int Board::checkMove(const std::string &command) {
    int fromRow = command[0] - 'a';
    int fromCol = command[1] - '1';
    int toRow = command[2] - 'a';
    int toCol = command[3] - '1';
    int result = isLegalMove(fromRow, fromCol, toRow, toCol);
    if (result == 42 || result == 41) {
        makeMove(fromRow, fromCol, toRow, toCol);
    }
    return result;
}

int Board::isLegalMove(int fromRow, int fromCol, int toRow, int toCol) {
    // Check if there's a piece at the source position
    if (chessBoard[fromRow][fromCol] == nullptr) {
        return 11; // No piece at source
    }

    // Check if the piece belongs to the current player
    if (whiteTurn != chessBoard[fromRow][fromCol]->getIsWhite()) {
        return 12; // Piece belongs to opponent
    }

    // Check if destination has a piece of the same color
    if (chessBoard[toRow][toCol] != nullptr &&
        chessBoard[toRow][toCol]->getIsWhite() == chessBoard[fromRow][fromCol]->getIsWhite()) {
        return 13; // Destination occupied by own piece
    }

    // Check if the move pattern is valid for the specific piece
    if (!chessBoard[fromRow][fromCol]->isValidMove(toRow, toCol)) {
        return 21; // Invalid move pattern for this piece
    }

    // Special handling for pawn diagonal capture
    if (auto* pawn = dynamic_cast<Pawn*>(chessBoard[fromRow][fromCol])) {
        int direction = pawn->getIsWhite() ? 1 : -1; // white moves down (row - 1), black moves up (row + 1)
        int rowDiff = toRow - fromRow;
        int colDiff = toCol - fromCol;
        Piece* target = chessBoard[toRow][toCol];

        // Diagonal capture
        if (abs(colDiff) == 1 && rowDiff == direction) {
            if (target != nullptr && target->getIsWhite() != pawn->getIsWhite()) {
                if (wouldBeInCheck(fromRow, fromCol, toRow, toCol)) return 31;
                return 42;
            }
            return 21; // diagonal but no enemy piece
        }

        // Forward move
        if (colDiff == 0) {
            if (target != nullptr) return 21; // can't move forward into a piece

            if (rowDiff == direction) {
                // single step
                if (wouldBeInCheck(fromRow, fromCol, toRow, toCol)) return 31;
                return 42;
            }

            // double step from starting row
            if (rowDiff == 2 * direction &&
                ((pawn->getIsWhite() && fromRow == 1) || (!pawn->getIsWhite() && fromRow == 6))) {
                if (chessBoard[fromRow + direction][fromCol] != nullptr) return 21;
                if (wouldBeInCheck(fromRow, fromCol, toRow, toCol)) return 31;
                return 42;
            }
        }

        return 21; // any other move is illegal
    }

    // For pieces other than Knight, check if the path is clear
    if (dynamic_cast<Knight*>(chessBoard[fromRow][fromCol]) == nullptr) {
        if (!isPathClear(fromRow, fromCol, toRow, toCol)) {
            return 21; // Path is blocked
        }
    }

    // Check if the move would put the king in check
    if (wouldBeInCheck(fromRow, fromCol, toRow, toCol)) {
        return 31; // Move would put king in check
    }

    // Check if the move creates a check on opponent's king
    if (wouldCauseCheck(fromRow, fromCol, toRow, toCol)) {
        return 41; // Legal move that causes check
    }

    // Legal move
    return 42;
}

bool Board::isPathClear(int fromRow, int fromCol, int toRow, int toCol) {
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

bool Board::wouldBeInCheck(int fromRow, int fromCol, int toRow, int toCol) {
    // Save the current state
    Piece* movingPiece = chessBoard[fromRow][fromCol];
    Piece* capturedPiece = chessBoard[toRow][toCol];

    // Simulate the move
    chessBoard[fromRow][fromCol] = nullptr;
    chessBoard[toRow][toCol] = movingPiece;

    // Find the king's position
    int kingRow = -1, kingCol = -1;
    bool isWhiteKing = movingPiece->getIsWhite();

    // If the moving piece is the king, use the destination as king's position
    if (dynamic_cast<King*>(movingPiece) != nullptr) {
        kingRow = toRow;
        kingCol = toCol;
    } else {
        // Otherwise, find the king
        auto kingPos = findKing(isWhiteKing);
        kingRow = kingPos.first;
        kingCol = kingPos.second;
    }

    // Check if any opponent piece can attack the king
    bool isInCheck = isSquareUnderAttack(kingRow, kingCol, !isWhiteKing);

    // Restore the board state
    chessBoard[fromRow][fromCol] = movingPiece;
    chessBoard[toRow][toCol] = capturedPiece;

    return isInCheck;
}

bool Board::wouldCauseCheck(int fromRow, int fromCol, int toRow, int toCol) {
    Piece* movingPiece = chessBoard[fromRow][fromCol];
    Piece* capturedPiece = chessBoard[toRow][toCol];

    // Save original piece position
    int origRow = movingPiece->getRow();
    int origCol = movingPiece->getCol();

    // Simulate move
    chessBoard[fromRow][fromCol] = nullptr;
    chessBoard[toRow][toCol] = movingPiece;
    movingPiece->setRow(toRow);
    movingPiece->setCol(toCol);

    auto opponentKingPos = findKing(!movingPiece->getIsWhite());
    int kingRow = opponentKingPos.first;
    int kingCol = opponentKingPos.second;

    bool causesCheck = isSquareUnderAttack(kingRow, kingCol, movingPiece->getIsWhite());

    // Undo move
    chessBoard[fromRow][fromCol] = movingPiece;
    chessBoard[toRow][toCol] = capturedPiece;
    movingPiece->setRow(origRow);
    movingPiece->setCol(origCol);

    return causesCheck;
}

bool Board::isSquareUnderAttack(int row, int col, bool byWhite) {
    // Check if any piece of the specified color can attack this square
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Piece* piece = chessBoard[i][j];
            if (piece != nullptr && piece->getIsWhite() == byWhite) {
                // Check if the piece can move to the square
                if (auto* pawn = dynamic_cast<Pawn*>(piece)) {
                    if (pawn->canAttack(row, col)) {
                        return true;
                    }
                } else if (piece->isValidMove(row, col)) {
                    if (dynamic_cast<Knight*>(piece)) {
                        return true;
                    } else if (isPathClear(i, j, row, col)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

std::pair<int, int> Board::findKing(bool isWhite) {
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

void Board::makeMove(int fromRow, int fromCol, int toRow, int toCol) {
    // Delete captured piece first
    if (chessBoard[toRow][toCol] != nullptr) {
        delete chessBoard[toRow][toCol];
    }

    // Actually execute the move on the board
    Piece* piece = chessBoard[fromRow][fromCol];
    chessBoard[fromRow][fromCol] = nullptr;
    chessBoard[toRow][toCol] = piece;

    if (piece != nullptr) {
        piece->setRow(toRow);
        piece->setCol(toCol);
    }

    // Switch turns for future changes that might be needed
    whiteTurn = !whiteTurn;

    // Check for pawn promotion
    if (piece != nullptr && dynamic_cast<Pawn*>(piece)) {
        Pawn* pawn = dynamic_cast<Pawn*>(piece);
        if ((pawn->getIsWhite() && toRow == 7) || (!pawn->getIsWhite() && toRow == 0)) {
            handlePromotion(toRow, toCol, pawn->getIsWhite());
        }
    }
}

std::vector<Move> Board::getAllValidMoves(bool turn) {
    std::vector<Move> moves;
    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            Piece* piece = chessBoard[fromRow][fromCol];
            if (piece && piece->getIsWhite() == turn) {
                for (int toRow = 0; toRow < 8; ++toRow) {
                    for (int toCol = 0; toCol < 8; ++toCol) {
                        int res = isLegalMove(fromRow, fromCol, toRow, toCol);
                        if (res == 41 || res == 42) {
                            Move m;
                            m.fromRow = fromRow;
                            m.fromCol = fromCol;
                            m.toRow = toRow;
                            m.toCol = toCol;

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

double Board::evaluateMove(const Move& move, int depth) {
    Board copy = *this; // Deep copy
    return copy.evaluateMoveInternal(move, depth);
}

// FIXED: Proper board state management in recursive evaluation
double Board::evaluateMoveInternal(const Move& move, int depth) {
    bool isWhite = whiteTurn;
    Piece* attacker = chessBoard[move.fromRow][move.fromCol];
    Piece* target = chessBoard[move.toRow][move.toCol];

    if (!attacker || attacker->getIsWhite() != isWhite) return -999;

    // Save captured piece for restoration
    Piece* capturedPiece = target;

    // Make the move
    chessBoard[move.fromRow][move.fromCol] = nullptr;
    chessBoard[move.toRow][move.toCol] = attacker;
    attacker->setRow(move.toRow);
    attacker->setCol(move.toCol);

    double score = 0.0;
    score += evaluateBoardPosition(isWhite);

    // Central control bonus
    if ((move.toRow >= 2 && move.toRow <= 5) && (move.toCol >= 2 && move.toCol <= 5)) {
        score += 2.0;
    }
    if ((move.toRow == 3 || move.toRow == 4) && (move.toCol == 3 || move.toCol == 4)) {
        score += 4.5;
    }

    // Square safety
    if (!isSquareUnderAttack(move.toRow, move.toCol, !isWhite)) {
        score += 0.3;
    }

    if (capturedPiece) {
        double captureValue = capturedPiece->getValue();
        score += captureValue;

        bool isSafeCapture = !isSquareUnderAttack(move.toRow, move.toCol, !isWhite);
        bool attackerIsProtected = isSquareProtected(move.toRow, move.toCol, isWhite);

        if (isSafeCapture) score += captureValue * 2;
        if (attacker->getValue() <= captureValue) score += 1.5;
        if (!isSafeCapture && !attackerIsProtected) {
            double penalty = (attacker->getValue() - captureValue) * 2;
            score -= penalty;
        }
    }

    // Penalize if attacker is vulnerable
    if (isSquareUnderAttack(move.toRow, move.toCol, !isWhite) &&
        !isSquareProtected(move.toRow, move.toCol, isWhite)) {
        score -= attacker->getValue() * 1.2;
    }

    // Threat & support analysis
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* piece = chessBoard[r][c];
            if (!piece || piece == attacker) continue;

            bool isEnemy = piece->getIsWhite() != isWhite;
            bool isAlly = !isEnemy;

            if (attacker->canAttack(r, c) &&
                (dynamic_cast<Knight*>(attacker) || dynamic_cast<Pawn*>(attacker) || isPathClear(move.toRow, move.toCol, r, c))) {

                if (isEnemy) {
                    double diff = piece->getValue() - attacker->getValue();
                    if (diff >= -1.0) {
                        score += std::max(0.5, diff) * 0.8;
                        if (!isSquareProtected(r, c, piece->getIsWhite())) {
                            score += std::max(1.0, diff) * 1.2;
                        }
                    } else if (!isSquareProtected(r, c, piece->getIsWhite())) {
                        score += 1.2;
                    }
                }

                if (isAlly && isSquareUnderAttack(r, c, !isWhite)) {
                    if (!isSquareUnderAttack(move.toRow, move.toCol, !isWhite)) {
                        score += piece->getValue() * 0.2;
                    } else {
                        score -= 0.3;
                    }
                }
            }
        }
    }

    // Recursive lookahead
    if (depth > 0) {
        whiteTurn = !whiteTurn;
        auto replies = getAllValidMoves(whiteTurn);

        double bestReplyScore = 0.0;
        if (!replies.empty()) {
            bestReplyScore = -1000.0;
            for (const auto& reply : replies) {
                bestReplyScore = std::max(bestReplyScore, -evaluateMoveInternal(reply, depth - 1));
            }
        }

        score -= bestReplyScore * 0.8;
        whiteTurn = !whiteTurn; // Restore turn
    }

    // FIXED: Restore the board state properly
    chessBoard[move.fromRow][move.fromCol] = attacker;
    chessBoard[move.toRow][move.toCol] = capturedPiece;
    attacker->setRow(move.fromRow);
    attacker->setCol(move.fromCol);

    // Check for king presence (safety check)
    try {
        findKing(true);
        findKing(false);
    } catch (const MyExceptions&) {
        return -9999;
    }

    return score;
}

std::vector<Move> Board::getTopMoves(int count, int depth) {
    evaluateAllMoves(depth);
    PriorityQueue<std::shared_ptr<Move>> pq;

    if (currentMoves.empty()) {
        throw MyExceptions("No legal moves available (checkmate or stalemate).");
    }

    for (Move& m : currentMoves) {
        auto ptr = std::make_shared<Move>(m);
        pq.push(ptr);
    }

    std::vector<Move> top;
    for (int i = 0; i < count && !pq.empty(); ++i) {
        top.push_back(*pq.poll());
    }

    return top;
}

std::ostream& operator<<(std::ostream& os, const Move& m) {
    os << m.notation << " (score: " << m.score << ")";
    return os;
}

bool Board::isSquareProtected(int row, int col, bool byWhite) {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* piece = chessBoard[r][c];
            if (piece && piece->getIsWhite() == byWhite) {
                if (piece->canAttack(row, col)) {
                    if (dynamic_cast<Knight*>(piece) || dynamic_cast<Pawn*>(piece)) return true;
                    if (isPathClear(r, c, row, col)) return true;
                }
            }
        }
    }
    return false;
}

double Board::evaluateBoardPosition(bool isWhite) {
    double score = 0.0;

    int homeRow = isWhite ? 0 : 7;
    int pawnStartRow = isWhite ? 1 : 6;
    int stage = getGameStage();

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece *piece = chessBoard[row][col];
            if (!piece || piece->getIsWhite() != isWhite) continue;

            // Reward pawn development (moved from start row)
            if (auto *p = dynamic_cast<Pawn *>(piece)) {
                if (row != pawnStartRow) score += 0.5;
            }

            if (dynamic_cast<King*>(piece)) {
                score += evaluateKingSafety(row, col, isWhite, stage);
            }

            // Reward minor piece development
            if ((dynamic_cast<Knight *>(piece) || dynamic_cast<Bishop *>(piece)) && row != homeRow) {
                score += 0.5;
            }
        }
    }

    return score;
}

int Board::getGameStage() {
    int pieceCount = 0;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if (chessBoard[r][c] != nullptr) pieceCount++;
        }
    }

    if (pieceCount >= 28) return 0; // Opening
    if (pieceCount > 16) return 1;  // Middlegame
    return 2;                       // Endgame
}

double Board::evaluateKingSafety(int row, int col, bool isWhite, int gameStage) {
    double score = 0;

    if (gameStage == 2) return 0; // King safety less important in endgame

    // Penalize king in center during opening or middlegame
    if ((row >= 2 && row <= 5) && (col >= 2 && col <= 5)) {
        score -= 5.0;
        if (gameStage == 0) score -= 3.0; // Extra penalty in opening
    }

    // Check pawn shield
    int pawnShield = 0;
    int frontRow = isWhite ? row - 1 : row + 1;
    if (frontRow >= 0 && frontRow < 8) {
        for (int c = col - 1; c <= col + 1; c++) {
            if (c >= 0 && c < 8) {
                Piece* piece = chessBoard[frontRow][c];
                if (piece && dynamic_cast<Pawn*>(piece) && piece->getIsWhite() == isWhite) {
                    pawnShield++;
                }
            }
        }
    }
    score += pawnShield * 1.5;

    return score;
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

        delete chessBoard[row][col]; // Delete the pawn
        chessBoard[row][col] = promoted;
        break;
    }
}

void Board::dumpBoard() {
    std::cout << "Current board state:\n";
    for (int row = 0; row < 8; ++row) {
        std::cout << (char)('A' + row) << " ";
        for (int col = 0; col < 8; ++col) {
            if (chessBoard[row][col]) {
                std::cout << chessBoard[row][col]->getSymbol() << " ";
            } else {
                std::cout << ". ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "  1 2 3 4 5 6 7 8\n";
}

// FIXED: Completely rewritten multithreaded function
std::vector<Move> Board::getTopMovesMultithreaded(int count, int depth, int numThreads) {
    // First get all valid moves
    std::vector<Move> allMoves = getAllValidMoves(whiteTurn);

    if (allMoves.empty()) {
        throw MyExceptions("No legal moves available (checkmate or stalemate).");
    }

    // Thread-safe containers
    std::mutex scoresMutex;
    std::vector<double> scores(allMoves.size(), -9999.0);
    std::atomic<int> moveIndex(0);

    // Worker function for threads
    auto worker = [&]() {
        while (true) {
            int currentIndex = moveIndex.fetch_add(1);
            if (currentIndex >= static_cast<int>(allMoves.size())) {
                break;
            }

            // Each thread gets its own board copy for evaluation
            Board boardCopy = *this;
            double score = boardCopy.evaluateMove(allMoves[currentIndex], depth);

            // Thread-safe assignment of score
            std::lock_guard<std::mutex> lock(scoresMutex);
            scores[currentIndex] = score;
        }
    };

    // Create and start threads
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker);
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    // Assign scores to moves and sort
    for (size_t i = 0; i < allMoves.size(); ++i) {
        allMoves[i].score = scores[i];
    }

    // Sort moves by score (highest first)
    std::sort(allMoves.begin(), allMoves.end(),
              [](const Move& a, const Move& b) { return a.score > b.score; });

    // Return top moves
    std::vector<Move> topMoves;
    int maxMoves = std::min(count, static_cast<int>(allMoves.size()));
    for (int i = 0; i < maxMoves; ++i) {
        topMoves.push_back(allMoves[i]);
    }

    return topMoves;
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