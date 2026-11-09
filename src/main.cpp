#include <exception>
#include <iostream>
#include "Game.h"

int main()
{
    try
    {
        Game game;
        return game.Run();
    }
    catch (const std::exception& error)
    {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }
}
