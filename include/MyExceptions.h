//
// Created by User on 08/05/2025.
//

#ifndef CHESS_MYEXCEPTIONS_H
#define CHESS_MYEXCEPTIONS_H

#include <exception>
#include <string>

/**
 * @class MyExceptions
 * @brief Custom exception class derived directly from std::exception.
 *
 * Stores an error message and returns it via what().
 */
class MyExceptions : public std::exception {
private:
    std::string message;

public:
    explicit MyExceptions(const std::string& msg)
            : message("Error: " + msg) {}


    // Override what() to return the message
    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif //CHESS_MYEXCEPTIONS_H
