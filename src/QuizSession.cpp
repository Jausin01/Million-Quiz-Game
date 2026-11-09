#include "QuizSession.h"

QuizSession::QuizSession(const QuestionBank& bank, Player& player, GameMode mode, Random& random)
    : _bank(bank), _player(player), _mode(mode), _random(random)
{
    _question = &_bank.RandomQuestion(_level, _random);
}

AnswerResult QuizSession::Submit(int answerIndex)
{
    const bool correct = answerIndex == _question->correctIndex;
    _player.RecordCategoryResult(_question->category, correct);

    if (!correct)
    {
        Finish(false);
        return AnswerResult::Wrong;
    }

    if (_level >= kLevelCount)
    {
        Finish(true);
        return AnswerResult::Won;
    }

    ++_level;
    _question = &_bank.RandomQuestion(_level, _random);
    return AnswerResult::Correct;
}

void QuizSession::TimeOut()
{
    _player.RecordCategoryResult(_question->category, false);
    Finish(false);
}

void QuizSession::Finish(bool won)
{
    _finished = true;
    _result.won = won;
    _result.levelReached = _level;
    _result.prize = won ? PrizeForLevel(kLevelCount) : GuaranteedPrize(_level);
    _player.RecordGame(_result.prize, _result.levelReached);
}

bool QuizSession::IsLifelineAvailable(Lifeline lifeline) const
{
    return !_finished && !_lifelineUsed[(int)lifeline];
}

bool QuizSession::TakeLifeline(Lifeline lifeline)
{
    if (!IsLifelineAvailable(lifeline))
    {
        return false;
    }
    _lifelineUsed[(int)lifeline] = true;
    _player.UseLifeline(LifelineName(lifeline));
    return true;
}

std::optional<std::array<int, 2>> QuizSession::UseFiftyFifty()
{
    if (!TakeLifeline(Lifeline::FiftyFifty))
    {
        return std::nullopt;
    }
    return PickFiftyFiftyRemovals(_question->correctIndex, _random);
}

std::optional<int> QuizSession::UsePhoneAFriend()
{
    if (!TakeLifeline(Lifeline::PhoneAFriend))
    {
        return std::nullopt;
    }
    return PickFriendAnswer(_question->correctIndex, _random);
}

std::optional<std::array<int, kAnswerCount>> QuizSession::UseAskAudience()
{
    if (!TakeLifeline(Lifeline::AskAudience))
    {
        return std::nullopt;
    }
    return PickAudienceVotes(_question->correctIndex, _random);
}
