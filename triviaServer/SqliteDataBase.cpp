#include "SqliteDataBase.h"
#include <sstream>
#include "DataBaseException.hpp"
#include "io.h"
#include <random>


SqliteDataBase::SqliteDataBase() {
	std::string dbFileName = "TriviaDB.sqlite";

	// Check whether the DB file already exists on disk
	int doesFileExist = _access(dbFileName.c_str(), 0);

	// Try opening (or creating) the SQLite database
	if (sqlite3_open(dbFileName.c_str(), &this->db) != SQLITE_OK)
	{
		std::string msg = std::string("Failed to open DB: ") + sqlite3_errmsg(this->db);
		this->db = nullptr;
		throw DataBaseException(msg);
	}

	// If the file is new, create all tables
	if (doesFileExist != 0) {
		// Creating all tables
		const char* sqlStatement = R"(
			CREATE TABLE USERS (
				USER_ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
				NAME TEXT NOT NULL,
				PASSWORDD TEXT NOT NULL,
				EMAIL TEXT NOT NULL UNIQUE,
				ADDRESS TEXT NOT NULL,
				PHONE TEXT NOT NULL UNIQUE,
				BIRTH_DATE TEXT NOT NULL
			);

			CREATE TABLE GAME_STATISTICS (
				USER_ID INTEGER NOT NULL,
				AVERAGE_ANSWER_TIME FLOAT NOT NULL DEFAULT 0,
				CORRECT_ANSWERS INTEGER NOT NULL DEFAULT 0,
				TOTAL_ANSWERS INTEGER NOT NULL DEFAULT 0,
				FOREIGN KEY (USER_ID) REFERENCES USERS(USER_ID) ON DELETE CASCADE
			);

			CREATE TABLE USER_STATISTICS (
				USER_ID INTEGER NOT NULL,
				PLAYER_GAMES INTEGER NOT NULL,
				TOTAL_SCORE INTEGER NOT NULL,
				FOREIGN KEY (USER_ID) REFERENCES USERS(USER_ID) ON DELETE CASCADE
			);
 
			CREATE TABLE QUESTIONS (
				id INTEGER PRIMARY KEY AUTOINCREMENT,
				QUESTION TEXT NOT NULL,
				ANSWER_FIRST TEXT NOT NULL,
				ANSWER_SECOND TEXT NOT NULL,
				ANSWER_THIRD TEXT NOT NULL,
				ANSWER_FOURTH TEXT NOT NULL,
				CORRECT INTEGER NOT NULL CHECK (CORRECT BETWEEN 1 AND 4)
			);
		)";

		Query queryRequest = Query(sqlStatement, nullptr, callbackCount);

		createQuery(queryRequest);
	}

	// Download and populate questions table (ignores if already filled).
	getQuestionsDatabase();
}
void SqliteDataBase::close() {
	// Close db
	if (this->db) 
	{
		// Optional: wipe question table on shutdown.
		const char* sqlStatement = R"(DELETE FROM QUESTIONS;)";
		Query queryRequest = Query(sqlStatement, nullptr, callbackCount);

		createQuery(queryRequest);

		sqlite3_close(this->db);
		this->db = nullptr;
	}
}

// Clear all users.
void SqliteDataBase::clear() {
	if (!this->db) return;

	const char* sqlStatement = R"(DELETE FROM USERS;)";
	Query queryRequest = Query(sqlStatement, nullptr, callbackCount);

	createQuery(queryRequest);
}

// User lookup helpers.
bool SqliteDataBase::doesUserExist(const std::string& userName) {
	// Creating SQL message that returns 1 if user exists and 0 if not.
	std::stringstream ss;
	ss << "SELECT COUNT(*) FROM USERS "
		<< "WHERE USERS.NAME = '" << userName << "';";

	std::string query = ss.str();
	int count = 0;
	Query queryRequest = Query(query.c_str(), &count, callbackCount);

	createQuery(queryRequest);


	return count > 0;
}

bool SqliteDataBase::doesPasswordMatch(const std::string& userName, const std::string& password) {
	// Creating SQL message that returns 1 if the password is matching and 0 if not 

	std::stringstream ss;
	ss << "SELECT COUNT(*) FROM USERS "
		<< "WHERE USERS.NAME = '" << userName << "' AND USERS.PASSWORDD = '" << password << "';";

	std::string query = ss.str();
	const char* sqlStatement = query.c_str();
	int count = 0;
	Query queryRequest = Query(query.c_str(), &count, callbackCount);

	createQuery(queryRequest);

	return count > 0; 
}

