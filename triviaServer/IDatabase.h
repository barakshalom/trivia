#pragma once
#include <string>
#include <iostream>
#include <functional>
#include <list>
#include "Question.h"

typedef struct Query
{
	const char* sqlStatement;
	void* param;
	int (*callback)(void*, int, char**, char**);
	Query(const char* sqlStatement, void* param, int (*callback)(void*, int, char**, char**))
		: sqlStatement(sqlStatement), param(param), callback(callback) {}

} Query;

class IDatabase 
{
public:
	virtual bool doesUserExist(const std::string& userName) = 0;
	virtual bool doesPasswordMatch(const std::string& userName, const std::string& password) = 0;
	virtual void addNewUser(const std::string& userName, const std::string& password, const std::string& email, const std::string& address, const std::string& phoneNumber, const std::string& birthDate) = 0;

	virtual std::vector<Question> getQuestions(const int numOfQ) = 0;
	virtual float getPlayerAvrageAnswerTime(const std::string& userName) = 0;
	virtual int getNumOfCorrectAnswers(const std::string& userName) = 0;
	virtual int getNumOfTotalAnswers(const std::string& userName) = 0;
	virtual int getNumOfPlayerGames(const std::string& userName) = 0;
	virtual int getNumOfPlayerScore(const std::string& userName) = 0;
	virtual std::vector<std::string> getHighScores() = 0;
	virtual void statsToDatabase(const std::string& username, const int totalAnswers, const int correctAnswers, const float averageAnswerTime) = 0;
	virtual void* createQuery(Query query) = 0;

	virtual void close() = 0;
};
