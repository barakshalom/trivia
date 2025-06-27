#pragma comment (lib, "ws2_32.lib")
#include "Server.h"
#include "WSAInitializer.h"
int main(void)
{
	WSAInitializer wsa;
	try
	{
		Server server;
		std::cout << "Trivia By Omer And Barak!\nPort 10000!" << std::endl;

		server.run();
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what();
	}

	return 0;
}