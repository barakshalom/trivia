#include "Server.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"

Server::Server()
{
	// Create a new SQLite database instance.
	m_database = new SqliteDataBase();

	// Create the request handler factory, passing the database to it.
	m_handlerFactory = new RequestHandlerFactory(m_database);

	// Create the communicator, which handles client connections and uses the factory to process requests.
	m_communicator = new Communicator(m_handlerFactory);
}

Server::~Server()
{
	delete m_communicator;
	delete m_handlerFactory;
	delete m_database;
}

// The main server loop
void Server::run()
{
	std::string command = ""; 

	// Start a new thread to handle incoming client requests asynchronously.
	std::thread clientsThread(&Communicator::startHandleRequests, m_communicator);
	
	// Detach the thread so it runs independently from the main thread.
	clientsThread.detach();

	// TEMP: Allowing server admin to type in console commands (like EXIT).
	while (true)
	{
		std::cout << "Write Commands: ";
		std::cin >> command;

		// Type EXIT to stop the server loop.
		if (command == EXIT)
		{
			break;
		}
		else
		{
			// Echo the command back (for testing/debugging purposes).
			std::cout << command << std::endl;
		}
	}
}
