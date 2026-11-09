#pragma once
#include <array>
#include <optional>
#include "GameRules.h"
#include "Lifelines.h"
#include "Player.h"
#include "Question.h"
#include "QuestionBank.h"
#include "Random.h"

/// How a finished game ended.
struct GameResult
{
    bool won = false;
    int levelReached = 0; // Level the game ended on
    int prize = 0;        // Money the player takes home
};

enum class AnswerResult
{
    Correct, // Right answer, the next question is ready
    Wrong,   // Wrong answer, game over
    Won,     // Right answer on the last level, game won
};

/// The rules of one game: current question, level progress, lifelines and the final result.
/// It knows nothing about windows or drawing, so it can be tested on its own.
class QuizSession
{
public:
    /// The bank must have questions for every level, and the player and bank must outlive the session.
    QuizSession(const QuestionBank& bank, Player& player, GameMode mode, Random& random);

    int GetLevel() const { return _level; }
    const Question& GetQuestion() const { return *_question; }
    GameMode GetMode() const { return _mode; }
    int GetLevelPrize() const { return PrizeForLevel(_level); }

    bool IsFinished() const { return _finished; }
    const GameResult& GetResult() const { return _result; }

    /// Answers the current question (answerIndex 0..3). Must not be called after the game ended.
    AnswerResult Submit(int answerIndex);

    /// The clock ran out: the game ends as if the answer was wrong.
    void TimeOut();

    bool IsLifelineAvailable(Lifeline lifeline) const;

    /// Each lifeline can be used once per game; they return nothing when already used.
    std::optional<std::array<int, 2>> UseFiftyFifty();
    std::optional<int> UsePhoneAFriend();
    std::optional<std::array<int, kAnswerCount>> UseAskAudience();

private:
    void Finish(bool won);
    bool TakeLifeline(Lifeline lifeline);

    const QuestionBank& _bank;
    Player& _player;
    GameMode _mode;
    Random& _random;

    int _level = 1;
    const Question* _question = nullptr;
    bool _finished = false;
    GameResult _result;
    std::array<bool, 3> _lifelineUsed = {};
};
