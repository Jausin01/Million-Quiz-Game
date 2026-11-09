#pragma once
#include <optional>
#include <string>
#include <vector>
#include "Player.h"

/// One line of the player file: Name|HighestPrize|MostDifficultLevel|TotalGames
struct PlayerRecord
{
    std::string name;
    int highestPrize = 0;
    int mostDifficultLevel = 0;
    int totalGames = 0;
};

/// Reads one line. Missing numbers count as 0; a line without a name or with
/// numbers that cannot be read is rejected.
bool ParsePlayerRecord(const std::string& line, PlayerRecord& outRecord);

std::string FormatPlayerRecord(const PlayerRecord& record);

/// Names are stored between '|' separators, so '|' cannot be part of a name.
bool IsValidPlayerNameChar(char c);

/// The text file where player profiles are kept between runs.
class PlayerDatabase
{
public:
    explicit PlayerDatabase(std::string path) : _path(std::move(path)) {}

    /// The saved profile with this exact name, if there is one.
    std::optional<Player> Find(const std::string& name) const;

    /// Adds the player or updates their line. Returns false if the file cannot be written.
    bool Save(const Player& player) const;

    /// Best players first (by highest prize), at most `maxCount` of them.
    std::vector<PlayerRecord> TopPlayers(size_t maxCount) const;

private:
    std::vector<PlayerRecord> ReadAll() const;

    std::string _path;
};
