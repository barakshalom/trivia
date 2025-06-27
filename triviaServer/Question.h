#pragma once
#include <string>
#include <vector>

// This class represents a single trivia question including its text, possible answers, and the correct answer id.
class Question
{
public:
	// Constructor
	Question(const std::string& question, const std::vector<std::string> answers, const int answerId);
	
	// Returns the question text.
	std::string getQuestion() const;

	// Returns a list of all possible answers.
	std::vector<std::string> getPossibleAnswers() const;
	
	// Returns the index (id) of the correct answer in the possible answers list.
	int getCorrectAnswerId() const;

private:
	// The text of the question.
	std::string m_question;

	// A vector containing all possible answer options.
	std::vector<std::string> m_possibleAnswers;

	// The index of the correct answer in the list.
	int m_answer;
};