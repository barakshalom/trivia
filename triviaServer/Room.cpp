#include "Room.h"

// Constructor that initializes the room's metadata and marks the room as active.
Room::Room(const RoomData& data)
{
	this->m_metadata = data;
	this->m_metadata.isActive = true; // Set the room status to active upon creation
} 

void Room::addUser(const std::shared_ptr<LoggedUser> user)
{
	std::lock_guard<std::mutex> lock(_mutex);

	// Only add the user if room is active and not full.
	if (m_metadata.isActive && m_metadata.maxPlayers > m_users.size())
	{
		this->m_users.push_back(user);
	}
}

void Room::removeUser(std::shared_ptr<LoggedUser> user)
{
	std::lock_guard<std::mutex> lock(_mutex);
	for (auto userIt = m_users.begin(); userIt != m_users.end(); userIt++)
	{
		// Compare usernames to find the user to remove.
		if (userIt->get()->getUsername() == user->getUsername())
		{
			m_users.erase(userIt);
			break;
		}
	}
}

std::vector< std::shared_ptr<LoggedUser>> Room::getAllUsers() const
{
	std::lock_guard<std::mutex> lock(_mutex);
	std::vector< std::shared_ptr<LoggedUser>> users;

	// Copy users from internal list to returnable list.
	for (auto user : m_users)
	{
		users.push_back(user);
	}
	return users;
}

RoomData Room::getData() const
{
	return m_metadata;
}
