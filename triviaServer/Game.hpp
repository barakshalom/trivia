#pragma once
#include <iostream>
#include <WinSock2.h>
#include "Question.h"

struct GameData {
	Question currentQuestion;
	unsigned int currentAnswerCount;
	unsigned int wrongAnswerCount;
	unsigned int averageAnswerCount;
};

class Game {
public:
	Game(const std::vector<Question>& questions, const std::map<LoggedUser, GameData>& players) {
		m_questions = questions;
		m_players = players;
	}
	
	Question getQuestionForUser(std::shared_ptr<LoggedUser> user);

	void submitAnswer();

	void removePlayer(std::shared_ptr<LoggedUser> user);

private:
	std::vector<Question> m_questions;
	std::map<LoggedUser, GameData> m_players;
};

void Game::removePlayer(std::shared_ptr<LoggedUser> user)
{
	if (m_players.find(*user) != m_players.end())
	{
		m_players.erase(*user);
	}
}

void Game::submitAnswer() {

}

Question Game::getQuestionForUser(std::shared_ptr<LoggedUser> user) {
	
}