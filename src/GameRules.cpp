#include "GameRules.h"
#include <algorithm>
#include <array>

namespace
{
    constexpr std::array<int, kLevelCount> kPrizeLadder =
    {
        100, 200, 300, 500, 1000, 2000, 4000, 8000, 16000, 32000, 64000, 125000, 250000, 500000, 1000000
    };

    constexpr std::array<int, 2> kSafeHavenLevels = { 5, 10 };
}

float TimeLimitSeconds(GameMode mode)
{
    switch (mode)
    {
    case GameMode::Classic: return 60.f;
    case GameMode::Speed:   return 30.f;
    case GameMode::Practice:
    default:                return 99.f;
    }
}

bool HasTimeLimit(GameMode mode)
{
    return mode != GameMode::Practice;
}

int PrizeForLevel(int level)
{
    level = std::clamp(level, 1, kLevelCount);
    return kPrizeLadder[level - 1];
}

int GuaranteedPrize(int failedLevel)
{
    // The highest safe haven the player has already passed.
    for (auto it = kSafeHavenLevels.rbegin(); it != kSafeHavenLevels.rend(); ++it)
    {
        if (failedLevel > *it)
        {
            return PrizeForLevel(*it);
        }
    }
    return 0;
}
