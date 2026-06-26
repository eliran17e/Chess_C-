#include <iostream>
#include <chrono>
#include <random>
#include "GameRunner.h"
#include "Exception_InvalidMove.h"

void GameRunner::runAutoplay(int depth, int threads, int movesToPlay) {
    Board b("RNBQKBNRPPPPPPPP################################pppppppprnbqkbnr");
    Chess gui(b.toString());

    auto start = std::chrono::high_resolution_clock::now();
    autoPlay(b, gui, depth, threads, movesToPlay);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "\nAutoplay completed in " << elapsed.count() << " seconds.\n";
}

void GameRunner::autoPlay(Board& b, Chess& gui, int depth, int threadCount, int movesToPlay) {
    int moveCounter = 0;

    while (moveCounter < movesToPlay) {
        try {
            std::vector<Move> topMoves = AIEngine::getTopMovesMultithreaded(b, 5, depth, std::max(threadCount, 1));
            if (topMoves.empty()) {
                std::cout << "No legal moves left. Game over.\n";
                break;
            }

            Move chosen = AIEngine::pickRandomTopMove(topMoves, 3);

            b.makeMove(chosen.fromRow, chosen.fromCol, chosen.toRow, chosen.toCol,true);
            gui.updateFromBoard(b);
            if (b.isCheckmate()) {
                std::cout << "Checkmate! " << (b.isWhiteTurn() ? "White" : "Black") << " lost.\n";
                break;
            }
            if (b.isStalemate()) {
                std::cout << "Stalemate! It's a draw.\n";
                break;
            }
            std::cout << "\nMove " << (moveCounter + 1) << ": " << chosen << "\n";
            gui.displayBoard();

            moveCounter++;
        } catch (const MyExceptions& e) {
            std::cerr << "Autoplay error: " << e.what() << "\n";
            break;
        }
    }
}

void GameRunner::runManual(int depth, int threads) {
    std::string board = "RNBQKBNRPPPPPPPP################################pppppppprnbqkbnr";
    Board b(board);
    Chess gui(board);
    int codeResponse;

    printSuggestions(b, depth, threads);

    std::string res = gui.getInput();
    while (res != "exit") {
        try {
            codeResponse = b.checkMove(res);
            gui.setCodeResponse(codeResponse);
            gui.updateFromBoard(b);
            printSuggestions(b, depth, threads);

            if (b.isCheckmate()) {
                std::cout << "Checkmate! " << (b.isWhiteTurn() ? "White" : "Black") << " lost.\n";
                break;
            }
            if (b.isStalemate()) {
                std::cout << "Stalemate! Game is a draw.\n";
                break;
            }
        } catch (const Exception_InvalidMove& e) {

            gui.setCodeResponse(e.getErrorCode());
        }
        res = gui.getInput();
    }

    std::cout << "\nExiting...\n";
}

void GameRunner::runVsComputer(int depth, int threads) {
    std::cout << "Choose player color: 0 for White, 1 for Black: ";
    int playerColor;
    std::cin >> playerColor;

    Board b("RNBQKBNRPPPPPPPP################################pppppppprnbqkbnr");
    Chess gui(b.toString());
    std::string userMove;

    // If the player chooses black, let AI move first
    if (playerColor == 1) {
        auto topMoves = AIEngine::getTopMovesMultithreaded(b, 3, depth, std::max(threads, 1));
        if (topMoves.empty()) {
            std::cout << "AI has no legal moves. Game over.\n";
            return;
        }
        Move aiMove = AIEngine::pickRandomTopMove(topMoves, 3);
        int code = b.checkMove(aiMove.notation,true);
        gui.updateFromBoard(b);
        gui.getInput(aiMove.notation);  // Apply move to GUI
        gui.setCodeResponse(code);
        std::cout << "AI played: " << aiMove << "\n";
    }

    while (true) {
        try {
            // Player move
            printSuggestions(b, depth, threads);
            userMove = gui.getInput();  // Empty = ask user
            if (userMove == "exit") break;
            int code = b.checkMove(userMove);
            gui.updateFromBoard(b);
            gui.setCodeResponse(code);

            if (b.isCheckmate()) {
                std::cout << "Checkmate! " << (b.isWhiteTurn() ? "White" : "Black") << " lost.\n";
                break;
            }
            if (b.isStalemate()) {
                std::cout << "Stalemate! It's a draw.\n";
                break;
            }

            // AI move
            auto topMoves = AIEngine::getTopMovesMultithreaded(b, 3, depth, std::max(threads, 1));
            if (topMoves.empty()) {
                std::cout << "AI has no legal moves. Game over.\n";
                break;
            }

            Move aiMove = AIEngine::pickRandomTopMove(topMoves, 3);
            code = b.checkMove(aiMove.notation,true);
            gui.updateFromBoard(b);
            gui.getInput(aiMove.notation);  // Apply move to GUI
            std::cout << "AI played: " << aiMove << "\n";
            gui.setCodeResponse(code);


            if (b.isCheckmate()) {
                std::cout << "Checkmate! " << (b.isWhiteTurn() ? "White" : "Black") << " lost.\n";
                break;
            }
            if (b.isStalemate()) {
                std::cout << "Stalemate! It's a draw.\n";
                break;
            }

        } catch (const Exception_InvalidMove& e) {
            gui.setCodeResponse(e.getErrorCode());
        }
    }

    std::cout << "\nExiting game.\n";
}

void GameRunner::printSuggestions(Board& b, int depth, int threads) {
    try {
        std::vector<Move> suggestions = AIEngine::getTopMovesMultithreaded(b, 3, depth, std::max(threads, 1));
        std::cout << "Top 3 suggested moves:\n";
        for (const auto& move : suggestions) {
            std::cout << move << "\n";
        }
    } catch (const MyExceptions& e) {
        std::cerr << "\033[1;31m" << e.what() << "\033[0m\n";
    }
}