// User registration.
void SqliteDataBase::addNewUser(const std::string& userName, const std::string& password, const std::string& email, const std::string& address, const std::string& phoneNumber, const std::string& birthDate)
{
	// Creating SQL message that adds user.

	std::stringstream ss;
	ss << "INSERT INTO USERS(NAME, PASSWORDD, EMAIL, ADDRESS, PHONE, BIRTH_DATE) "
	   << "VALUES('" << userName << "', '" << password << "', '" << email << "', '"
	   << address << "', '" << phoneNumber << "', '" << birthDate << "');";

	std::string query = ss.str();
	Query queryRequest = Query(query.c_str(), nullptr, callbackCount);
	createQuery(queryRequest);
}

// Question retrieval.
std::vector<Question> SqliteDataBase::getQuestions(const int numOfQ)
{
	std::stringstream ss;
	std::vector<Question> questions;
	ss << "SELECT * FROM QUESTIONS LIMIT " << numOfQ;
	std::string query = ss.str();
	Query queryRequest = Query(query.c_str(), &questions, callbackQuestions);
	createQuery(queryRequest);

	return questions;
}

// Statistics queries.
float SqliteDataBase::getPlayerAvrageAnswerTime(const std::string& userName) {
	// Creating SQL message that gets the avg answer time of user

	std::stringstream ss;
	ss << "SELECT AVERAGE_ANSWER_TIME FROM GAME_STATISTICS "
	   << "WHERE USER_ID = (SELECT USER_ID FROM USERS WHERE NAME = '" << userName << "');";

	std::string query = ss.str();
	const char* sqlStatement = query.c_str();

	float averageAnswerTime = 0;
	Query queryRequest = Query(query.c_str(), &averageAnswerTime, callbackAverage);

	createQuery(queryRequest);

	return averageAnswerTime;
}

int SqliteDataBase::getNumOfTotalAnswers(const std::string& userName) {
	// Creating SQL message that gets the num of total answers of user

	std::stringstream ss;
	ss << "SELECT TOTAL_ANSWERS FROM GAME_STATISTICS "
	   << "WHERE USER_ID = (SELECT USER_ID FROM USERS WHERE NAME = '" << userName << "');";

	std::string query = ss.str();
	const char* sqlStatement = query.c_str();

	int totalAnswers = 0;
	Query queryRequest = Query(query.c_str(), &totalAnswers, callbackCount);

	createQuery(queryRequest);

	return totalAnswers;
}

int SqliteDataBase::getNumOfCorrectAnswers(const std::string& userName) {
	// Creating SQL message that gets the num of correct answers of user

	std::stringstream ss;
	ss << "SELECT CORRECT_ANSWERS FROM GAME_STATISTICS "
	   << "WHERE USER_ID = (SELECT USER_ID FROM USERS WHERE NAME = '" << userName << "');";

	std::string query = ss.str();
	const char* sqlStatement = query.c_str();

	int correctAnswers = 0;
	Query queryRequest = Query(query.c_str(), &correctAnswers, callbackCount);

	createQuery(queryRequest);

	return correctAnswers;
}

int SqliteDataBase::getNumOfPlayerGames(const std::string& userName) {
	// Creating SQL message that gets the num of games of user

	std::stringstream ss;
	ss << "SELECT PLAYER_GAMES FROM USER_STATISTICS "
	   << "WHERE USER_ID = (SELECT USER_ID FROM USERS WHERE NAME = '" << userName << "');";

	std::string query = ss.str();
	const char* sqlStatement = query.c_str();

	int playerGames = 0;
	Query queryRequest = Query(query.c_str(), &playerGames, callbackCount);

	createQuery(queryRequest);

	return playerGames;
}

