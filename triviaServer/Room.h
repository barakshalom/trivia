#pragma once
#include "Structs.h"
#include <mutex>

// This class represents a trivia room, including its metadata and list of users.
class Room
{
public:
	// Default constructor.
	Room() = default;

	// Constructor that initializes the room with RoomData.
	Room(const RoomData& data);

	// Adds a user to the room.
	void addUser(const  std::shared_ptr<LoggedUser> user);
	
	// Removes a user from the room.
	void removeUser(std::shared_ptr<LoggedUser> user);
	
	// Returns a list of all users currently in the room.
	std::vector< std::shared_ptr<LoggedUser>> getAllUsers() const;
	
	// Returns the room's metadata.
	RoomData getData() const;

private:
	// Metadata about the room.
	RoomData m_metadata;

	// List of users currently in the room.
	std::vector< std::shared_ptr<LoggedUser>> m_users;
	
	// Mutex for thread safe access to room data.
	mutable std::mutex _mutex;
};