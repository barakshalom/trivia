#include "JsonResponsePacketSerializer.h"

// Trivial status responses
Buffer JsonResponsePacketSerializer::serializeResponse(const LoginResponse& loginResponse)
{
	Buffer buffer;
	return serialize("status", loginResponse.status, buffer, RequestId::LOGIN);
}

Buffer JsonResponsePacketSerializer::serializeResponse(const SignupResponse& signupResponse)
{
	Buffer buffer;
	return serialize("status", signupResponse.status, buffer, RequestId::SIGNUP);
}

Buffer JsonResponsePacketSerializer::serializeResponse(const ErrorResponse& errorResponse)
{
	Buffer buffer;
	return serialize("message", errorResponse.message, buffer, RequestId::ERROR_MSG);
}

Buffer JsonResponsePacketSerializer::serializeResponse(const LogoutResponse& logoutResponse)
{
	Buffer buffer;
	return serialize("status", logoutResponse.status, buffer, RequestId::LOGOUT);
}

Buffer JsonResponsePacketSerializer::serializeResponse(const JoinRoomResponse& joinResponse)
{
	Buffer buffer;
	return serialize("status", joinResponse.status, buffer, RequestId::JOIN_ROOM);
}

Buffer JsonResponsePacketSerializer::serializeResponse(const CreateRoomResponse& createResponse)
{
	Buffer buffer;
	return serialize("status", createResponse.status, buffer, RequestId::CREATE_ROOM);
}

// Serializes a list of active rooms
Buffer JsonResponsePacketSerializer::serializeResponse(const getRoomResponse& roomResponse)
{
	Buffer buffer;
	std::vector<json> roomList;

	// Convert each room to JSON and add it to the list
	for (const auto& room : roomResponse.rooms)
	{
		roomList.push_back({
			{ "name", room.name },
			{ "id", room.id },
			{ "maxPlayers", room.maxPlayers }
			});
	}
	
	// Serialize room list to buffer
	serialize("Rooms", roomList, buffer, RequestId::GET_ROOMS);
	return buffer;
}

// Serializes list of usernames in a room
Buffer JsonResponsePacketSerializer::serializeResponse(const getPlayersInRoomReponse& playerResponse)
{
	Buffer buffer;
	return serialize("PlayersInRoom", playerResponse.users, buffer, RequestId::GET_PLAYERS_IN_ROOM);
}

// Serializes high score list
Buffer JsonResponsePacketSerializer::serializeResponse(const HighScoreResponse& highscoreResponse)
{
	Buffer buffer;
	return serialize("Scores", highscoreResponse.highScore, buffer, RequestId::GET_HIGH);
}

// Serializes player statistics
Buffer JsonResponsePacketSerializer::serializeResponse(const StatsResponse& statsResponse)
{
	Buffer buffer;
	json stats = {
		{ "games", statsResponse.stats[0] },
		{ "correct", statsResponse.stats[1] },
		{ "total", statsResponse.stats[2] },
		{ "time", statsResponse.stats[3] }
	};
	return serialize("Stats", stats, buffer, RequestId::STATS);
}

// Serializes LeaveRoomResponse
Buffer JsonResponsePacketSerializer::serializeResponse(const LeaveRoomResponse& leaveResponse)
{
	Buffer buffer;
	return serialize("status", leaveResponse.status, buffer, RequestId::LEAVE_ROOM);
}

// Serializes StartGameResponse
Buffer JsonResponsePacketSerializer::serializeResponse(const StartGameResponse& startResponse)
{
	Buffer buffer;
	return serialize("status", startResponse.status, buffer, RequestId::START_ROOM);

}

// Serializes CloseRoomResponse
Buffer JsonResponsePacketSerializer::serializeResponse(const CloseRoomResponse& closeResponse)
{
	Buffer buffer;
	return serialize("status", closeResponse.status, buffer, RequestId::CLOSE_ROOM);
}

// Serializes current state of a room
Buffer JsonResponsePacketSerializer::serializeResponse(const GetRoomStateResponse& stateResponse)
{
	Buffer buffer;
	json stats = {
		{ "status", stateResponse.status },
		{ "answerCount", stateResponse.answerCount },
		{ "answerTimeout", stateResponse.answerTimeout },
		{ "hasGameBegun", stateResponse.hasGameBegun },
		{ "players", stateResponse.players}
	};
	return serialize("RoomState", stats, buffer, RequestId::STATE);
}

// Serializes game results for all players
Buffer JsonResponsePacketSerializer::serializeResponse(const GetGameResultsResponse& gameResultsResponse)
{
	Buffer buffer;
	std::vector<json> playerList;

	// Add each player's result to the list
	for (auto player : gameResultsResponse.results)
	{
		playerList.push_back({ player.username, {"averageAnswerCount", player.averageAnswerCount}, {"correctAnswerCount", player.correctAnswerCount}, {"wrongAnswerCount", player.wrongAnswerCount} });
	}
	json results = {
		{ "status", gameResultsResponse.status },
		playerList
	};
	return serialize("Results", results, buffer, RequestId::GAME_RESULTS);
}

// Serializes answer submission result
Buffer JsonResponsePacketSerializer::serializeResponse(const SubmitAnswerResponse& answerResponse)
{
	Buffer buffer;
	json questions = {
		{ "status", answerResponse.status },
		{ "answerId", answerResponse.correctAnswerId }
	};
	return serialize("Submit", questions, buffer, RequestId::SUBMIT_ANSWER);
}

// Serializes question and its answers
Buffer JsonResponsePacketSerializer::serializeResponse(const GetQuestionResponse& questionResponse)
{
	Buffer buffer;
	std::vector<json> answers;

	// Convert each answer to JSON and add to list
	for (auto answer : questionResponse.answers)
	{
		answers.push_back({ answer.first, answer.second });
	}

	json questions = {
		{ "status", questionResponse.status },
		{ "question", questionResponse.question },
		answers
	};
	return serialize("Questions", questions, buffer, RequestId::GET_QUESTION);
}

// Serializes LeaveGameResponse
Buffer JsonResponsePacketSerializer::serializeResponse(const LeaveGameResponse& leaveGameResponse)
{
	Buffer buffer;
	return serialize("status", leaveGameResponse.status, buffer, RequestId::LEAVE_GAME);
}

// Appends the size of the message (4 bytes) to the buffer in big endian order
void JsonResponsePacketSerializer::putMessageSize(const std::string& message, Buffer& buffer)
{
	// Convert the size of the message (in characters) into an unsigned int
	unsigned int size = static_cast<unsigned int>(message.size()); //using a static cast for safe size that we get for the bits
	
	// Create a pointer to the size variable, treating it as a sequence of bytes (unsigned char)
    // This allows us to access each byte of the 4-byte integer individually
	unsigned char* sizePtr = reinterpret_cast<unsigned char*>(&size); //getting the size for extraction to bits using reinterpretion
	
	// Push each byte to the buffer in big endian order (most significant byte first)
	buffer.push_back(sizePtr[3]); 
	buffer.push_back(sizePtr[2]);
	buffer.push_back(sizePtr[1]);
	buffer.push_back(sizePtr[0]);

	// Note: On little-endian systems (like most PCs), this reverses the default order
	// This ensures that the message size is stored in network byte order (big-endian),
	// which is a standard for communication protocols.
}
