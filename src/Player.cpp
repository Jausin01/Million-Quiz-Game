#include "Player.h"
#include <algorithm>

void Player::RecordGame(int prize, int levelReached)
{
    ++_totalGames;
    _highestPrize = std::max(_highestPrize, prize);
    _mostDifficultLevel = std::max(_mostDifficultLevel, levelReached);
}

void Player::RecordCategoryResult(const std::string& category, bool success)
{
    ++_categoryAttempts[category];
    if (success)
    {
        ++_categorySuccess[category];
    }
}

double Player::GetCategorySuccessRate(const std::string& category) const
{
    auto attempts = _categoryAttempts.find(category);
    if (attempts == _categoryAttempts.end() || attempts->second == 0)
    {
        return 0.0;
    }

    auto successes = _categorySuccess.find(category);
    int successCount = successes == _categorySuccess.end() ? 0 : successes->second;
    return 100.0 * successCount / attempts->second;
}

void Player::UseLifeline(const std::string& lifelineName)
{
    ++_lifelineUsage[lifelineName];
}

int Player::GetLifelineUsage(const std::string& lifelineName) const
{
    auto it = _lifelineUsage.find(lifelineName);
    return it == _lifelineUsage.end() ? 0 : it->second;
}