int SqliteDataBase::getNumOfPlayerScore(const std::string& userName) {
	// Creating SQL message that gets the num of total score of user

	std::stringstream ss;
	ss << "SELECT TOTAL_SCORE FROM USER_STATISTICS "
	   << "WHERE USER_ID = (SELECT USER_ID FROM USERS WHERE NAME = '" << userName << "');";

	std::string query = ss.str();
	const char* sqlStatement = query.c_str();

	int totalScore = 0;
	Query queryRequest = Query(query.c_str(), &totalScore, callbackCount);

	createQuery(queryRequest);

	return totalScore;
}

// High score list (top 3).
std::vector<std::string> SqliteDataBase::getHighScores()
{
	// Creating SQL message that gets the num of total score of user
	std::stringstream ss;
	ss << "SELECT USERS.NAME FROM USERS "
		<< "JOIN USER_STATISTICS ON USERS.USER_ID = USER_STATISTICS.USER_ID "
		<< "ORDER BY USER_STATISTICS.TOTAL_SCORE DESC "
		<< "LIMIT 3;";

	std::string query = ss.str();
	const char* sqlStatement = query.c_str();

	std::vector<std::string> bestUsers;
	Query queryRequest = Query(query.c_str(), &bestUsers, callbackClm);
	createQuery(queryRequest);
	return bestUsers;
}

// Generic query executor with error check.
void* SqliteDataBase::createQuery(Query query)
{
	char* errMessage = nullptr;

	if (sqlite3_exec(this->db, query.sqlStatement, query.callback, query.param, &errMessage) != SQLITE_OK)
	{
		std::string msg = std::string("Failed to execute Query: ") + std::string(errMessage);
		sqlite3_free(errMessage);
		throw DataBaseException(msg);
	}
	return query.param;
}

// SQLite callback helpers (count/avg/questions).
int SqliteDataBase::callbackCount(void* data, int argc, char** argv, char** colNames) {
	// Returning the number in data
	if (argc > 0 && argv[0]) {
		*static_cast<int*>(data) = std::stoi(argv[0]);
	}
	return 0;
}

int SqliteDataBase::callbackAverage(void* data, int argc, char** argv, char** colNames) {
	// Returning the number in data
	if (argc > 0 && argv[0]) {
		*static_cast<float*>(data) = std::stof(argv[0]);
	}
	return 0;
}

int SqliteDataBase::callbackQuestions(void* data, int argc, char** argv, char** colNames) {
	auto* results = static_cast<std::vector<Question>*>(data);
	std::vector<std::string> answers = { argv[2], argv[3], argv[4] ,argv[5] };;
	unsigned int answerId = std::stoi(argv[6]);
	std::string question = argv[1];
	results->push_back(Question(question, answers, answerId));
	return 0;
}

// Score calculation helper.
int SqliteDataBase::calcPoints(const std::string& username)
{
	int rightAnswers = getNumOfCorrectAnswers(username);
	int allAnswers = getNumOfTotalAnswers(username);
	//about the time
	return (rightAnswers + allAnswers) * (int)getPlayerAvrageAnswerTime(username);
}

// Libcurl write callback (accumulate response).
size_t SqliteDataBase::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

// Download & store questions if DB is empty.
bool SqliteDataBase::getQuestionsDatabase()
{
	CURL* curl = curl_easy_init();
	if (curl)
	{
		std::string response;
		//getting the questions
		curl_easy_setopt(curl, CURLOPT_URL, "https://opentdb.com/api.php?amount=20&type=multiple");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			throw DataBaseException("Could Not Get Questions For Database.. using Default Questions");
		}
		curl_easy_cleanup(curl);

		// Parse JSON response and insert each question into QUESTIONS table.
		try
		{
			auto parsedMessage = json::parse(response);

			for (const auto& jMsg : parsedMessage["results"])
			{
				// Combine correct + incorrect answers then shuffle.
				auto questions = jMsg["incorrect_answers"].get<std::vector<std::string>>();
				questions.push_back(jMsg["correct_answer"].get<std::string>());
				std::random_shuffle(questions.begin(), questions.end()); //shuffling the questions
				
				auto correctIt = std::find(questions.begin(), questions.end(), jMsg["correct_answer"].get<std::string>()); //getting the correct answer
				Question questionItem = Question(jMsg["question"].get<std::string>(), questions, std::distance(questions.begin(), correctIt)); //Creating a question
				std::stringstream ss;

				ss << "INSERT INTO QUESTIONS (QUESTION, ANSWER_FIRST, ANSWER_SECOND, ANSWER_THIRD, ANSWER_FOURTH, CORRECT) VALUES ('" <<
					questionItem.getQuestion() << "', '";
				for (auto& question : questions)
				{
					ss << question + "', '";
				}
				std::string insertString = ss.str();
				insertString.pop_back();
				insertString = insertString + std::to_string(questionItem.getCorrectAnswerId()) + ");";
				Query query = Query(insertString.c_str(), nullptr, callbackCount);

				createQuery(query);
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what();
			close();
		}
		return true;
	}
	return false;
}
 
