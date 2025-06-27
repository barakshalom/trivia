#include "RoomMemberRequestHandler.h"
#include "Communicator.h"

RoomMemberRequestHandler::RoomMemberRequestHandler(RequestHandlerFactory* factory, std::shared_ptr<LoggedUser> user, std::shared_ptr<Room> room)
{
	m_handlerFactory = factory;
	m_room = room;
	m_user = user;
}

bool RoomMemberRequestHandler::isRequestRelevant(const RequestInfo& request) const
{
	return request.id == RequestId::LEAVE_ROOM || request.id == RequestId::STATE;
}

RequestResult RoomMemberRequestHandler::handleRequest(const RequestInfo& request) const
{
	RequestResult result;
	if (isRequestRelevant(request))
	{
		if (request.id == RequestId::LEAVE_ROOM)
		{
			leaveRoom(result);
		}
		else if (request.id == RequestId::STATE)
		{
			getRoomState(result);
		}
	}
	return result;
}

void RoomMemberRequestHandler::getRoomState(RequestResult& result) const
{
	GetRoomStateResponse stateResponse;
	std::vector<std::string> players;

	auto data = m_room->getData();

	// Collect usernames of all users currently in the room.
	for (auto user : m_room->getAllUsers())
	{
		players.push_back(user->getUsername());
	}
	stateResponse.answerCount = data.numOfQuestionsInGame;
	stateResponse.answerTimeout = data.timePerQuestion;
	stateResponse.players = players;
	stateResponse.hasGameBegun = data.isActive;
	stateResponse.status = 1;

	// Serialize response and keep the current handler.
	result.response = JsonResponsePacketSerializer::serializeResponse(stateResponse);
	result.newHandler = std::move(m_handlerFactory->createRoomMemberHandler(m_user, m_room));
}

void RoomMemberRequestHandler::leaveRoom(RequestResult& result) const
{
	LeaveRoomResponse leaveResponse;

	leaveResponse.status = 1;

	// Serialize the leave response.
	result.response = JsonResponsePacketSerializer::serializeResponse(leaveResponse);
	
	// Remove the user from the room.
	m_handlerFactory->getRoomManager()->leaveRoom(m_user, m_room);
	
	// Assign a new MenuRequestHandler to return to the main menu.
	result.newHandler = std::move(m_handlerFactory->createMenuRequestHandler(m_user));
}
