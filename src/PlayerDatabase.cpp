#include "PlayerDatabase.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace
{
    /// Reads a whole-number field. An empty field counts as 0.
    bool ParseNumber(const std::string& text, int& outValue)
    {
        if (text.empty())
        {
            outValue = 0;
            return true;
        }
        try
        {
            size_t used = 0;
            outValue = std::stoi(text, &used);
            return used == text.size();
        }
        catch (const std::exception&)
        {
            return false;
        }
    }
}

bool ParsePlayerRecord(const std::string& line, PlayerRecord& outRecord)
{
    std::stringstream stream(line);
    std::string fields[4];
    for (std::string& field : fields)
    {
        std::getline(stream, field, '|');
    }

    if (fields[0].empty())
    {
        return false;
    }

    PlayerRecord record;
    record.name = fields[0];
    if (!ParseNumber(fields[1], record.highestPrize) ||
        !ParseNumber(fields[2], record.mostDifficultLevel) ||
        !ParseNumber(fields[3], record.totalGames))
    {
        return false;
    }

    outRecord = record;
    return true;
}

std::string FormatPlayerRecord(const PlayerRecord& record)
{
    return record.name + "|" + std::to_string(record.highestPrize) + "|" +
        std::to_string(record.mostDifficultLevel) + "|" + std::to_string(record.totalGames);
}

bool IsValidPlayerNameChar(char c)
{
    return c >= 32 && c <= 126 && c != '|';
}

std::vector<PlayerRecord> PlayerDatabase::ReadAll() const
{
    std::vector<PlayerRecord> records;
    std::ifstream file(_path);
    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        PlayerRecord record;
        if (ParsePlayerRecord(line, record))
        {
            records.push_back(record);
        }
    }
    return records;
}

std::optional<Player> PlayerDatabase::Find(const std::string& name) const
{
    for (const PlayerRecord& record : ReadAll())
    {
        if (record.name == name)
        {
            return Player(record.name, record.highestPrize, record.mostDifficultLevel, record.totalGames);
        }
    }
    return std::nullopt;
}

bool PlayerDatabase::Save(const Player& player) const
{
    PlayerRecord updated;
    updated.name = player.GetName();
    updated.highestPrize = player.GetHighestPrize();
    updated.mostDifficultLevel = player.GetMostDifficultLevelReached();
    updated.totalGames = player.GetTotalGames();

    std::vector<PlayerRecord> records = ReadAll();
    auto existing = std::find_if(records.begin(), records.end(),
        [&](const PlayerRecord& record) { return record.name == updated.name; });
    if (existing != records.end())
    {
        *existing = updated;
    }
    else
    {
        records.push_back(updated);
    }

    std::filesystem::path path(_path);
    if (path.has_parent_path())
    {
        std::error_code ignored;
        std::filesystem::create_directories(path.parent_path(), ignored);
    }

    std::ofstream file(_path, std::ios::trunc);
    if (!file.is_open())
    {
        std::cerr << "Could not write " << _path << "\n";
        return false;
    }
    for (const PlayerRecord& record : records)
    {
        file << FormatPlayerRecord(record) << "\n";
    }
    return true;
}

std::vector<PlayerRecord> PlayerDatabase::TopPlayers(size_t maxCount) const
{
    std::vector<PlayerRecord> records = ReadAll();
    std::stable_sort(records.begin(), records.end(),
        [](const PlayerRecord& a, const PlayerRecord& b) { return a.highestPrize > b.highestPrize; });
    if (records.size() > maxCount)
    {
        records.resize(maxCount);
    }
    return records;
}
