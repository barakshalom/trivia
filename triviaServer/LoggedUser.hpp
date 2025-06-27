#pragma once
#include <string>
#include <iostream>
#include <WinSock2.h>

// Represents a user who is currently logged into the system.
// Stores the username and the user's associated socket (used for communication).
class LoggedUser
{
public:
	// Constructs a LoggedUser with only a username.
	LoggedUser(const std::string& userName) {
		m_username = userName;
	}

	// Constructs a LoggedUser with a username and socket.
	LoggedUser(const std::string& userName, std::shared_ptr<SOCKET> userSocket)
	{
		m_username = userName;
		this->_userSocket = userSocket;
	}

	// Getters
	std::shared_ptr<SOCKET> getSocket() const
	{
		return _userSocket;
	}
	std::string getUsername() const
	{
		return this->m_username;
	}
private:
	// Logged user username and socket.
	std::string m_username;
	std::shared_ptr<SOCKET> _userSocket;
};
