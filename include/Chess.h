#pragma once
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif
#include "Board.h"
#include <string>

class Chess {
    static constexpr int kDisplaySize = 21;
    unsigned char m_board[kDisplaySize][kDisplaySize] = { 0 };
    bool m_turn = true;
    std::string m_boardString;
    std::string m_input;
    std::string m_msg = "\n";
    std::string m_errorMsg = "\n";
    int m_codeResponse;

    void clear() const;
    void setFrames();
    void setPieces();
    void show() const;

    void showAskInput() const;
    bool isSame() const;
    bool isValid() const;
    bool isExit() const;

    void doTurn();

public:
    Chess(const std::string& start = "RNBQKBNRPPPPPPPP################################pppppppprnbqkbnr");
    Chess(const Chess&) = delete;
    Chess& operator=(const Chess&) = delete;
    std::string getInput(const std::string& aiInput = "");
    void setCodeResponse(int codeResponse);
    void setInput(const std::string& input);
    void excute();
    void displayBoard() const;
    void updateFromBoard(const Board& b);
};
