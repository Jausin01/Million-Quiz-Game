#include "QuestionBank.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace
{
    const char kTitleKey[] = "Title=";
    const char kRightAnswerKey[] = "RightAnswer=";
    const char kCategoryKey[] = "Category=";
    const char kAnswerKeyPrefix[] = "Answer";

    bool StartsWith(const std::string& text, const std::string& prefix)
    {
        return text.rfind(prefix, 0) == 0;
    }

    /// Some question titles carry a " (var..." suffix from the original question set.
    std::string RemoveVariantSuffix(const std::string& title)
    {
        size_t position = title.find(" (var");
        return position == std::string::npos ? title : title.substr(0, position);
    }

    /// The question being read, plus what was found so far.
    struct PendingQuestion
    {
        Question question;
        bool hasTitle = false;
        bool hasAnswer[kAnswerCount] = {};
        bool hasRightAnswer = false;

        bool IsComplete() const
        {
            for (bool found : hasAnswer)
            {
                if (!found)
                {
                    return false;
                }
            }
            return hasTitle && hasRightAnswer;
        }
    };
}

std::vector<Question> ParseQuestions(std::istream& input, const std::string& sourceName, std::ostream& warnings)
{
    std::vector<Question> questions;
    PendingQuestion pending;
    bool inQuestion = false;
    int questionNumber = 0;

    auto finishQuestion = [&]()
    {
        if (!inQuestion)
        {
            return;
        }
        if (pending.IsComplete())
        {
            questions.push_back(pending.question);
        }
        else
        {
            warnings << sourceName << ": skipped incomplete question #" << questionNumber << "\n";
        }
        pending = PendingQuestion{};
    };

    std::string line;
    while (std::getline(input, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        if (line.empty())
        {
            continue;
        }

        if (StartsWith(line, "[Question"))
        {
            finishQuestion();
            inQuestion = true;
            ++questionNumber;
        }
        else if (StartsWith(line, kTitleKey))
        {
            pending.question.text = RemoveVariantSuffix(line.substr(sizeof(kTitleKey) - 1));
            pending.hasTitle = !pending.question.text.empty();
        }
        else if (StartsWith(line, kAnswerKeyPrefix) && line.size() > 7 && line[6] >= 'A' && line[6] <= 'D' && line[7] == '=')
        {
            int index = line[6] - 'A';
            pending.question.answers[index] = line.substr(8);
            pending.hasAnswer[index] = true;
        }
        else if (StartsWith(line, kRightAnswerKey))
        {
            std::string value = line.substr(sizeof(kRightAnswerKey) - 1);
            if (!value.empty() && value[0] >= 'A' && value[0] <= 'D')
            {
                pending.question.correctIndex = value[0] - 'A';
                pending.hasRightAnswer = true;
            }
        }
        else if (StartsWith(line, kCategoryKey))
        {
            pending.question.category = line.substr(sizeof(kCategoryKey) - 1);
        }
    }
    finishQuestion();

    return questions;
}

size_t QuestionBank::LoadLevel(int level, const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Could not open question file: " << filename << "\n";
        return 0;
    }

    std::vector<Question> questions = ParseQuestions(file, filename, std::cerr);
    size_t accepted = questions.size();
    AddQuestions(level, std::move(questions));
    return accepted;
}

void QuestionBank::AddQuestions(int level, std::vector<Question> questions)
{
    std::vector<Question>& target = _questions[level];
    target.insert(target.end(), questions.begin(), questions.end());
}

size_t QuestionBank::Count(int level) const
{
    auto it = _questions.find(level);
    return it == _questions.end() ? 0 : it->second.size();
}

bool QuestionBank::HasAllLevels(int levelCount) const
{
    for (int level = 1; level <= levelCount; ++level)
    {
        if (Count(level) == 0)
        {
            return false;
        }
    }
    return true;
}

const Question& QuestionBank::RandomQuestion(int level, Random& random) const
{
    const std::vector<Question>& candidates = _questions.at(level);
    return candidates[random.Range(0, (int)candidates.size() - 1)];
}
