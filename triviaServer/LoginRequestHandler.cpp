#include "LoginRequestHandler.h"

LoginRequestHandler::LoginRequestHandler(RequestHandlerFactory* factory, std::shared_ptr<SOCKET> currentSocket)
{
    m_handlerFactory = factory;
    _currentUserSock = currentSocket;
}

bool LoginRequestHandler::isRequestRelevant(const RequestInfo& request) const
{
    return request.id == RequestId::SIGNUP || request.id == RequestId::LOGIN; //checking the request
}

RequestResult LoginRequestHandler::handleRequest(const RequestInfo& request) const
{
    RequestResult result;
    std::shared_ptr<LoggedUser> user;
    LoginResponse loginResponse;
    ErrorResponse errorResponse;

    // Making a pointer temp
    if (isRequestRelevant(request))
    {
        // Need to add the login manager and then
        if (request.id == RequestId::SIGNUP)
        {
            SignupRequest login = JsonRequestPacketDeserializer::deserializeSignupRequest(request.buffer);
            user = std::make_shared<LoggedUser>(login.username, _currentUserSock);
            loginResponse.status = m_handlerFactory->getLoginManager()->signup(login.username, login.password, login.email, login.address, login.phoneNumber, login.birthDate, user) ? 1 : 0;
        }
        else
        {
            LoginRequest login = JsonRequestPacketDeserializer::deserializeLoginRequest(request.buffer);
            user = std::make_shared<LoggedUser>(login.username, _currentUserSock);
            loginResponse.status = m_handlerFactory->getLoginManager()->signin(login.username, login.password, user) ? 1 : 0;
        }
        
        loginResponse.status == 1 ? result.newHandler = std::move(m_handlerFactory->createMenuRequestHandler(user)) : result.newHandler = std::move(m_handlerFactory->createLoginRequestHandler(_currentUserSock));
        result.response = JsonResponsePacketSerializer::serializeResponse(loginResponse);
    }
    else
    {
        // request exception
        result.newHandler = std::move(m_handlerFactory->createLoginRequestHandler(_currentUserSock));
        errorResponse.message = "Error: Not A Login/Signup Request!";
        result.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
    }
    return result;
}
