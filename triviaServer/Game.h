#pragma once
#include <iostream>
#include <WinSock2.h>
#include "Question.h"
#include <map>
#include "LoggedUser.hpp"
#include <vector>
#include "JsonResponsePacketSerializer.h"
#include <chrono>
#include "IDatabase.h"
#include <mutex>

// Structure that holds game-related data for a single player.
typedef struct GameData {
	std::shared_ptr<Question> currentQuestion;
	unsigned int correctAnswerCount;
	unsigned int wrongAnswerCount;
	double averageAnswerTime;
	std::chrono::steady_clock::time_point questionStartTime;
	unsigned int currentQuestionIndex;
} GameData;

// Represents a single game session.
class Game {
public:
	// Constructor and destructor.
	Game(const std::vector<Question>& questions, const std::map<std::shared_ptr<LoggedUser>, GameData>& players, const unsigned int gameId);
	
	// Returns the next question for the given user.
	Question getQuestionForUser(std::shared_ptr<LoggedUser> user);

	// Submits the user's answer and returns whether it was correct or not.
	bool submitAnswer(std::shared_ptr<LoggedUser> user, const unsigned int answerId);

	// Returns the id of the game
	int getGameId() const;

	// Returns final results for all players in the game.
	std::vector<PlayerResults> getPlayerResults();

	// Removes a player from the game.
	void removePlayer(std::shared_ptr<LoggedUser> user);

	// Returns the number of players still in the game.
	int getPlayerCount() const;

private:
	std::vector<Question> m_questions;
	std::map<std::shared_ptr<LoggedUser>, GameData> m_players;
	unsigned int m_gameId;
	mutable std::mutex _mutex;
};