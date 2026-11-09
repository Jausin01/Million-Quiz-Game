// Plain assert-style tests for the game rules (no window needed).
#include <cstdio>
#include <iostream>
#include <set>
#include <sstream>
#include "GameRules.h"
#include "Lifelines.h"
#include "PlayerDatabase.h"
#include "QuestionBank.h"
#include "QuizSession.h"

#ifndef QUESTION_DIR
#define QUESTION_DIR "data/questions"
#endif

namespace
{
    int failures = 0;

    void Check(bool condition, const char* what)
    {
        if (!condition)
        {
            std::cerr << "FAILED: " << what << "\n";
            ++failures;
        }
    }

    Question MakeQuestion(int correctIndex, const std::string& category = "Test")
    {
        Question q;
        q.text = "Question?";
        q.answers = { "A", "B", "C", "D" };
        q.correctIndex = correctIndex;
        q.category = category;
        return q;
    }

    /// A bank with one question per level whose right answer is always B (index 1).
    QuestionBank MakeBank()
    {
        QuestionBank bank;
        for (int level = 1; level <= kLevelCount; ++level)
        {
            bank.AddQuestions(level, { MakeQuestion(1) });
        }
        return bank;
    }

    void TestRules()
    {
        Check(TimeLimitSeconds(GameMode::Classic) == 60.f, "classic time");
        Check(TimeLimitSeconds(GameMode::Speed) == 30.f, "speed time");
        Check(!HasTimeLimit(GameMode::Practice) && HasTimeLimit(GameMode::Speed), "practice has no clock");

        Check(PrizeForLevel(1) == 100 && PrizeForLevel(5) == 1000 && PrizeForLevel(10) == 32000 &&
              PrizeForLevel(15) == 1000000, "prize ladder");

        // Failing before level 6 keeps nothing, then the safe havens kick in.
        Check(GuaranteedPrize(1) == 0 && GuaranteedPrize(5) == 0, "no safe haven yet");
        Check(GuaranteedPrize(6) == 1000 && GuaranteedPrize(10) == 1000, "first safe haven");
        Check(GuaranteedPrize(11) == 32000 && GuaranteedPrize(15) == 32000, "second safe haven");
    }

    void TestParser()
    {
        std::istringstream good(
            "[Question1]\r\nTitle=What is 5 + 3? (var2)\r\nAnswerA=7\r\nAnswerB=8\r\nAnswerC=9\r\nAnswerD=6\r\n"
            "RightAnswer=B\r\nCategory=Math\r\n\r\n"
            "[Question2]\nTitle=Broken\nAnswerA=1\nAnswerB=2\nRightAnswer=A\n"      // only two answers
            "[Question3]\nTitle=Bad letter\nAnswerA=1\nAnswerB=2\nAnswerC=3\nAnswerD=4\nRightAnswer=Z\n"
            "[Question4]\nTitle=Last one\nAnswerA=a\nAnswerB=b\nAnswerC=c\nAnswerD=d\nRightAnswer=D\nCategory=Test\n");
        std::ostringstream warnings;
        std::vector<Question> questions = ParseQuestions(good, "test", warnings);

        Check(questions.size() == 2, "only complete questions are kept");
        Check(questions[0].text == "What is 5 + 3?", "variant suffix removed, no trailing CR");
        Check(questions[0].answers[1] == "8" && questions[0].correctIndex == 1, "answers and right answer");
        Check(questions[0].category == "Math", "category");
        Check(questions[1].correctIndex == 3, "last question without trailing blank line");
        Check(!warnings.str().empty(), "skipped questions are reported");
    }

    void TestRealQuestionFiles()
    {
        QuestionBank bank;
        for (int level = 1; level <= kLevelCount; ++level)
        {
            bank.LoadLevel(level, std::string(QUESTION_DIR) + "/Level" + std::to_string(level) + "Questions.txt");
            Check(bank.Count(level) > 0, "every level file has questions");
        }
        Check(bank.HasAllLevels(kLevelCount), "bank is complete");
    }

    void TestLifelines()
    {
        Random random(1234);
        for (int trial = 0; trial < 500; ++trial)
        {
            int correct = trial % kAnswerCount;

            std::array<int, 2> removed = PickFiftyFiftyRemovals(correct, random);
            Check(removed[0] != removed[1] && removed[0] != correct && removed[1] != correct, "50/50 hides two wrong answers");

            std::array<int, kAnswerCount> votes = PickAudienceVotes(correct, random);
            int sum = 0;
            bool nonNegative = true;
            for (int v : votes) { sum += v; nonNegative = nonNegative && v >= 0; }
            Check(sum == 100 && nonNegative, "audience votes add up to 100");
            Check(votes[correct] >= 40 && votes[correct] <= 70, "audience favours the right answer");

            int friendAnswer = PickFriendAnswer(correct, random);
            Check(friendAnswer >= 0 && friendAnswer < kAnswerCount, "friend picks a valid answer");
        }

        int right = 0;
        for (int trial = 0; trial < 5000; ++trial)
        {
            right += PickFriendAnswer(2, random) == 2 ? 1 : 0;
        }
        Check(right > 3800 && right < 4200, "friend is right about 80% of the time");
    }

