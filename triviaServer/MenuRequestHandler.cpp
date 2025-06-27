#include "MenuRequestHandler.h"
#include "LoginRequestHandler.h"
#include "RoomMemberRequestHandler.h"
#include "RoomAdminRequestHandler.h"

MenuRequestHandler::MenuRequestHandler(RequestHandlerFactory* factory, std::shared_ptr<LoggedUser> user)
{
    m_user = user;
    m_handlerFactory = factory;
}

bool MenuRequestHandler::isRequestRelevant(const RequestInfo& request) const
{
    // Checking all menu relevant ids
    return request.id == RequestId::CREATE_ROOM || request.id == RequestId::GET_ROOMS
        || request.id == RequestId::LOGOUT || request.id == RequestId::GET_PLAYERS_IN_ROOM
        || request.id == RequestId::JOIN_ROOM || request.id == RequestId::STATS || request.id == RequestId::GET_HIGH;
}

RequestResult MenuRequestHandler::handleRequest(const RequestInfo& request) const
{
    ErrorResponse errorResponse;
    RequestResult result;
    try
    {
        if (isRequestRelevant(request))
        {
            // A map that links each request ID to its corresponding member function handler.
            std::map<RequestId, RequestResult(MenuRequestHandler::*)(const Buffer&)> handlers = {
            {RequestId::CREATE_ROOM,&MenuRequestHandler::createRoom},
            {RequestId::GET_ROOMS,&MenuRequestHandler::getRooms},
            {RequestId::LOGOUT,&MenuRequestHandler::logout},
            {RequestId::GET_PLAYERS_IN_ROOM,&MenuRequestHandler::getPlayersInRooms},
            {RequestId::JOIN_ROOM,&MenuRequestHandler::joinRoom},
            {RequestId::GET_HIGH,&MenuRequestHandler::getScore},
            {RequestId::STATS,&MenuRequestHandler::getStats},
            };

            // Look up the request ID in the handlers map.
            auto it = handlers.find(request.id);
            if (it != handlers.end())
            {
                // Call the appropriate handler function.
                auto func = it->second;
                result = (const_cast<MenuRequestHandler*>(this)->*func)(request.buffer);
            }
        }
        else
        {
            // Request is not valid for the menu state 
            errorResponse.message = "Error: Not A Menu Request!";
            result.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
        }
    }
    catch (const std::exception& e)
    {
        result.response = JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ e.what() });
    }

    // If no new handler was assigned, keep the current MenuRequestHandler.
    if (result.newHandler == nullptr) {
        result.newHandler = std::move(m_handlerFactory->createMenuRequestHandler(m_user));
    }

    return result;
}

RequestResult MenuRequestHandler::createRoom(const Buffer& buffer)
{
    RequestResult result;
    CreateRoomResponse response;

    // Deserialize the client's buffer into a CreateRoomRequest object.
    CreateRoomRequest request = JsonRequestPacketDeserializer::deserializeCreateRoomRequest(buffer);
    
    // Fill RoomData with info from the request.
    RoomData data;
    data.maxPlayers = request.maxUsers;
    data.name = request.roomName;
    data.numOfQuestionsInGame = request.questionCount;
    data.timePerQuestion = request.answerTimeout;
    try
    {
        // Attempt to create a new room using RoomManager.
        // If successful, assign a RoomAdminRequestHandler to the user.
        auto room = m_handlerFactory->getRoomManager()->createRoom(m_user, data);
        result.newHandler = std::move(m_handlerFactory->createAdminRequestHandler(m_user, room));
        response.status = 1;
    }
    catch (const std::exception&)
    {
        // If room creation fails, return status = 0.
        response.status = 0;
    }

    // Serialize the response and return the result.
    result.response = JsonResponsePacketSerializer::serializeResponse(response);
    return result;
}

RequestResult MenuRequestHandler::getRooms(const Buffer& buffer)
{
    getRoomResponse response;
    RequestResult result;
    // Push all active room names from the RoomManager.
    for (auto& room : m_handlerFactory->getRoomManager()->getRooms())
    {
        response.rooms.push_back(room);
    }

    // Serialize the response and return the result.
    result.response = JsonResponsePacketSerializer::serializeResponse(response);
    return result;
}

RequestResult MenuRequestHandler::logout(const Buffer& buffer)
{
    RequestResult result;
    LogoutResponse logout;
    // Attempt to log out the user using their username.
    m_handlerFactory->getLoginManager()->logout(m_user->getUsername()) ? logout.status = 1 : logout.status = 0;
    
    // Serialize the logout response.
    result.response = JsonResponsePacketSerializer::serializeResponse(logout);
   
    // Reassign the client to the login handler after logging out.
    result.newHandler = std::move(m_handlerFactory->createLoginRequestHandler(m_user->getSocket()));
    return result;
}

RequestResult MenuRequestHandler::getPlayersInRooms(const Buffer& buffer)
{
    RequestResult result;
    std::vector<std::string> users;

    // Deserialize the client's buffer into a GetPlayersInRoomRequest object.
    GetPlayersInRoomRequest request =  JsonRequestPacketDeserializer::deserializeGetPlayersInRoomRequest(buffer);
   
    // Push all users from the specified room and collect their usernames.
    for (auto user : m_handlerFactory->getRoomManager()->getRoom(request.roomId)->getAllUsers())
    {
        users.push_back(user->getUsername());
    }

    // Serialize the usernames into the response.
    result.response = JsonResponsePacketSerializer::serializeResponse(getPlayersInRoomResponse{ users });
    return result;
}

RequestResult MenuRequestHandler::joinRoom(const Buffer& buffer)
{
    JoinRoomResponse join;
    RequestResult result;
    try
    {
        // Attempt to join the requested room.
        auto room = m_handlerFactory->getRoomManager()->joinRoom(JsonRequestPacketDeserializer::deserializeJoinRoomRequest(buffer).roomId, m_user);
        
        // If join succeeded, change user handler to a RoomMemberRequestHandler.
        if (room.first)
        {
            result.newHandler = std::move(m_handlerFactory->createRoomMemberHandler(m_user, room.second));
            join.status = 1;
        }
        else
        {
            // Room is full or does not exist.
            join.status = 0;
        }
    }
    catch (const std::exception&)
    {
        join.status = 0;
    }

    // Serialize the response and return result.
    result.response = JsonResponsePacketSerializer::serializeResponse(join);
    return result;
}

RequestResult MenuRequestHandler::getScore(const Buffer& buffer)
{
    HighScoreResponse score;
    RequestResult result;

    // Get the system wide high score from the StatisticsManager.
    score.highScore = m_handlerFactory->getStatisticsManager()->getHighScore();
   
    // Serialize the response and return it.
    result.response = JsonResponsePacketSerializer::serializeResponse(score);
    return result;
}


RequestResult MenuRequestHandler::getStats(const Buffer& buffer)
{
    StatsResponse stats;
    RequestResult result;

    // Get the user score from the StatisticsManager.
    stats.stats = m_handlerFactory->getStatisticsManager()->getUserStatistics(JsonRequestPacketDeserializer::deserializeStatsRequest(buffer).username);
    
    // Serialize the response and return it.
    result.response = JsonResponsePacketSerializer::serializeResponse(stats);
    return result;
}
