#pragma once
#include "IDatabase.h"

#include <curl/curl.h>
#include <vector>
#include "sqlite3.h"
#include "JsonResponsePacketSerializer.h"


class Question;

// Concrete implementation of IDatabase using SQLite for storing and retrieving game data.
class SqliteDataBase : public IDatabase 
{
public:
	SqliteDataBase();

	// User management.
	virtual bool doesUserExist(const std::string& userName) override;
	virtual bool doesPasswordMatch(const std::string& userName, const std::string& password) override;
	virtual void addNewUser(const std::string& userName, const std::string& password, const std::string& email, const std::string& address, const std::string& phoneNumber, const std::string& birthDate) override;

	// Question handling.
	virtual std::vector<Question> getQuestions(const int numOfQ) override;
	
	// Stats
	virtual float getPlayerAvrageAnswerTime(const std::string& userName) override;
	virtual int getNumOfCorrectAnswers(const std::string& userName) override;
	virtual int getNumOfTotalAnswers(const std::string& userName) override;
	virtual int getNumOfPlayerGames(const std::string& userName) override;
	virtual int getNumOfPlayerScore(const std::string& userName) override;
	virtual std::vector<std::string> getHighScores() override;

	// Internal utilities.
	virtual void* createQuery(Query query) override;
	virtual void close() override;
	virtual void statsToDatabase(const std::string& username, const int totalAnswers, const int correctAnswers, const float averageAnswerTime) override;
private:
	sqlite3* db; // Pointer to SQLite database connection.

	// Helpers
	void clear(); // Clears internal temp data if needed.

	// Callback functions for SQLite query result processing.
	static int callbackCount(void* data, int argc, char** argv, char** colNames);
	static int callbackAverage(void* data, int argc, char** argv, char** colNames);
	static int callbackClm(void* data, int argc, char** argv, char** colNames);
	static int callbackQuestions(void* data, int argc, char** argv, char** colNames);
	
	// Calculates score points based on player's stats.
	int calcPoints(const std::string& username);

	// Loads questions into DB from external source (if implemented).
	bool getQuestionsDatabase();

	// Callback for handling HTTP response body (used with libcurl).
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
};
