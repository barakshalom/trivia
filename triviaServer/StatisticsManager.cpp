#include "StatisticsManager.h"

StatisticsManager::StatisticsManager(IDatabase* database) {
	m_database = database;
}

// Nothing to clean up.
StatisticsManager::~StatisticsManager() {
}

// Retrieves the top high scores from the database.
std::vector<std::string> StatisticsManager::getHighScore() const {
	std::lock_guard<std::mutex> lock(_mutex);
	return m_database->getHighScores(); // Delegates the logic to the database.
}

// Retrieves statistics for a specific user.
std::vector<std::string> StatisticsManager::getUserStatistics(const std::string& userName) const {
	std::vector<std::string> userStatistics;
	std::lock_guard<std::mutex> lock(_mutex);

	// Collects various statistics about the user from the database. 
	// 1. Number of games played.
	userStatistics.push_back(std::to_string(m_database->getNumOfPlayerGames(userName)));
	
	// 2. Number of correct answers.
	userStatistics.push_back(std::to_string(m_database->getNumOfCorrectAnswers(userName)));
	
	// 3. Number of total answers.
	userStatistics.push_back(std::to_string(m_database->getNumOfTotalAnswers(userName)));
	
	// 4. Average answer time.
	userStatistics.push_back(std::to_string(m_database->getPlayerAvrageAnswerTime(userName)));
	
	// Returns the collected statistics.
	return userStatistics;
}
