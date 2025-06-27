#include "Game.h"


Game::Game(const std::vector<Question>& questions, const std::map<std::shared_ptr<LoggedUser>, GameData>& players, const unsigned int gameId) 
{
	m_questions = questions;
	m_players = players;
	m_gameId = gameId;
}

Question Game::getQuestionForUser(std::shared_ptr<LoggedUser> user)
{
	std::lock_guard<std::mutex> lock(_mutex);
	return *m_players[user].currentQuestion.get();
}

bool Game::submitAnswer(std::shared_ptr<LoggedUser> user, const unsigned int answerId)
{
	std::lock_guard<std::mutex> lock(_mutex);

	// Check if the user has more questions left.
	if (m_questions.size() > m_players[user].currentQuestionIndex)
	{
		// Answer ID 0 is treated as a timeout (no answer).
		if (answerId != 0)
		{
			// Check if answer is correct (offset by -1 to match 0-based index).
			if (answerId == m_players[user].currentQuestion->getCorrectAnswerId() - 1)
			{
				m_players[user].correctAnswerCount++;
			}
			else
			{
				m_players[user].wrongAnswerCount++;
			}
		}
		else
		{
			m_players[user].wrongAnswerCount++;
		}

		// Calculate how long it took to answer the question.
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> timeTaken = now - m_players[user].questionStartTime;

		// Update averageAnswerTime (actually a sum of all times, not yet averaged).
		m_players[user].currentQuestionIndex++;
		m_players[user].averageAnswerTime += timeTaken.count();
		
		// If there are more questions, update the currentQuestion and reset timer.
		if (m_questions.size() > m_players[user].currentQuestionIndex)
		{
			m_players[user].currentQuestion = std::make_shared<Question>(m_questions[m_players[user].currentQuestionIndex]);
			m_players[user].questionStartTime = std::chrono::steady_clock::now();
			return true;
		}

		// No more questions left
		return false;
	}
	// User already finished all questions
	return false;
}

int Game::getGameId() const
{
	return m_gameId;
}

// Builds and returns a list of PlayerResults for users who finished the game.
std::vector<PlayerResults> Game::getPlayerResults()
{
	std::vector<PlayerResults> results;
	std::lock_guard<std::mutex> lock(_mutex);

	for (auto player : m_players)
	{
		// Only include players who answered all questions
		if(m_players[player.first].currentQuestionIndex >= m_questions.size())
		{
			auto playerResults = m_players[player.first];
			PlayerResults result;
			int sumQuestions = playerResults.correctAnswerCount + playerResults.wrongAnswerCount;
			result.averageAnswerCount = (float)playerResults.averageAnswerTime / (float)sumQuestions;
			result.username = player.first->getUsername();
			result.correctAnswerCount = playerResults.correctAnswerCount;
			result.wrongAnswerCount = playerResults.wrongAnswerCount;
			results.push_back(result);
		}
		
	}
	return results;
}

// Removes a player from the game (e.g., if they left mid game)
void Game::removePlayer(std::shared_ptr<LoggedUser> user)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (m_players.find(user) != m_players.end())
	{
		m_players.erase(user);
	}
}

// Returns the number of players currently in the game.
int Game::getPlayerCount() const
{
	std::lock_guard<std::mutex> lock(_mutex);
	return m_players.size();
}
