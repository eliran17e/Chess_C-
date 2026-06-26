//
// Created by User on 06/06/2025.
//

#ifndef CHESS_GAMERUNNER_H
#define CHESS_GAMERUNNER_H

#include "Chess.h"
#include "Board.h"
#include "AIEngine.h"
#include "MyExceptions.h"

class GameRunner {
private:
    static void autoPlay(Board& b, Chess& gui, int depth, int threadCount, int movesToPlay);
public:
    static void runAutoplay(int depth, int threads, int movesToPlay = 8);
    static void runManual(int depth, int threads);
    static void runVsComputer(int depth, int threads);
    static void printSuggestions(Board& b, int depth, int threads);
};


#endif //CHESS_GAMERUNNER_H