    void TestPlayerAndDatabase()
    {
        Player player("Ana");
        player.RecordGame(1000, 7);
        player.RecordGame(0, 3);
        Check(player.GetTotalGames() == 2 && player.GetHighestPrize() == 1000 &&
              player.GetMostDifficultLevelReached() == 7, "player stats keep the best values");

        player.RecordCategoryResult("Math", true);
        player.RecordCategoryResult("Math", false);
        Check(player.GetCategorySuccessRate("Math") == 50.0 && player.GetCategorySuccessRate("Art") == 0.0, "category rate");

        PlayerRecord record;
        Check(ParsePlayerRecord("Mimi|1000000|15|3", record) && record.name == "Mimi" &&
              record.highestPrize == 1000000 && record.totalGames == 3, "parse a record");
        Check(ParsePlayerRecord("Solo", record) && record.totalGames == 0, "missing numbers count as 0");
        Check(!ParsePlayerRecord("Bob|abc|1|1", record), "bad number rejected");
        Check(!ParsePlayerRecord("|1|1|1", record), "empty name rejected");
        Check(FormatPlayerRecord({ "Zed", 5, 2, 1 }) == "Zed|5|2|1", "format a record");
        Check(!IsValidPlayerNameChar('|') && IsValidPlayerNameChar('a'), "name characters");

        const std::string path = "players_test.txt";
        std::remove(path.c_str());
        PlayerDatabase db(path);
        Check(!db.Find("Ana").has_value(), "missing file finds nobody");
        db.Save(Player("Low", 100, 2, 1));
        db.Save(Player("High", 32000, 11, 4));
        db.Save(Player("Mid", 1000, 6, 2));
        db.Save(Player("Low", 500, 3, 2)); // update
        Check(db.Find("Low").has_value() && db.Find("Low")->GetHighestPrize() == 500, "save updates an existing player");
        std::vector<PlayerRecord> top = db.TopPlayers(2);
        Check(top.size() == 2 && top[0].name == "High" && top[1].name == "Mid", "leaderboard order");
        std::remove(path.c_str());
    }

    void TestFullGameWin()
    {
        QuestionBank bank = MakeBank();
        Random random(7);
        Player player("Winner");
        QuizSession session(bank, player, GameMode::Classic, random);

        for (int level = 1; level < kLevelCount; ++level)
        {
            Check(session.GetLevel() == level, "level goes up by exactly one per right answer");
            Check(session.Submit(1) == AnswerResult::Correct, "right answer");
        }
        Check(session.GetLevel() == kLevelCount, "reached the last level");
        Check(session.Submit(1) == AnswerResult::Won, "winning answer");
        Check(session.IsFinished() && session.GetResult().won && session.GetResult().prize == 1000000, "won the million");
        Check(player.GetTotalGames() == 1 && player.GetHighestPrize() == 1000000 &&
              player.GetMostDifficultLevelReached() == 15, "profile updated once");
    }

    void TestLossAndTimeout()
    {
        QuestionBank bank = MakeBank();
        Random random(7);

        {   // Wrong answer on level 7: keeps the level 5 safe haven.
            Player player("Loser");
            QuizSession session(bank, player, GameMode::Classic, random);
            for (int i = 1; i < 7; ++i) session.Submit(1);
            Check(session.Submit(0) == AnswerResult::Wrong, "wrong answer");
            Check(session.GetResult().prize == 1000 && session.GetResult().levelReached == 7 && !session.GetResult().won,
                "wrong answer keeps the safe haven prize");
            Check(player.GetHighestPrize() == 1000, "profile gets the guaranteed prize, not the last level prize");
        }
        {   // Timeout on level 1.
            Player player("Slow");
            QuizSession session(bank, player, GameMode::Speed, random);
            session.TimeOut();
            Check(session.IsFinished() && session.GetResult().prize == 0 && player.GetTotalGames() == 1,
                "timeout counts as a finished game");
        }
    }

    void TestLifelineUse()
    {
        QuestionBank bank = MakeBank();
        Random random(9);
        Player player("Helper");
        QuizSession session(bank, player, GameMode::Practice, random);

        Check(session.IsLifelineAvailable(Lifeline::FiftyFifty), "available at the start");
        Check(session.UseFiftyFifty().has_value(), "first use works");
        Check(!session.UseFiftyFifty().has_value(), "second use is refused");
        Check(!session.IsLifelineAvailable(Lifeline::FiftyFifty), "marked as used");
        Check(session.IsLifelineAvailable(Lifeline::AskAudience), "other lifelines unaffected");
        Check(player.GetLifelineUsage("50/50") == 1, "usage recorded once");

        session.Submit(1); // Lifelines stay used on the next question
        Check(!session.UseFiftyFifty().has_value(), "still used on the next question");
    }
}

int main()
{
    TestRules();
    TestParser();
    TestRealQuestionFiles();
    TestLifelines();
    TestPlayerAndDatabase();
    TestFullGameWin();
    TestLossAndTimeout();
    TestLifelineUse();

    if (failures == 0)
    {
        std::cout << "All tests passed.\n";
    }
    return failures == 0 ? 0 : 1;
}
