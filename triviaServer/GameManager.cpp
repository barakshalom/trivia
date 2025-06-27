#include "GameManager.h"

GameManager::GameManager(IDatabase* database)
{
	m_database = database;
	m_nextGameId = 1;
}

std::shared_ptr<Game> GameManager::createGame(Room* room)
{
	std::map<std::shared_ptr<LoggedUser>, GameData> loggedUsers;
	
	// Lock to ensure thread safety.
	std::lock_guard<std::mutex> lock(_mutex);

	std::shared_ptr<Game> game;

	// Retrieve questions for the game from the database.
	auto questions = m_database->getQuestions(room->getData().numOfQuestionsInGame);
	
	// Create default GameData for each user.
	GameData data;
	data.averageAnswerTime = 0;
	data.correctAnswerCount = 0;
	data.wrongAnswerCount = 0;
	data.currentQuestionIndex = 0;
	data.currentQuestion = std::make_shared<Question>(questions[data.currentQuestionIndex]);
	data.questionStartTime = std::chrono::steady_clock::now();
	
	// Assign default GameData to each user in the room.
	for (auto user : room->getAllUsers())
	{
		loggedUsers[user] = data;
	}

	// Create a new game object and store it.
	game = std::make_shared<Game>(questions, loggedUsers, m_nextGameId++);
	m_games.push_back(game);
	return game;
}

void GameManager::deleteGame(const unsigned int gameId)
{
	std::lock_guard<std::mutex> lock(_mutex);

	// Find and remove the game with the specified ID
	for (auto it = m_games.begin(); it != m_games.end(); ++it)
	{
		if (it->get()->getGameId() == gameId)
		{
			m_games.erase(it);
			break;
		}
	}
}

void GameManager::submitGameStatsToDB(PlayerResults data)
{
	std::lock_guard<std::mutex> lock(_mutex);

	// Pass the player's performance data to the database.
	m_database->statsToDatabase(data.username, data.correctAnswerCount + data.wrongAnswerCount, data.correctAnswerCount, data.averageAnswerCount);
}