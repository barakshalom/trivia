#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <exception>
#include <iostream>

// This class is responsible for initializing and cleaning up Winsock (Windows Sockets API).
class WSAInitializer
{
public:
	// Constructor will initialize Winsock.
	WSAInitializer();
	// Destructor will clean up Winsock.
	~WSAInitializer();
};