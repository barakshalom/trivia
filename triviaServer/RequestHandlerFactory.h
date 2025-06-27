#pragma once
#include <memory>
#include "LoginManager.h"
#include "RoomManager.h"
#include "StatisticsManager.h"
#include "GameManager.h"

class MenuRequestHandler;
class LoginRequestHandler;
class RoomAdminRequestHandler;
class RoomMemberRequestHandler;
class GameRequestHandler;

/*
* Responsible for creating and managing request handlers for different client states. (eg - menu, login, room, game..)
* 
* This factory class provides a centralized way to create instances of various IRequestHandler
* implementations, depending on the client's current state.
* It also manages shared logic components like LoginManager, RoomManager, GameManager..
*
* This design supports clean separation of concerns and enables state transitions in the server
* by assigning new handlers to clients as they move through the application flow.
*/
class RequestHandlerFactory
{
public:
	// Constrictor and destructor.
	RequestHandlerFactory(IDatabase* database);
	virtual ~RequestHandlerFactory();

	// Creates a handler for each requests.
	std::unique_ptr<LoginRequestHandler> createLoginRequestHandler(std::shared_ptr<SOCKET> userSocket) const;
	std::unique_ptr<MenuRequestHandler> createMenuRequestHandler(std::shared_ptr<LoggedUser> user) const;
	std::unique_ptr<RoomAdminRequestHandler> createAdminRequestHandler(std::shared_ptr<LoggedUser> user, std::shared_ptr<Room> room) const;
	std::unique_ptr<RoomMemberRequestHandler> createRoomMemberHandler(std::shared_ptr<LoggedUser> user, std::shared_ptr<Room> room) const;
	std::unique_ptr<GameRequestHandler> createGameRequestHandler(std::shared_ptr<LoggedUser> user, std::shared_ptr<Game> game) const;
	
	// Returns a pointer to each manager.
	LoginManager* getLoginManager() const;
	RoomManager* getRoomManager() const;
	StatisticsManager* getStatisticsManager() const;
	GameManager* getGameManager() const;
private:
	// Managers to manage all related operations of each request.
	RoomManager* m_roomManager;
	StatisticsManager* m_statisticsManager;
	IDatabase* m_database;
	LoginManager* m_loginManager;
	GameManager* m_gameManager;
};