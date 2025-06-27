#include "Communicator.h"

constexpr const char* EXIT = "EXIT";

// Constant used to indicate when a client wants to disconnect.
class Server
{
public:
	// Constructor: responsible for setting up the server components (database, handler factory, communicator).
	Server();

	// Destructor: cleans up any dynamically allocated resources.
	virtual ~Server();

	// Starts the server loop: begins listening for and handling client connections.
	void run();

private:
	// Pointer to the database.
	IDatabase* m_database;
	// Factory responsible for creating request handlers for different parts of the system.
	RequestHandlerFactory* m_handlerFactory;
	// Core component responsible for managing socket communication with all clients.
	Communicator* m_communicator;


};