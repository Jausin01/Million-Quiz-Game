#pragma once
#include <map>
#include <string>

/// A player profile. Name, best prize, deepest level and number of games are saved
/// between runs (see PlayerDatabase); category and lifeline statistics last one session.
class Player
{
public:
    Player() : Player("Guest") {}
    explicit Player(const std::string& name) : _name(name) {}
    Player(const std::string& name, int highestPrize, int mostDifficultLevel, int totalGames)
        : _name(name), _highestPrize(highestPrize), _mostDifficultLevel(mostDifficultLevel), _totalGames(totalGames) {}

    const std::string& GetName() const { return _name; }
    int GetHighestPrize() const { return _highestPrize; }
    int GetMostDifficultLevelReached() const { return _mostDifficultLevel; }
    int GetTotalGames() const { return _totalGames; }

    /// Adds a finished game to the profile.
    void RecordGame(int prize, int levelReached);

    void RecordCategoryResult(const std::string& category, bool success);

    /// Percentage (0..100) of questions of the category answered correctly.
    double GetCategorySuccessRate(const std::string& category) const;

    void UseLifeline(const std::string& lifelineName);
    int GetLifelineUsage(const std::string& lifelineName) const;

private:
    std::string _name;
    int _highestPrize = 0;
    int _mostDifficultLevel = 0;
    int _totalGames = 0;
    std::map<std::string, int> _categoryAttempts;
    std::map<std::string, int> _categorySuccess;
    std::map<std::string, int> _lifelineUsage;
};
