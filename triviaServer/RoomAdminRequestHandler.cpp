#include "RoomAdminRequestHandler.h"
#include "Communicator.h"
#include "GameRequestHandler.h"

RoomAdminRequestHandler::RoomAdminRequestHandler(RequestHandlerFactory* factory, std::shared_ptr<LoggedUser> user, std::shared_ptr<Room> room)
{
	m_handlerFactory = factory;
	m_room = room;
	m_user = user;
}

bool RoomAdminRequestHandler::isRequestRelevant(const RequestInfo& request) const
{
	return request.id == RequestId::CLOSE_ROOM || request.id == RequestId::START_ROOM 
		|| request.id == RequestId::STATE;
}

RequestResult RoomAdminRequestHandler::handleRequest(const RequestInfo& request) const
{
	//return request.;
	RequestResult result;
	if (isRequestRelevant(request))
	{
		if (request.id == RequestId::CLOSE_ROOM)
		{
			closeRoom(result);
		}
		else if (request.id == RequestId::START_ROOM)
		{
			startRoom(result);
		}
		else if (request.id == RequestId::STATE)
		{
			getRoomState(result);
		}
	}
	return result;
}

void RoomAdminRequestHandler::getRoomState(RequestResult& result) const
{
	GetRoomStateResponse stateResponse;
	std::vector<std::string> players;
	auto data = m_room->getData();

	// Collect usernames of users in the room.
	for (auto user : m_room->getAllUsers())
	{
		players.push_back(user->getUsername());
	}

	// Fill room state information.
	stateResponse.answerCount = data.numOfQuestionsInGame;
	stateResponse.answerTimeout = data.timePerQuestion;
	stateResponse.players = players;
	stateResponse.hasGameBegun = data.isActive;
	stateResponse.status = 1;

	// Serialize the response and return the same handler.
	result.response = JsonResponsePacketSerializer::serializeResponse(stateResponse);
	result.newHandler = std::move(m_handlerFactory->createAdminRequestHandler(m_user, m_room));

}

void RoomAdminRequestHandler::startRoom(RequestResult& result) const
{
	StartGameResponse startResponse;

	// Create a new game for the current room.
	auto game = m_handlerFactory->getGameManager()->createGame(m_room.get());
	
	// Delete the room (it is no longer needed after game starts).
	m_handlerFactory->getRoomManager()->deleteRoom(m_room->getData().id);
	
	// Notify all players (except the admin) and assign them a GameRequestHandler.
	for (auto user : m_room->getAllUsers())
	{
		if (m_user->getUsername() != user->getUsername())
		{
			startResponse.status = 1;
			Communicator::sendMessage(user->getSocket(), JsonResponsePacketSerializer::serializeResponse(startResponse));
			Communicator::setClientHandler(*user->getSocket().get(), std::move(m_handlerFactory->createGameRequestHandler(user, game)));

		}
	}

	// Set response and assign a GameRequestHandler to the admin too.
	result.response = JsonResponsePacketSerializer::serializeResponse(startResponse);
	result.newHandler = std::move(m_handlerFactory->createGameRequestHandler(m_user, game));
}

void RoomAdminRequestHandler::closeRoom(RequestResult& result) const
{
	LeaveRoomResponse leaveResponse;
	CloseRoomResponse closeResponse;

	// Notify and return each user (except admin) to the menu.
	for (auto user : m_room->getAllUsers())
	{
		leaveResponse.status = 1;
		if (m_user->getUsername() != user->getUsername())
		{
			Communicator::sendMessage(user->getSocket(), JsonResponsePacketSerializer::serializeResponse(leaveResponse));
			Communicator::setClientHandler(*user->getSocket().get(), std::move(m_handlerFactory->createMenuRequestHandler(user)));
		}
	}

	// Delete the room from the manager.
	m_handlerFactory->getRoomManager()->deleteRoom(m_room->getData().id);
	
	// Return the admin to the menu.
	closeResponse.status = 1;
	result.response = JsonResponsePacketSerializer::serializeResponse(closeResponse);
	result.newHandler = std::move(m_handlerFactory->createMenuRequestHandler(m_user));
}
