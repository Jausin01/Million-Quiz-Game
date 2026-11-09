#pragma once
#include <optional>
#include "GameContext.h"
#include "Player.h"

/// Asks the player to create a profile or sign in with an existing one.
/// A failed sign-in continues as "Guest". Returns nothing if the window is closed.
std::optional<Player> RunLoginScreen(GameContext& context);
