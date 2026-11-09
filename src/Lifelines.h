#pragma once
#include <array>
#include "Question.h"
#include "Random.h"

enum class Lifeline
{
    FiftyFifty,
    PhoneAFriend,
    AskAudience,
};

/// Name used when recording lifeline usage.
const char* LifelineName(Lifeline lifeline);

/// 50/50: the indexes of two wrong answers to hide.
std::array<int, 2> PickFiftyFiftyRemovals(int correctIndex, Random& random);

/// Phone a friend: the answer index the friend suggests. Right about 80% of the time.
int PickFriendAnswer(int correctIndex, Random& random);

/// Ask the audience: percentage of votes per answer. Always adds up to 100,
/// and the right answer gets between 40% and 70%.
std::array<int, kAnswerCount> PickAudienceVotes(int correctIndex, Random& random);
