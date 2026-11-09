#pragma once
#include <array>
#include <string>

constexpr int kAnswerCount = 4;

/// A multiple-choice question with answers A to D.
struct Question
{
    std::string text;
    std::array<std::string, kAnswerCount> answers;
    int correctIndex = 0; // 0 = A, 1 = B, 2 = C, 3 = D
    std::string category;
};
