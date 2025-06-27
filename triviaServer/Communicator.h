#include <WinSock2.h>
#include <windows.h>
#include <map>
#include <iostream>
#include <thread>
#include <vector>
#include "LoginRequestHandler.h"
#include "CommunicatorException.hpp"
#include <mutex>
#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"
#include "RequestHandlerFactory.h"

//message size
#define MESSAGE_SIZE 1024
#define START_PROTOCOL 5

constexpr const int PORT = 10000;

class Communicator
{
public:
	// Constrictor and destructor.
	Communicator(RequestHandlerFactory* handlerFactory);
	virtual ~Communicator();

	/*
	* Handles incoming clients connections by accepting them and creating a new thread for each.
	* Input: none.
	* Output: none.
	*/
	void startHandleRequests();

	/*
	* Sends a message to the specified client socket.
	* Input: clientSocket - shared pointer to the client's socket.
			 message - buffer containing the message to be sent.
	* Output: none.
	*/
	static void sendMessage(std::shared_ptr<SOCKET> clientSocket, Buffer message);
	
	/*
	* Connect a new request handler with the given client socket.
	* This function updates the handler assigned to the specified client,
	* allowing the server to transition the client to a new state.
	* Input: sock - shared pointer to the client's socket.
			 handler - unique pointer to the new IRequestHandler for this client.
	* Output: none.
	*/
	static void setClientHandler(SOCKET sock, std::unique_ptr<IRequestHandler> handler);

private:
	/*
	* Accepts the client and create a specific socket from server to this client.
	* Input: none.
	* Output: client's socket.
	*/
	SOCKET acceptClient() const;

	void bindAndListen();

	/*
	* Handles all communication with a single connected client.
	*	Start position: assigns a LoginRequestHandler to the client.
	*	Body: Enters a loop of -- receive - parse - delegate to handler - send response.
	* Input: clientSocket - shared pointer to the client's socket.
	* Output: none.
	*/
	void handleNewClient(const SOCKET& clientSocket);

	static std::map<SOCKET, std::unique_ptr<IRequestHandler>> m_clients;
	SOCKET m_serverSocket;
	RequestHandlerFactory* m_handlerFactory;
	static std::recursive_mutex _clientsMutex;
};