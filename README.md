# Million Quiz Game

A 15-question trivia game in the style of a TV quiz show, written in **C++17** with **SFML 3**.
Answer questions of rising difficulty, use lifelines and climb the prize ladder from $100 to $1,000,000.

Solo academic project, LaSalle College Vancouver (Game Programming).

## Features

- 15 levels, each with its own question file (`data/questions/LevelNQuestions.txt`)
- Three modes: **Classic** (60 s per question), **Speed** (30 s) and **Practice** (no clock)
- Three lifelines, one use each per game: **50/50**, **Phone a Friend** (right about 80% of the time) and **Ask the Audience**
- Prize ladder with safe havens at levels 5 and 10: fail after them and you keep $1,000 or $32,000
- Player profiles saved to a text file: best prize, deepest level, games played
- Leaderboard of the top 9 players
- Resource manager that loads each texture and font only once

## Controls

Mouse only, except at the login screen, where you type a name (max. 16 characters) and press **Enter**.
On the main menu, **Enter** also opens and closes the game mode list.

## Build

You need a C++17 compiler and [SFML 3](https://www.sfml-dev.org/).

### CMake (Windows, Linux, macOS)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cd build && ./MillionQuizGame     # assets/ and data/ are copied next to the executable
```

If SFML 3 is not installed, CMake downloads and builds it automatically.
Run the tests with `ctest --test-dir build`.

### Visual Studio 2022

1. Download SFML 3.0.0 (Visual C++ 17, 64-bit) and extract it to `External/SFML-3.0.0/`.
2. Open `MillionQuizGame.sln`, choose **x64**, build and run.
   The SFML DLLs are copied next to the executable after each build.

The game reads `assets/` and `data/` from its working directory, which Visual Studio sets to the project folder.

## Project structure

```
src/
  main.cpp, Game              Start-up: loads questions, creates the window and shared objects
  MainMenuScreen              Menu, game mode list, leaderboard
  LoginScreen                 Create a profile or sign in
  QuizScreen                  The game itself: questions, clock, lifelines
  GameOverScreen              Result and profile summary
  GameFlow                    Login -> quiz -> save -> game over

  QuizSession                 Rules of one game (levels, answers, lifelines, result). No SFML
  GameRules                   Time limits, prize ladder, safe havens. No SFML
  Lifelines                   50/50, friend and audience logic. No SFML
  Question, QuestionBank      Question data and the text file parser. No SFML
  Player, PlayerDatabase      Player profiles and the PlayerDB.txt file. No SFML

  ResourceManager, UiHelpers  Texture/font cache, buttons, mouse input
  Audio                       Music and sound effects (optional files)
  Config                      Window size and file locations
tests/tests.cpp               Tests for everything marked "No SFML"
assets/                       Images, font and (optional) sounds
data/questions/               One question file per level
```

## Question file format

```
[Question1]
Title=Who was the first President of the United States?
AnswerA=George Washington
AnswerB=Thomas Jefferson
AnswerC=Abraham Lincoln
AnswerD=John Adams
RightAnswer=A
Category=History
```

Incomplete questions are skipped, and a warning is printed.

## Assets

The images in `assets/Images/` are used for learning and portfolio purposes only. Several of them
(menu and game backgrounds, the images on the right side of the quiz screen) come from the TV show
that inspired the game, which belongs to its owners. They are not licensed for reuse: replace them
with your own artwork before publishing or sharing the game. If an image file is missing the game
still runs and prints a warning.
Sounds are not included; see `assets/Soundtrack/README.md`.
The font is Comic Relief, licensed under the SIL Open Font License.
The code has no license attached yet; add one before reusing it.
