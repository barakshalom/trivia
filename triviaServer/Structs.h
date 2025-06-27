#pragma once
#include <string>
#include <vector>
#include "LoggedUser.hpp"

typedef struct RoomData
{
	unsigned int id;
	std::string name;
	unsigned int maxPlayers;
	unsigned int numOfQuestionsInGame;
	unsigned int timePerQuestion;
	bool isActive; //changed it to bool
	RoomData() = default;
} RoomData;