//
// Created by User on 06/06/2025.
//

#ifndef CHESS_EXCEPTION_INVALIDMOVE_H
#define CHESS_EXCEPTION_INVALIDMOVE_H

#include <exception>
#include <string>

class Exception_InvalidMove: public std::exception {
private:
    std::string message;
    int errorCode;

public:
    explicit Exception_InvalidMove(int er){
        errorCode=er;
        switch (errorCode){
            case 11:message="There's no piece at the source position"; break;
            case 12: message="The piece belongs to the other player"; break;
            case 13: message= "There's a piece of your color in that square"; break;
            case 21: message="Move pattern is illegal"; break;
            case 31: message="The movement will put your king in check"; break;

        }
    }
    int getErrorCode() const{
        return errorCode;
    }

    // Override what() to return the message
    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif //CHESS_EXCEPTION_INVALIDMOVE_H
