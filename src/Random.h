#pragma once
#include <random>

/// Small wrapper around std::mt19937 so the game (and the tests) share one source of randomness.
class Random
{
public:
    Random() : _engine(std::random_device{}()) {}
    explicit Random(unsigned seed) : _engine(seed) {}

    /// Uniform integer in [low, high].
    int Range(int low, int high)
    {
        return std::uniform_int_distribution<int>(low, high)(_engine);
    }

    /// True with the given probability (0..1).
    bool Chance(double probability)
    {
        return std::uniform_real_distribution<double>(0.0, 1.0)(_engine) < probability;
    }

private:
    std::mt19937 _engine;
};
