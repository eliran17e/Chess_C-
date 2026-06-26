cmake_minimum_required(VERSION 3.15)
project(Chess)

if (MSVC AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.21)
    set(CMAKE_CXX_STANDARD 23)
else()
    set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Chess
    src/main.cpp
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

target_include_directories(Chess PRIVATE include)

find_package(Threads REQUIRED)
target_link_libraries(Chess PRIVATE Threads::Threads)

if (MSVC)
    target_compile_options(Chess PRIVATE /W4 /permissive- /Zc:externConstexpr /Zc:inline)
else()
    target_compile_options(Chess PRIVATE -Wall -Wextra)
endif()
