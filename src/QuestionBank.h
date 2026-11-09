#pragma once
#include <iosfwd>
#include <map>
#include <string>
#include <vector>
#include "Question.h"
#include "Random.h"

/// Reads questions in the game's text format:
///
///   [Question1]
///   Title=...
///   AnswerA=...  AnswerB=...  AnswerC=...  AnswerD=...
///   RightAnswer=A
///   Category=History
///
/// Questions that are incomplete are skipped and reported to `warnings`.
std::vector<Question> ParseQuestions(std::istream& input, const std::string& sourceName, std::ostream& warnings);

/// All questions of the game, grouped by level (difficulty).
class QuestionBank
{
public:
    /// Loads the questions of one level from a file. Returns how many were accepted.
    size_t LoadLevel(int level, const std::string& filename);

    void AddQuestions(int level, std::vector<Question> questions);

    size_t Count(int level) const;

    /// True when every level from 1 to `levelCount` has at least one question.
    bool HasAllLevels(int levelCount) const;

    /// Random question of a level. The level must have questions (see Count).
    const Question& RandomQuestion(int level, Random& random) const;

private:
    std::map<int, std::vector<Question>> _questions;
};
