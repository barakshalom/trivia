#include "RequestHandlerFactory.h"
#include "MenuRequestHandler.h"
#include "LoginRequestHandler.h"
#include "RoomAdminRequestHandler.h"
#include "RoomMemberRequestHandler.h"
#include "GameRequestHandler.h"

RequestHandlerFactory::RequestHandlerFactory(IDatabase* database)
{
	m_database = database;
	m_loginManager = new LoginManager(m_database);
	m_roomManager = new RoomManager();
	m_statisticsManager = new StatisticsManager(m_database);
	m_gameManager = new GameManager(m_database);
}

RequestHandlerFactory::~RequestHandlerFactory()
{
	delete m_loginManager;
	delete m_roomManager;
	delete m_statisticsManager;
	delete m_gameManager;
	m_database->close();
}

std::unique_ptr<LoginRequestHandler> RequestHandlerFactory::createLoginRequestHandler(std::shared_ptr<SOCKET> userSocket) const
{
	return std::make_unique<LoginRequestHandler>(const_cast<RequestHandlerFactory*>(this), userSocket); //created factory
}

std::unique_ptr<MenuRequestHandler> RequestHandlerFactory::createMenuRequestHandler(std::shared_ptr<LoggedUser> user) const
{
	return std::make_unique<MenuRequestHandler>(const_cast<RequestHandlerFactory*>(this), user);
}

std::unique_ptr<RoomAdminRequestHandler> RequestHandlerFactory::createAdminRequestHandler(std::shared_ptr<LoggedUser> user, std::shared_ptr<Room> room) const
{
	return std::make_unique<RoomAdminRequestHandler>(const_cast<RequestHandlerFactory*>(this), user, room);
}

std::unique_ptr<RoomMemberRequestHandler> RequestHandlerFactory::createRoomMemberHandler(std::shared_ptr<LoggedUser> user, std::shared_ptr<Room> room) const
{
	return std::make_unique<RoomMemberRequestHandler>(const_cast<RequestHandlerFactory*>(this), user, room);
}

std::unique_ptr<GameRequestHandler> RequestHandlerFactory::createGameRequestHandler(std::shared_ptr<LoggedUser> user, std::shared_ptr<Game> game) const
{
	return std::make_unique<GameRequestHandler>(const_cast<RequestHandlerFactory*>(this), user, game);
}

LoginManager* RequestHandlerFactory::getLoginManager() const
{
	return m_loginManager;
}

RoomManager* RequestHandlerFactory::getRoomManager() const
{
	return m_roomManager;
}

StatisticsManager* RequestHandlerFactory::getStatisticsManager() const
{
	return m_statisticsManager;
}

GameManager* RequestHandlerFactory::getGameManager() const {
	return m_gameManager;
}