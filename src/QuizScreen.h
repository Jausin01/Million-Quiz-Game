#pragma once
#include <optional>
#include "GameContext.h"
#include "GameRules.h"
#include "Player.h"
#include "QuizSession.h"

/// Plays one game: shows the questions, runs the clock and the lifelines.
/// The player's profile is updated when the game ends.
/// Returns nothing if the window was closed before the game finished.
std::optional<GameResult> RunQuizScreen(GameContext& context, GameMode mode, Player& player);
