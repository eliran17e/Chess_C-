#include "AIEngine.h"
#include "MyExceptions.h"
#include "PriorityQueue.h"
#include <algorithm>
#include <atomic>
#include <thread>
#include <memory>
#include <cstdlib>

std::vector<Move> AIEngine::getTopMovesMultithreaded(Board& board, int count, int depth, int numThreads) {

    bool aiIsWhite = board.isWhiteTurn();

    // Get all valid moves for the current turn
    std::vector<Move> allMoves = board.getAllValidMoves(aiIsWhite);

    if (allMoves.empty()) {
        throw MyExceptions("No legal moves available (checkmate or stalemate).");
    }

    std::mutex scoresMutex;
    std::vector<double> scores(allMoves.size()); // Scores vector, size matching allMoves
    std::atomic<int> moveIndex(0); // Atomic counter for thread-safe move assignment

    // Worker function for each thread
    auto worker = [&]() {
        while (true) {
            int currentIndex = moveIndex.fetch_add(1); // Atomically get next index
            if (currentIndex >= static_cast<int>(allMoves.size())) {
                break; // No more moves to process
            }


            Board boardCopy = board;
            Move currentMove = allMoves[currentIndex]; // Get the move to evaluate

            // The score returned by evaluateMove is from White's perspective (as minimax returns White's perspective).
            double scoreFromWhitesPerspective = boardCopy.evaluateMove(currentMove, depth);

            // Adjust the score to be from the AI's perspective.
            double finalScoreForAI;
            if (aiIsWhite) {
                finalScoreForAI = scoreFromWhitesPerspective; // AI is White, keep score as is
            } else {
                finalScoreForAI = -scoreFromWhitesPerspective; // AI is Black, negate score
            }

            std::lock_guard<std::mutex> lock(scoresMutex);
            scores[currentIndex] = finalScoreForAI;
        }
    };

    // Create and run threads
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker);
    }

    // Join all threads to wait for them to complete
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    // Assign the calculated scores back to the original Move objects
    for (size_t i = 0; i < allMoves.size(); ++i) {
        allMoves[i].score = scores[i];
    }


    PriorityQueue<std::shared_ptr<Move>> pq;
    for (auto & allMove : allMoves) {
        pq.push(std::make_shared<Move>(allMove));
    }

    std::vector<Move> topMoves;
    for (int i = 0; i < count && !pq.empty(); ++i) {
        topMoves.push_back(*pq.poll());
    }

    return topMoves;
}

Move AIEngine::pickBestMove(const std::vector<Move>& moves) {
    if (moves.empty()) throw MyExceptions("No moves to pick from.");
    return moves[0];
}

Move AIEngine::pickRandomTopMove(const std::vector<Move>& moves, int topN) {
    if (moves.empty()) throw MyExceptions("No moves to pick from.");
    int limit = std::min(static_cast<int>(moves.size()), topN);
    int index = rand() % limit;
    return moves[index];
}