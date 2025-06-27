#pragma once
#include "RequestException.hpp"
#include <time.h>
#include <vector>
#include <memory>

class IRequestHandler;

typedef std::vector<unsigned char> Buffer;

enum class RequestId
{
	NONE = 0x0, ERROR_MSG, LOGIN, SIGNUP, CREATE_ROOM, JOIN_ROOM, GET_PLAYERS_IN_ROOM, STATS, LOGOUT, GET_ROOMS, CLOSE_ROOM, START_ROOM,
	STATE, GET_HIGH, LEAVE_ROOM, LEAVE_GAME, GET_QUESTION, SUBMIT_ANSWER, GAME_RESULTS, EXIT
};

// Represents a single request received from a client.
// This struct contains metadata about the request - type, time received, and the raw message data.
typedef struct RequestInfo
{
	RequestId id; // The type of the request
	time_t receivalTime;  // The timestamp of when the request was received.
	Buffer buffer; // The raw data
} RequestInfo;

typedef struct RequestResult
{
	Buffer response;
	std::unique_ptr<IRequestHandler> newHandler;
} RequestResult;



class IRequestHandler
{
public:
	virtual bool isRequestRelevant(const RequestInfo& request) const = 0;
	virtual RequestResult handleRequest(const RequestInfo& request) const = 0;

private:

};