cmake_minimum_required(VERSION 3.16)
project(MillionQuizGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use an installed SFML 3 if there is one, otherwise download and build it.
find_package(SFML 3 COMPONENTS Graphics Window System Audio QUIET)
if(NOT SFML_FOUND)
    include(FetchContent)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        GIT_SHALLOW ON)
    FetchContent_MakeAvailable(SFML)
endif()

# Game rules and data handling: no SFML needed, so they are also used by the tests.
add_library(GameLogic STATIC
    src/GameRules.cpp
    src/Lifelines.cpp
    src/Player.cpp
    src/PlayerDatabase.cpp
    src/QuestionBank.cpp
    src/QuizSession.cpp)
target_include_directories(GameLogic PUBLIC src)

add_executable(MillionQuizGame
    src/main.cpp
    src/Game.cpp
    src/Audio.cpp
    src/ResourceManager.cpp
    src/UiHelpers.cpp
    src/MainMenuScreen.cpp
    src/LoginScreen.cpp
    src/QuizScreen.cpp
    src/GameOverScreen.cpp
    src/GameFlow.cpp)
target_link_libraries(MillionQuizGame PRIVATE GameLogic SFML::Graphics SFML::Audio)

if(MSVC)
    target_compile_options(GameLogic PRIVATE /W4)
    target_compile_options(MillionQuizGame PRIVATE /W4)
else()
    target_compile_options(GameLogic PRIVATE -Wall -Wextra)
    target_compile_options(MillionQuizGame PRIVATE -Wall -Wextra)
endif()

# The game reads assets/ and data/ from its working directory,
# so copy them next to the executable after every build.
add_custom_command(TARGET MillionQuizGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:MillionQuizGame>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:MillionQuizGame>/data)

# Tests:  ctest --test-dir build
include(CTest)
if(BUILD_TESTING)
    add_executable(GameTests tests/tests.cpp)
    target_link_libraries(GameTests PRIVATE GameLogic)
    target_compile_definitions(GameTests PRIVATE QUESTION_DIR="${CMAKE_SOURCE_DIR}/data/questions")
    add_test(NAME GameTests COMMAND GameTests)
endif()
