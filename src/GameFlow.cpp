#include "GameFlow.h"
#include "GameOverScreen.h"
#include "LoginScreen.h"
#include "QuizScreen.h"

void PlayGame(GameContext& context, GameMode mode)
{
    std::optional<Player> player = RunLoginScreen(context);
    if (!player)
    {
        return;
    }

    std::optional<GameResult> result = RunQuizScreen(context, mode, *player);
    if (!result)
    {
        return;
    }

    // Saved right away, so the game counts even if the window is closed on the result screen.
    context.players.Save(*player);
    RunGameOverScreen(context, *player, *result);
}
