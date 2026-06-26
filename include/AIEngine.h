#pragma once

#include "Board.h"
#include <vector>

class AIEngine {
public:
    static std::vector<Move> getTopMovesMultithreaded(Board& board, int count, int depth, int numThreads);
    static Move pickBestMove(const std::vector<Move>& moves);
    static Move pickRandomTopMove(const std::vector<Move>& moves, int topN);
};
