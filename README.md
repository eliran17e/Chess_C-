# Chess Engine (C++)

A console-based chess engine written in modern C++. It enforces full move legality,
detects all standard end-of-game conditions, and ships with a multithreaded
minimax AI that can play against you, suggest moves, or play itself.

<p align="center">
  <img src="./img/cpp-logo.png" alt="C++" height="90">
</p>

## Features

- **Complete move-legality rules** — per-piece movement, path blocking, pawn promotion, and castling (rights tracking + path/through-check validation).
- **End-game detection** — check, checkmate, stalemate, the 50-move rule, threefold repetition, and draw by insufficient material.
- **Minimax AI** — evaluates positions by material, center control, and capture safety, searching to a user-chosen depth (1–6).
- **Multithreading** — top-level move evaluation is parallelized across a configurable number of threads (0–8).
- **Three play modes** — manual (both sides, with AI suggestions), AI vs AI autoplay, and human vs AI.
- **Cross-platform console UI** — ANSI rendering on Linux/macOS, Win32 console API on Windows.

## Build

Requires a C++20 compiler, CMake ≥ 3.15, and a threads library.

```sh
sudo apt install -y g++ cmake make   # Linux

cd Chess
cmake -B build
cmake --build build
./build/Chess
```

## Usage

On launch you are prompted for:

| Prompt | Range | Meaning |
|--------|-------|---------|
| AI depth | 1–6 | Search depth — higher is stronger but slower |
| Threads | 0–8 | 0 = single-threaded; higher parallelizes AI analysis |
| Mode | 0 / 1 / 2 | 0 = Manual, 1 = Autoplay (AI vs AI), 2 = Play vs Computer |
| Moves | 1–1000 | (Autoplay only) number of moves to play |

Enter moves in coordinate notation, e.g. `b4d4`. Castling uses a custom notation
(`a5a3`, `a5a7`, `h5h3`, `h5h7`). Type `exit` to quit.

## How the AI works

The engine scores each candidate move with a minimax search from the current
position:

- **Material** — standard piece values, summed from White's perspective.
- **Positioning** — a small bonus for occupying the four central files/ranks.
- **Capture safety** — full credit for a capture that can't be immediately recaptured, partial credit otherwise.
- **Lookahead** — simulates the opponent's best replies down to the chosen depth.

Candidate moves are ranked with a bounded priority queue; in autoplay and
vs-computer modes the engine picks randomly among the top moves for more
varied, human-like play.

## Project structure

```
Chess/
├── CMakeLists.txt
├── include/        # headers (Board, Piece hierarchy, AIEngine, GameRunner, ...)
└── src/            # implementations + main.cpp
```

## Known limitations

- No en passant.
- Castling path/through-check validation applies to the human player only, not AI-generated moves.
- Console interface only — no GUI.

## Possible improvements

- En passant and full castling parity for the AI.
- Alpha-beta pruning and transposition tables for deeper, faster search.
- Undo/redo, save/load (FEN/PGN), and move timers.
- A unit-test suite and a graphical front-end.

---

<sub>Originally built as a C++ course exercise at Tel-Hai Excellenteam, then refactored for clarity and correctness.</sub>
