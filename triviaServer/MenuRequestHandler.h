#pragma once
#include "IRequestHandler.h"
#include "RequestHandlerFactory.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"
#include "LoginManager.h"


class MenuRequestHandler : public IRequestHandler
{
public:
	// Constrictor and destructor.
	MenuRequestHandler(RequestHandlerFactory* factory, std::shared_ptr<LoggedUser> user);
	virtual ~MenuRequestHandler() = default;
	
	/*
	* Checks whether the request is relevant to the menu context.
	* Input: request - the request information received from the client.
	* Output: true if the request can be handled by the menu, false otherwise.
	*/
	virtual bool isRequestRelevant(const RequestInfo& request) const override;
	
	/*
	* Handles a client request while the user is in the menu state.
	* Routes the request to the appropriate handler function based on its request ID.
	* Input: request - the request information received from the client.
	* Output: a RequestResult containing the response and (optionally) a new handler.
	*/
	virtual RequestResult handleRequest(const RequestInfo& request) const override;



private:
	RequestHandlerFactory* m_handlerFactory;
	std::shared_ptr<LoggedUser> m_user;

	/*
	* Handles specific menu-related client requests based on the received buffer.
	*
	* Each function processes a different request type and returns a RequestResult
	* with the response and (optionally) a new handler for state transition.
	* 
	* Input: buffer - the request information received from the client.
	* Output: a RequestResult containing the response and (optionally) a new handler.
	*/
	RequestResult createRoom(const Buffer& buffer);
	RequestResult getRooms(const Buffer& buffer);
	RequestResult logout(const Buffer& buffer);
	RequestResult getPlayersInRooms(const Buffer& buffer);
	RequestResult joinRoom(const Buffer& buffer);
	RequestResult getScore(const Buffer& buffer);
	RequestResult getStats(const Buffer& buffer);

};