#pragma once
#include "IRequestHandler.h"
#include "RequestHandlerFactory.h"

// Handles requests from the room administrator.
class RoomAdminRequestHandler : public IRequestHandler
{
public:
	// Constructor and destructor.
	RoomAdminRequestHandler(RequestHandlerFactory* factory, std::shared_ptr<LoggedUser> user, std::shared_ptr<Room> room);
	virtual ~RoomAdminRequestHandler() = default;

	// Checks if the given request is relevant for the room admin.
	virtual bool isRequestRelevant(const RequestInfo& request) const override;
	
	// Handles the request by calling the appropriate function based on request id.
	virtual RequestResult handleRequest(const RequestInfo& request) const override;
private:
	// Gathers and serializes the current state of the room.
	void getRoomState(RequestResult& result) const;

	// Starts the game for this room and transitions to GameRequestHandler.
	void startRoom(RequestResult& result) const;
	
	// Closes the room and transitions back to MenuRequestHandler.
	void closeRoom(RequestResult& result) const;

	// Pointer to the factory for creating new request handlers.
	RequestHandlerFactory* m_handlerFactory;
	
	// Pointer to the admin user.
	std::shared_ptr<LoggedUser> m_user;
	
	// Pointer to the room being managed.
	std::shared_ptr<Room> m_room;

};