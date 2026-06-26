#include <catch2/catch_test_macros.hpp>

#include "Pawn.h"
#include "Knight.h"
#include "Bishop.h"
#include "Rook.h"
#include "Queen.h"
#include "King.h"

// Pieces are constructed as Piece(isWhite, symbol, col, row); isValidMove takes (toRow, toCol).

TEST_CASE("Piece values follow the standard scale", "[pieces]") {
    REQUIRE(Pawn(true, 'P', 0, 0).getValue() == 1.0);
    REQUIRE(Knight(true, 'N', 0, 0).getValue() == 3.0);
    REQUIRE(Bishop(true, 'B', 0, 0).getValue() == 3.0);
    REQUIRE(Rook(true, 'R', 0, 0).getValue() == 5.0);
    REQUIRE(Queen(true, 'Q', 0, 0).getValue() == 9.0);
    REQUIRE(King(true, 'K', 0, 0).getValue() == 1000.0);
}

TEST_CASE("Rook moves only along ranks and files", "[pieces]") {
    Rook r(true, 'R', 0, 0); // col 0, row 0
    REQUIRE(r.isValidMove(0, 5));        // same rank
    REQUIRE(r.isValidMove(5, 0));        // same file
    REQUIRE_FALSE(r.isValidMove(3, 3));  // diagonal is not a rook move
}

TEST_CASE("Bishop moves only on diagonals", "[pieces]") {
    Bishop b(true, 'B', 4, 4); // col 4, row 4
    REQUIRE(b.isValidMove(6, 6));        // diagonal
    REQUIRE(b.isValidMove(2, 6));        // other diagonal
    REQUIRE_FALSE(b.isValidMove(4, 5));  // straight is not a bishop move
}

TEST_CASE("Knight moves in an L shape", "[pieces]") {
    Knight n(true, 'N', 4, 4);
    REQUIRE(n.isValidMove(6, 5));        // 2 + 1
    REQUIRE(n.isValidMove(5, 6));        // 1 + 2
    REQUIRE_FALSE(n.isValidMove(5, 5));  // 1 + 1 is not an L
}

TEST_CASE("Queen combines rook and bishop movement", "[pieces]") {
    Queen q(true, 'Q', 4, 4);
    REQUIRE(q.isValidMove(4, 0));        // rank
    REQUIRE(q.isValidMove(0, 4));        // file
    REQUIRE(q.isValidMove(0, 0));        // diagonal
    REQUIRE_FALSE(q.isValidMove(5, 6));  // knight-like jump is illegal
}

TEST_CASE("King moves a single square in any direction", "[pieces]") {
    King k(true, 'K', 4, 4);
    REQUIRE(k.isValidMove(5, 5));        // diagonal step
    REQUIRE(k.isValidMove(4, 5));        // sideways step
    REQUIRE_FALSE(k.isValidMove(4, 6));  // two squares is too far
}

TEST_CASE("White pawn movement patterns", "[pieces]") {
    Pawn p(true, 'P', 1, 1); // col 1, row 1 (white moves toward higher rows)
    REQUIRE(p.isValidMove(2, 1));        // single step forward
    REQUIRE(p.isValidMove(3, 1));        // double step from start
    REQUIRE(p.isValidMove(2, 0));        // diagonal capture
    REQUIRE(p.isValidMove(2, 2));        // diagonal capture
    REQUIRE_FALSE(p.isValidMove(4, 1));  // cannot advance three squares
    REQUIRE_FALSE(p.isValidMove(0, 1));  // cannot move backward
}

TEST_CASE("Black pawn moves toward lower rows", "[pieces]") {
    Pawn p(false, 'p', 1, 6); // col 1, row 6
    REQUIRE(p.isValidMove(5, 1));        // single step forward
    REQUIRE(p.isValidMove(4, 1));        // double step from start
    REQUIRE_FALSE(p.isValidMove(7, 1));  // cannot move backward
}
