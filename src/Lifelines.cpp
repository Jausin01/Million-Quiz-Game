#include "Lifelines.h"
#include <algorithm>
#include <vector>

namespace
{
    constexpr double kFriendCorrectChance = 0.8;
    constexpr int kAudienceMinCorrectPercent = 40;
    constexpr int kAudienceMaxCorrectPercent = 70;

    /// Indexes of the three wrong answers, in random order.
    std::vector<int> ShuffledWrongIndexes(int correctIndex, Random& random)
    {
        std::vector<int> wrong;
        for (int i = 0; i < kAnswerCount; ++i)
        {
            if (i != correctIndex)
            {
                wrong.push_back(i);
            }
        }
        for (int i = (int)wrong.size() - 1; i > 0; --i)
        {
            std::swap(wrong[i], wrong[random.Range(0, i)]);
        }
        return wrong;
    }
}

const char* LifelineName(Lifeline lifeline)
{
    switch (lifeline)
    {
    case Lifeline::FiftyFifty:   return "50/50";
    case Lifeline::PhoneAFriend: return "Phone a Friend";
    case Lifeline::AskAudience:  return "Ask Audience";
    }
    return "";
}

std::array<int, 2> PickFiftyFiftyRemovals(int correctIndex, Random& random)
{
    std::vector<int> wrong = ShuffledWrongIndexes(correctIndex, random);
    return { wrong[0], wrong[1] };
}

int PickFriendAnswer(int correctIndex, Random& random)
{
    if (random.Chance(kFriendCorrectChance))
    {
        return correctIndex;
    }
    return ShuffledWrongIndexes(correctIndex, random)[0];
}

std::array<int, kAnswerCount> PickAudienceVotes(int correctIndex, Random& random)
{
    std::array<int, kAnswerCount> votes = {};
    votes[correctIndex] = random.Range(kAudienceMinCorrectPercent, kAudienceMaxCorrectPercent);

    // The rest is split randomly between the three wrong answers.
    int remaining = 100 - votes[correctIndex];
    std::vector<int> wrong = ShuffledWrongIndexes(correctIndex, random);
    for (size_t i = 0; i + 1 < wrong.size(); ++i)
    {
        int share = random.Range(0, remaining);
        votes[wrong[i]] = share;
        remaining -= share;
    }
    votes[wrong.back()] = remaining;

    return votes;
}
