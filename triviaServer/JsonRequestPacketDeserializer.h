#pragma once
#include "IRequestHandler.h"
#include "json.hpp"
#include "LoggedUser.hpp"

// Request Structs

// These structs represent the different types of client requests.
// Each one corresponds to a specific action in the system (login, signup, join room..).
// They are used as typed outputs when deserializing raw request buffers.

typedef struct LoginRequest
{
	std::string username;
	std::string password;
} LoginRequest;

typedef struct SignupRequest
{
	std::string username;
	std::string password;
	std::string email;
	std::string address;
	std::string phoneNumber;
	std::string birthDate;
} SignupRequest;


typedef struct GetPlayersInRoomRequest
{
	unsigned int roomId;
} GetPlayersInRoomRequest;

typedef struct JoinRoomRequest
{
	unsigned int roomId;
} JoinRoomRequest;

typedef struct StatsRequest
{
	std::string username;
} StatsRequest;

typedef struct CreateRoomRequest
{
	std::string roomName;
	unsigned int maxUsers;
	unsigned int questionCount;
	unsigned int answerTimeout;
} CreateRoomRequest;

typedef struct SubmitAnswerRequest
{
	unsigned int answerId;
} SubmitAnswerRequest;


// JsonRequestPacketDeserializer
using json = nlohmann::json;

// This utility class is responsible for converting raw buffers (Buffer)
// into specific request structs. Each static method corresponds to one request type.
class JsonRequestPacketDeserializer
{
public:

	static LoginRequest deserializeLoginRequest(const Buffer& buffer);
	static SignupRequest deserializeSignupRequest(const Buffer& buffer);
	static GetPlayersInRoomRequest deserializeGetPlayersInRoomRequest(const Buffer& buffer);
	static JoinRoomRequest deserializeJoinRoomRequest(const Buffer& buffer);
	static CreateRoomRequest deserializeCreateRoomRequest(const Buffer& buffer);
	static StatsRequest deserializeStatsRequest(const Buffer& buffer);
	static SubmitAnswerRequest deserializerSubmitAnswerRequest(const Buffer& buffer);
private:

};