// Simple column list callback (push argv[] into vector).
int SqliteDataBase::callbackClm(void* data, int argc, char** argv, char** colNames)
{
	auto* results = static_cast<std::vector<std::string>*>(data);
	std::stringstream row;
	for (int i = 0; i < argc; ++i)
	{
		results->push_back(argv[i]);
	}
	return 0;
}

// Update statistics after a game.
void SqliteDataBase::statsToDatabase(const std::string& username, const int totalAnswers, const int correctAnswers, const float averageAnswerTime)
{
	// Resolve USER_ID
	int userId = -1;
	std::stringstream ssUserId;
	ssUserId << "SELECT USER_ID FROM USERS WHERE NAME = '" << username << "';";
	std::string userIdStr = ssUserId.str();
	Query userIdQuery(userIdStr.c_str(), &userId, callbackCount);
	createQuery(userIdQuery);

	if (userId == -1)
	{
		throw DataBaseException("User not found when updating stats");
	}

	// Update GAME_STATISTICS.
	std::stringstream ssCheck;
	ssCheck << "SELECT COUNT(*) FROM GAME_STATISTICS WHERE USER_ID = " << userId << ";";
	int statExists = 0;
	std::string sCheckStr = ssCheck.str();
	Query checkQuery(sCheckStr.c_str(), &statExists, callbackCount);
	createQuery(checkQuery);

	std::stringstream ss;
	if (statExists > 0)
	{
		// Weighted average update formula
		ss << "UPDATE GAME_STATISTICS SET "
			<< "TOTAL_ANSWERS = TOTAL_ANSWERS + " << totalAnswers << ", "
			<< "CORRECT_ANSWERS = CORRECT_ANSWERS + " << correctAnswers << ", "
			<< "AVERAGE_ANSWER_TIME = ((AVERAGE_ANSWER_TIME * TOTAL_ANSWERS) + (" << averageAnswerTime << " * " << totalAnswers << ")) / (TOTAL_ANSWERS + " << totalAnswers << ") "
			<< "WHERE USER_ID = " << userId << ";";
	}
	else
	{
		ss << "INSERT INTO GAME_STATISTICS (USER_ID, AVERAGE_ANSWER_TIME, CORRECT_ANSWERS, TOTAL_ANSWERS) "
			<< "VALUES (" << userId << ", " << averageAnswerTime << ", " << correctAnswers << ", " << totalAnswers << ");";
	}
	std::string queryStr = ss.str();
	Query finalQuery(queryStr.c_str(), nullptr, callbackCount);
	createQuery(finalQuery);

	// Update USER_STATISTICS.
	int score = calcPoints(username);

	std::stringstream ssCheckUserStats;
	ssCheckUserStats << "SELECT COUNT(*) FROM USER_STATISTICS WHERE USER_ID = " << userId << ";";
	
	int userStatExists = 0;
	std::string userStatsStr = ssCheckUserStats.str();
	createQuery(Query(userStatsStr.c_str(), &userStatExists, callbackCount));
	std::stringstream ssUserStats;
	
	if (userStatExists > 0)
	{
		ssUserStats << "UPDATE USER_STATISTICS SET "
			<< "PLAYER_GAMES = PLAYER_GAMES + 1, "
			<< "TOTAL_SCORE = TOTAL_SCORE + " << score
			<< " WHERE USER_ID = " << userId << ";";
	}
	else
	{
		ssUserStats << "INSERT INTO USER_STATISTICS (USER_ID, PLAYER_GAMES, TOTAL_SCORE) "
			<< "VALUES (" << userId << ", 1, " << score << ");";
	}
	std::string sStatsStr = ssUserStats.str();
	createQuery(Query(sStatsStr.c_str(), nullptr, callbackCount));

}