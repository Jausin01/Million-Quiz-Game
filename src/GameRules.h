#pragma once

/// How the clock behaves during a game.
enum class GameMode
{
    Classic,  // 60 seconds per question
    Speed,    // 30 seconds per question
    Practice, // No time limit
};

constexpr int kLevelCount = 15; // Questions needed to win the game

/// Seconds the player has for each question. Meaningless for Practice (no limit).
float TimeLimitSeconds(GameMode mode);

/// True when the mode has a countdown.
bool HasTimeLimit(GameMode mode);

/// Prize of a level (1..15), in dollars.
int PrizeForLevel(int level);

/// Money the player keeps after failing `failedLevel`.
/// Levels 5 and 10 are safe havens: passing them guarantees their prize.
int GuaranteedPrize(int failedLevel);
