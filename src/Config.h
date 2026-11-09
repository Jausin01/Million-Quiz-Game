#pragma once
#include <string>

/// Window size, file locations and other fixed settings of the game.
namespace config
{
    constexpr unsigned kWindowWidth = 2560;
    constexpr unsigned kWindowHeight = 1440;
    constexpr unsigned kFrameRate = 60;
    inline const char* const kWindowTitle = "Million Quiz Game";

    // Everything is read relative to the folder the game runs from.
    inline const char* const kFontFile = "assets/Fonts/ComicRelief-Regular.ttf";
    inline const char* const kImagesDir = "assets/Images/";
    inline const char* const kPlayerDatabaseFile = "data/PlayerDB.txt";

    // Optional: the game runs without sound when these files are missing.
    inline const char* const kMenuMusicFile = "assets/Soundtrack/menu_theme.wav";
    inline const char* const kQuizMusicFile = "assets/Soundtrack/quiz_theme.wav";
    inline const char* const kRightAnswerSoundFile = "assets/Soundtrack/right_answer.wav";
    inline const char* const kGameOverSoundFile = "assets/Soundtrack/game_over.wav";

    inline std::string ImageFile(const std::string& name) { return kImagesDir + name; }

    inline std::string QuestionFile(int level)
    {
        return "data/questions/Level" + std::to_string(level) + "Questions.txt";
    }
}
