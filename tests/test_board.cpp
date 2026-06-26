#include <catch2/catch_test_macros.hpp>

#include "Board.h"
#include "MyExceptions.h"
#include "Exception_InvalidMove.h"

namespace {
// Standard starting position. Row 0 is White's back rank, rows 0-1 White,
// rows 6-7 Black. '#' is an empty square.
const std::string kStart =
    "RNBQKBNRPPPPPPPP################################pppppppprnbqkbnr";

// checkMove returns these codes (see Board.cpp); 42 == legal, quiet move.
constexpr int kMoveLegal = 42;
}

TEST_CASE("Board round-trips the starting position", "[board]") {
    Board b(kStart);
    REQUIRE(b.toString() == kStart);
}

TEST_CASE("Board rejects input that is not 64 squares", "[board]") {
    REQUIRE_THROWS_AS(Board("too short"), MyExceptions);
}

TEST_CASE("White moves first", "[board]") {
    Board b(kStart);
    REQUIRE(b.isWhiteTurn());
}

TEST_CASE("Both kings are located at their starting squares", "[board]") {
    Board b(kStart);
    REQUIRE(b.findKing(true) == std::make_pair(0, 4));
    REQUIRE(b.findKing(false) == std::make_pair(7, 4));
}

TEST_CASE("Starting position is neither checkmate nor stalemate", "[board]") {
    Board b(kStart);
    REQUIRE_FALSE(b.isCheckmate());
    REQUIRE_FALSE(b.isStalemate());
}

TEST_CASE("A legal pawn advance is accepted and passes the turn", "[board]") {
    Board b(kStart);
    REQUIRE(b.checkMove("b1c1") == kMoveLegal); // pawn one square forward
    REQUIRE_FALSE(b.isWhiteTurn());             // now Black to move
}

TEST_CASE("An illegal move is rejected", "[board]") {
    Board b(kStart);
    REQUIRE_THROWS_AS(b.checkMove("b1e1"), Exception_InvalidMove); // pawn 3 squares
}

TEST_CASE("King versus King is a draw by insufficient material", "[board]") {
    std::string kk(64, '#');
    kk[0]  = 'K'; // row 0, col 0
    kk[63] = 'k'; // row 7, col 7
    Board b(kk);
    REQUIRE(b.isDrawByInsufficientMaterial());
}

TEST_CASE("A full starting position is not an insufficient-material draw", "[board]") {
    Board b(kStart);
    REQUIRE_FALSE(b.isDrawByInsufficientMaterial());
}
