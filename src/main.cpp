#include "../include/GameRunner.h"

int main() {
    const int MAX_DEPTH = 6;
    const int MAX_THREADS = 8;

    int depth = 0;
    std::cout << "Enter AI depth (1-" << MAX_DEPTH << "): ";
    std::cin >> depth;
    while (std::cin.fail() || depth < 1 || depth > MAX_DEPTH) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Please enter a valid depth (1-" << MAX_DEPTH << "): ";
        std::cin >> depth;
    }

    int threadCount = 0;
    std::cout << "Enter number of threads (0-" << MAX_THREADS << "): ";
    std::cin >> threadCount;
    while (std::cin.fail() || threadCount < 0 || threadCount > MAX_THREADS) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Please enter 0-" << MAX_THREADS << " threads: ";
        std::cin >> threadCount;
    }

    int mode = 0;
    std::cout << "Choose mode:\n"
                 " 0 = Manual\n"
                 " 1 = Autoplay\n"
                 " 2 = Play vs AI\n"
                 "Enter mode number: ";
    std::cin >> mode;
    while (std::cin.fail() || mode < 0 || mode > 2) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Enter a valid mode number (0, 1, or 2): ";
        std::cin >> mode;
    }

    int moves = 0;
    if (mode == 1) {
        std::cout << "Enter number moves to autoplay (1-1000): ";
        std::cin >> moves;
        while (std::cin.fail() || moves < 1 || moves > 1000) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Enter a valid number of moves (1-1000): ";
            std::cin >> moves;
        }
    }
    GameRunner runner;

    switch (mode) {
        case 0:
            runner.runManual(depth, threadCount);
            break;
        case 1:
            runner.runAutoplay(depth, threadCount, moves); // or ask user how many moves
            break;
        case 2:
            runner.runVsComputer(depth, threadCount);
            break;
        default:
            std::cerr << "Invalid mode selected.\n";
            return 1;
    }

    return 0;
}