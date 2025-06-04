#include "../include/Chess.h"
#include "../include/Board.h"
#include "../include/MyExceptions.h"
#include <iostream>  // already present
#include <chrono>    // optional, for benchmarking
#include <random>

void autoplay(Board& b, Chess& gui, int depth, int threadCount, int movesToPlay = 8) {
    std::mt19937 rng(std::random_device{}());
    int moveCounter = 0;

    while (moveCounter < movesToPlay) {
        std::vector<Move> topMoves;
        try {
            if (threadCount <= 0)
                topMoves = b.getTopMoves(5, depth);
            else
                topMoves = b.getTopMovesMultithreaded(5, depth, threadCount);

            if (topMoves.empty()) {
                std::cout << "No legal moves left. Game over.\n";
                break;
            }

            std::uniform_int_distribution<int> dist(0, std::min(4, (int)topMoves.size() - 1));
            Move chosen = topMoves[dist(rng)];

            // Apply move to board and update GUI
            b.makeMove(chosen.fromRow, chosen.fromCol, chosen.toRow, chosen.toCol);
            gui.updateFromBoard(b);

            std::cout << "\nMove " << (moveCounter + 1) << ": " << chosen << "\n";
            gui.displayBoard();

            moveCounter++;

        } catch (const MyExceptions& e) {
            std::cerr << "Autoplay error: " << e.what() << "\n";
            break;
        }
    }
}


int main()
{
    std::string board = "RNBQKBNRPPPPPPPP################################pppppppprnbqkbnr";
    Board b(board);
    Chess a(board);
    int depth, threadCount;
    int codeResponse = 0;

    std::cout << "Enter AI depth: ";
    if (!(std::cin >> depth)) {
        std::cerr << "Invalid input for depth.\n";
        return 1;
    }

    std::cout << "Enter number of threads (0 for no threading): ";
    if (!(std::cin >> threadCount)) {
        std::cerr << "Invalid input for thread count.\n";
        return 1;
    }
    int autoPlayChoice = 0;
    std::cout << "Choose mode: 1 = Autoplay, 0 = Manual play: ";
    std::cin >> autoPlayChoice;
    if (autoPlayChoice != 0 && autoPlayChoice != 1) {
        std::cerr << "Invalid mode selection. Enter 0 for Manual or 1 for Autoplay.\n";
        return 1;
    }

    if (autoPlayChoice == 1) {
        int movesToPlay = 8;
        auto start = std::chrono::high_resolution_clock::now();

        autoplay(b, a, depth, threadCount, movesToPlay);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "\nAutoplay completed in " << elapsed.count() << " seconds.\n";
        return 0;
    }
    try {
        std::vector<Move> suggestions;
        if (threadCount <= 0) {
            suggestions = b.getTopMoves(3, depth);
        } else {
            suggestions = b.getTopMovesMultithreaded(3, depth, threadCount);
        }

        std::cout << "Top 3 suggested moves:\n";
        for (const auto& move : suggestions) {
            std::cout << move << "\n";
        }
    } catch (const MyExceptions& e) {
        std::cerr << "\033[1;31m" << e.what() << "\033[0m\n";
    }

    std::string res = a.getInput();
    while (res != "exit")
    {
        codeResponse = b.checkMove(res);

        if (codeResponse == 41 || codeResponse == 42) {
            try {
                std::vector<Move> suggestions;
                if (threadCount <= 0) {
                    suggestions = b.getTopMoves(3, depth);
                } else {
                    suggestions = b.getTopMovesMultithreaded(3, depth, threadCount);
                }

                std::cout << "Top 3 suggested moves:\n";
                for (const auto& move : suggestions) {
                    std::cout << move << "\n";
                }
            } catch (const MyExceptions& e) {
                std::cerr << "\033[1;31m" << e.what() << "\033[0m\n";
            }
        }

        a.setCodeResponse(codeResponse);
        res = a.getInput();
    }

    std::cout << "\nExiting\n";
    return 0;
}
