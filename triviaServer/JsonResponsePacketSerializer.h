#pragma once
#include "Structs.h"
#include "IRequestHandler.h"
#include "json.hpp"
#include <sstream>

// Response Structs
// These structs define the response format for various server operations.
// Each struct corresponds to a response sent to the client after a request.

typedef struct ErrorResponse
{
	std::string message;
} ErrorResponse;

typedef struct SignupResponse
{
	unsigned int status;
} SignupResponse;

typedef struct LoginResponse
{
	unsigned int status;
} LoginResponse;

typedef struct LogoutResponse
{
	unsigned int status;
} LogoutResponse;

typedef struct CreateRoomResponse
{
	unsigned int status;
} CreateRoomResponse;

typedef struct JoinRoomResponse
{
	unsigned int status;
} JoinRoomResponse;

typedef struct getRoomResponse
{
	std::vector<RoomData> rooms;
} getRoomResponse;

typedef struct getPlayersInRoomResponse
{
	std::vector<std::string> users;
} getPlayersInRoomReponse;

typedef struct StatsResponse
{
	std::vector<std::string> stats;
} StatsResponse;

typedef struct CloseRoomResponse
{
	unsigned int status;
} CloseRoomResponse;

typedef struct StartGameResponse
{
	unsigned int status;
} StartGameResponse;

typedef struct LeaveGameResponse
{
	unsigned int status;
} LeaveGameResponse;

typedef struct GetQuestionResponse
{
	unsigned int status;
	std::string question;
	std::map<int, std::string> answers;
} GetQuestionResponse;

typedef struct SubmitAnswerResponse
{
	unsigned int status;
	unsigned int correctAnswerId;
} SubmitAnswerResponse;

typedef struct PlayerResults
{
	std::string username;
	unsigned int wrongAnswerCount;
	unsigned int correctAnswerCount;
	float averageAnswerCount;
} PlayerResults;

typedef struct GetGameResultsResponse
{
	unsigned int status;
	std::vector<PlayerResults> results;
} GetGameResultsResponse;

typedef struct GetRoomStateResponse
{
	unsigned int status;
	bool hasGameBegun;
	std::vector<std::string> players;
	unsigned int answerCount;
	unsigned int answerTimeout;
} GetRoomStateResponse;

typedef struct LeaveRoomResponse
{
	unsigned int status;
} LeaveRoomResponse;

typedef struct HighScoreResponse
{
	std::vector<std::string> highScore;
} HighScoreResponse;

// Define alias for JSON library
using json = nlohmann::json;

// This class converts C++ response structs into binary Buffers
// (std::vector<unsigned char>) that can be sent over the network.
// Each serializeResponse method handles a specific response type.
class JsonResponsePacketSerializer
{
public:
	// Serialize individual response types into a Buffer.
	static Buffer serializeResponse(const LoginResponse& loginResponse);
	static Buffer serializeResponse(const SignupResponse& signupResponse);
	static Buffer serializeResponse(const ErrorResponse& errorResponse);
	static Buffer serializeResponse(const LogoutResponse& logoutResponse);
	static Buffer serializeResponse(const JoinRoomResponse& joinResponse);
	static Buffer serializeResponse(const CreateRoomResponse& createResponse);
	static Buffer serializeResponse(const getRoomResponse& roomResponse);
	static Buffer serializeResponse(const getPlayersInRoomReponse& playerResponse);
	static Buffer serializeResponse(const HighScoreResponse& highscoreResponse);
	static Buffer serializeResponse(const StatsResponse& statsResponse);
	static Buffer serializeResponse(const LeaveRoomResponse& leaveResponse);
	static Buffer serializeResponse(const StartGameResponse& startResponse);
	static Buffer serializeResponse(const CloseRoomResponse& closeResponse);
	static Buffer serializeResponse(const GetRoomStateResponse& stateResponse);
	static Buffer serializeResponse(const GetGameResultsResponse& gameResultsResponse);
	static Buffer serializeResponse(const SubmitAnswerResponse& answerResponse);
	static Buffer serializeResponse(const GetQuestionResponse& questionResponse);
	static Buffer serializeResponse(const LeaveGameResponse& leaveGameResponse);

private:
	// Helper method for serialzing everything! 

	/*
	* Serializes a response into a binary buffer to send over the network.	* the receiver knows how many bytes to expect. The exact encoding format depends
	* on the protocol implementation.
	*
	* Input: <T1 - key type, T2 - Value type>
	*        key - key name under which the value is stored in the JSON object.
	*        value - data to be serialized.
	*        buffer - output buffer where the serialized message will be stored.
	*        id -  request id to identify the message type.
	* Output: buffer - full serialized packet ready to be sent.
	*/
	template<typename T1, typename T2>
	static Buffer serialize(const T1& key, const T2& value, Buffer& buffer, const RequestId& id)
	{
		std::string message;
		json json;

		// Insert the key value pair into a JSON object.
		json[key] = value;

		// Convert the JSON object to a string representation.
		message = json.dump();

		// Add the request ID as the first byte in the buffer.
		buffer.push_back(static_cast<unsigned char>(id));

		// Add the size of the message as metadata (multiple bytes depending on protocol).
		putMessageSize(message, buffer);

		// Append the actual message characters as bytes.
		for (auto& bit : message)
		{
			buffer.push_back(static_cast<unsigned char>(bit));
		}
		return buffer;
	}

	/*
	* This function encodes the size of the message into the buffer so that
	* the receiver knows how many bytes to expect. The exact encoding format depends
	* on the protocol implementation.
	* 
	* Input: message - serialized JSON string.
	* Output:  buffer - buffer to which the message size will be added.
	*/
	static void putMessageSize(const std::string& message, Buffer& buffer);

};
