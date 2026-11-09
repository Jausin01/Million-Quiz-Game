#pragma once
#include "GameContext.h"
#include "Player.h"
#include "QuizSession.h"

/// Shows how the game ended and the player's profile. Returns when the player
/// goes back to the menu (or closes the window).
void RunGameOverScreen(GameContext& context, const Player& player, const GameResult& result);
