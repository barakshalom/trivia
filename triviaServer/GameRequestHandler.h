#pragma once
#include "IRequestHandler.h"
#include "RequestHandlerFactory.h"
#include "GameManager.h"
#include "JsonRequestPacketDeserializer.h"

// This class handles game related requests from the client during an active game session.
class GameRequestHandler : public IRequestHandler
{
public:
	// Constructor and destructor.
	GameRequestHandler(RequestHandlerFactory* factory, std::shared_ptr<LoggedUser> user, std::shared_ptr<Game> game);
	virtual ~GameRequestHandler() = default;

	// Checks if the received request is relevant for the game state.
	virtual bool isRequestRelevant(const RequestInfo& request) const override;
	
	// Handles the request and returns an appropriate response.
	virtual RequestResult handleRequest(const RequestInfo& request) const override;

private:
	// Handles the get question request.
	void getQuestion(RequestResult& result, const Buffer& buffer) const;
	
	// Handles the submit answer request
	void submitAnswer(RequestResult& result, const Buffer& buffer) const;
	
	// Handles the request to get game results.
	void getGameResults(RequestResult& result, const Buffer& buffer) const;
	
	// Handles the request to leave the game
	void leaveGame(RequestResult& result, const Buffer& buffer) const;

	// Pointer to the request handler factory (used to generate new handlers).
	RequestHandlerFactory* m_handlerFactory;
	
	// Pointer to the current user in the game.
	std::shared_ptr<LoggedUser> m_user;
	
	// Pointer to the current game session.
	std::shared_ptr<Game> m_game;
};
