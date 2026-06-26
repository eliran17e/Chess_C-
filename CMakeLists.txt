cmake_minimum_required(VERSION 3.15)
project(Chess CXX)

if (MSVC AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.21)
    set(CMAKE_CXX_STANDARD 23)
else()
    set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

# Core engine, built as a library so both the app and the tests can link it.
add_library(chess_engine
    src/Board.cpp
    src/Chess.cpp
    src/Piece.cpp
    src/Rook.cpp
    src/Bishop.cpp
    src/Queen.cpp
    src/King.cpp
    src/Knight.cpp
    src/Pawn.cpp
    src/AIEngine.cpp
    src/GameRunner.cpp
)
target_include_directories(chess_engine PUBLIC include)
target_link_libraries(chess_engine PUBLIC Threads::Threads)

if (MSVC)
    target_compile_options(chess_engine PRIVATE /W4 /permissive- /Zc:externConstexpr /Zc:inline)
else()
    target_compile_options(chess_engine PRIVATE -Wall -Wextra)
endif()

# Game executable.
add_executable(Chess src/main.cpp)
target_link_libraries(Chess PRIVATE chess_engine)

# Unit tests (opt-in: cmake -B build -DBUILD_TESTS=ON).
option(BUILD_TESTS "Build the unit-test suite" OFF)
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
