#pragma once
#include "IRequestHandler.h"
#include "RequestHandlerFactory.h"

// RoomMemberRequestHandler handles requests from a user who is a member (but not the admin).
// of a trivia room. This includes checking the room state and leaving the room.

class RoomMemberRequestHandler : public IRequestHandler
{
public:
	// Constructor and destructor.
	RoomMemberRequestHandler(RequestHandlerFactory* factory, std::shared_ptr<LoggedUser> user, std::shared_ptr<Room> room);
	virtual ~RoomMemberRequestHandler() = default;
	
	/*
	* Checks if the incoming request ID is relevant to room member actions.
	* Input: request - The incoming request data from the client.
	* Output: true if the request is one of: GET_ROOM_STATE, LEAVE_ROOM.
	*/
	virtual bool isRequestRelevant(const RequestInfo& request) const override;

	/*
	* Handles the relevant request and returns a RequestResult with the response and next handler.
	* Input: request - The incoming request data.
	* Output: RequestResult containing response buffer and optional new handler.
	*/
	virtual RequestResult handleRequest(const RequestInfo& request) const override;
 
private:
	/*
	* Fills the RequestResult with current room metadata and player list.
	* Input: result - The RequestResult to populate with room state data.
	* Output: none.
	*/
	void getRoomState(RequestResult& result) const;

	/*
	* Handles logic for user leaving the room, updates room and redirects user to menu.
	* Input: result - The RequestResult to populate with response and new handler.
	* Output: none.
	*/
	void leaveRoom(RequestResult& result) const;

	RequestHandlerFactory* m_handlerFactory; // Used to create new handlers.
	std::shared_ptr<LoggedUser> m_user; // The user associated with this handler.
	std::shared_ptr<Room> m_room; // The room the user is currently in.

};