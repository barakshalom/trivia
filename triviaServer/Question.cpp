#include "Question.h"

Question::Question(const std::string& question, const std::vector<std::string> answers, const int answerId)
{
	this->m_answer = answerId;
	this->m_possibleAnswers = answers;
	this->m_question = question;
}

// Returns the question text.
std::string Question::getQuestion() const
{
	return m_question;
}

// Returns the vector of possible answer choices.
std::vector<std::string> Question::getPossibleAnswers() const
{
	return m_possibleAnswers;
}

// Returns the ID (1-based) of the correct answer.
// Note: This assumes the correct answer index stored internally is 0-based.
int Question::getCorrectAnswerId() const
{
	return m_answer + 1;
}
