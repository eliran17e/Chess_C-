//
// Created by User on 05/04/2025.
//

#include "Piece.h"

Piece::Piece(bool isWhite, char symbol, int col, int row)
        : isWhite(isWhite), symbol(symbol), col(col), row(row) {}



Piece::Piece(const Piece& other)
        : isWhite(other.isWhite), symbol(other.symbol), col(other.col), row(other.row), value(other.value) {}

Piece& Piece::operator=(const Piece& other) {
    if (this == &other) {
        return *this;
    }
    this->symbol = other.symbol;
    this->isWhite = other.isWhite;
    this->col = other.col;
    this->row = other.row;
    this->value = other.value;
    return *this;
}
double Piece::getValue() const {
    return value;
}



char Piece::getSymbol() const {
    return symbol;
}

bool Piece::getIsWhite() const {
    return isWhite;}

int Piece::getCol() const {
    return col;
}
int Piece::getRow() const {
    return row;
}
void Piece::setCol(int x) {
    col = x;
}
void Piece::setRow(int x) {
    row = x;
}
