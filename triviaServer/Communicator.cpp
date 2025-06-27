#include "Communicator.h"

std::recursive_mutex Communicator::_clientsMutex;

// A map that associates each connected client socket with its corresponding request handler.
// This map is used to route incoming client messages to the appropriate logic handler.
std::map<SOCKET, std::unique_ptr<IRequestHandler>> Communicator::m_clients;

Communicator::Communicator(RequestHandlerFactory* handlerFactory) : m_handlerFactory(handlerFactory)
{
	// Starting server socket
	m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_serverSocket == INVALID_SOCKET)
	{
		// Getting function name and socket error
		throw CommunicatorException(__FUNCTION__ " - socket");
	}

}

Communicator::~Communicator()
{
	// Trying to close the socket
	try
	{
		closesocket(this->m_serverSocket);
	}
	catch (...) {}
}

void Communicator::startHandleRequests()
{
	// Vector to store all client handler threads
	// This ensures the threads stay alive while the server is running.
	std::vector<std::thread> connections;
	try
	{
		// Prepare the server socket 
		bindAndListen();

		// Infinite loop to continuously accept new client connections.
		while (true)
		{
			std::cout << "Waiting for client connection request" << std::endl;
			
			// Accept a new client connection.
			// This is a blocking call and returns a new socket for the client.
			SOCKET clientSocket = acceptClient();

			// Create a new thread to handle communication with the connected client.
			connections.emplace_back(std::thread(&Communicator::handleNewClient, this, clientSocket));
		}
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what();
	}
}

SOCKET Communicator::acceptClient() const
{
	// This accepts the client and create a specific socket from server to this client.
	// The process will not continue until a client connects to the server.
	SOCKET client_socket = accept(m_serverSocket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
	{
		throw CommunicatorException(__FUNCTION__);
	}

	std::cout << "Client accepted. Server and client can speak" << std::endl;
	// The function that handle the conversation with the client
	return client_socket;
}

void Communicator::bindAndListen()
{
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(PORT); // port that server will listen for
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

	// Connecting between the socket and the configuration (port and etc..)
	if (bind(m_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
	{
		throw CommunicatorException(__FUNCTION__ " - bind");
	}

	//listening for incoming requests of clients
	if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		throw CommunicatorException(__FUNCTION__ " - listen");
	}
	std::cout << "Listening on port " << PORT << std::endl;
}

void Communicator::handleNewClient(const SOCKET& clientSocket)
{
	RequestResult result;
	unsigned char data[MESSAGE_SIZE] = { 0 };
	RequestInfo requestInfo;
	int bytesReceived = 0;
	unsigned int size = 0;

	{
		// Put the client in the login state.
		std::lock_guard<std::recursive_mutex> lock(_clientsMutex);
		m_clients[clientSocket] = m_handlerFactory->createLoginRequestHandler(std::make_shared<SOCKET>(clientSocket));
	}

	try
	{
		while(true)
		// Infinite loop to process requests until the connection is closed.
		{
			// Receive raw data.
			bytesReceived = recv(clientSocket, reinterpret_cast<char*>(data), MESSAGE_SIZE, 0);
			if (0 >= bytesReceived)
			{
				throw CommunicatorException("Could Not Receive Data.");
			}

			// Determine payload length request.
			for (int i = 1; i < START_PROTOCOL; i++)
			{
				if (data[i] != '\0')
				{
					size += int(data[i]);
				}
			}

			// Build RequestInfo.
			for (unsigned int i = START_PROTOCOL; i < size + START_PROTOCOL; i++)
			{
				requestInfo.buffer.push_back(static_cast<unsigned char>(data[i]));
			}
			requestInfo.id = RequestId(data[0]);
			requestInfo.receivalTime = time(nullptr);
			std::cout << "Message: " << data << std::endl;

			// Delegate client to current handler
			{
				std::lock_guard<std::recursive_mutex> lock(_clientsMutex);
				result = m_clients[clientSocket]->handleRequest(requestInfo);
				m_clients[clientSocket] = std::move(result.newHandler);
			}

			//Send response back to client
			std::string resultString = std::string(result.response.begin(), result.response.end());
			if (send(clientSocket, resultString.c_str(), resultString.size(), 0) == SOCKET_ERROR)
			{
				throw CommunicatorException("Could Not Send Data.");
			}
			std::cout << "Sended Message." << std::endl;

			// Prepare for next iteration (cleaning)			
			memset(data, 0, MESSAGE_SIZE);
			result.response.resize(0);
			requestInfo.buffer.resize(0);
			size = 0;
		}
	}
	catch (const CommunicatorException&)
	{
		m_handlerFactory->getLoginManager()->logout(m_handlerFactory->getLoginManager()->getUsernameBySocket(clientSocket));
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what();
	}
}

void Communicator::sendMessage(std::shared_ptr<SOCKET> clientSocket, Buffer message)
{
	// Transferring buffer message into a string
	std::string resultString = std::string(message.begin(), message.end());
	
	// Sending the message to client - if send() returning error throw expectation.
	if (send(*clientSocket, resultString.c_str(), resultString.size(), 0) == SOCKET_ERROR)
	{
		throw CommunicatorException("Could Not Send Data." + std::to_string(WSAGetLastError()));
	}
	std::cout << "Sended Message." << std::endl;
}

void Communicator::setClientHandler(SOCKET sock, std::unique_ptr<IRequestHandler> handler)
{
	// Lock the clients map to ensure thread safe access.
	// Using a recursive mutex allows relocking by the same thread if needed.
	std::lock_guard<std::recursive_mutex> lock(_clientsMutex);

	// Assign the request handler to the given client socket.
	// Ownership of the handler is transferred into the map using std::move,
	// so this Communicator now manages the lifecycle of the handler.
	m_clients[sock] = std::move(handler);
}