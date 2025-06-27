#pragma once
#include "IRequestHandler.h"
#include "RequestHandlerFactory.h"
#include "MenuRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"
#include "LoginManager.h"


class LoginRequestHandler : public IRequestHandler
{
public:
	// Constructor that builds the exception message.
	LoginRequestHandler(RequestHandlerFactory* factory, std::shared_ptr<SOCKET> currentSocket);
	virtual ~LoginRequestHandler() = default;
	virtual bool isRequestRelevant(const RequestInfo& request) const override;
	virtual RequestResult handleRequest(const RequestInfo& request) const override;

private:
	RequestHandlerFactory* m_handlerFactory;
	std::shared_ptr<SOCKET> _currentUserSock;
};