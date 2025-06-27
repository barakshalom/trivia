#include "RoomManager.h"

std::shared_ptr<Room> RoomManager::createRoom(const std::shared_ptr<LoggedUser> user, RoomData& data)
{
	const int MAX_PLAYERS = 10;
	const int MAX_QUESTIONS = 50;
	const int MAX_TIMEOUT = 40;

	std::lock_guard<std::mutex> lock(_mutex);

	// Assign a new room id.
	data.id = m_rooms.size() + 1; 

	// Create a new room object.
	auto room = std::make_shared<Room>(data);

	// Add admin as the first participant.
	room->addUser(user);

	// Validate input constraints before storing the room.
	if (data.maxPlayers <= MAX_PLAYERS && data.numOfQuestionsInGame <= MAX_QUESTIONS && data.timePerQuestion <= MAX_TIMEOUT)
	{
		m_rooms[data.id] = room; // Save room to map.
		return room;
	}

	// If constraints fail, remove room and throw error.
	m_rooms.erase(data.id);  
	throw std::invalid_argument("Room data is invalid."); 
}

// Deletes a room from the map using its id.
void RoomManager::deleteRoom(const int id)
{
	std::lock_guard<std::mutex> lock(_mutex);
	m_rooms.erase(id);
}

// Adds a user to an existing room by id, if room is not full.
// Returns (true, room) if successful, (false, room) if room is full.
std::pair<bool, std::shared_ptr<Room>> RoomManager::joinRoom(const int id, const std::shared_ptr<LoggedUser> user)
{
	std::lock_guard<std::mutex> lock(_mutex);

	// If room doesn't exist, throw an error.
	if (m_rooms.find(id) == m_rooms.end())
	{
		throw std::invalid_argument("Could Not Find A room");
	}

	// Check if room is full.
	if (m_rooms[id]->getData().maxPlayers <= m_rooms[id]->getAllUsers().size())
	{
		return { false, m_rooms[id] };
	}

	// Add user to the room.
	m_rooms[id]->addUser(user);

	return { true, m_rooms[id] };
}

bool RoomManager::getRoomState(const int id) const
{
	std::lock_guard<std::mutex> lock(_mutex);
	return m_rooms.at(id)->getData().isActive;
}

// Removes a user from a specific room.
void RoomManager::leaveRoom(std::shared_ptr<LoggedUser> user, std::shared_ptr<Room> room)
{
	std::lock_guard<std::mutex> lock(_mutex);
	m_rooms[room->getData().id]->removeUser(user);
}

// Returns a vector of RoomData for all currently active rooms.
std::vector<RoomData> RoomManager::getRooms() const
{
	std::lock_guard<std::mutex> lock(_mutex);
	std::vector<RoomData> rooms;
	for (auto& room : m_rooms)
	{
		if (room.second->getData().isActive)
		{
			rooms.push_back(room.second->getData());
		}
	}
	return rooms;
}

// Returns a shared_ptr to a room by its id.
std::shared_ptr<Room> RoomManager::getRoom(const int id)
{
	std::lock_guard<std::mutex> lock(_mutex);
	return m_rooms[id];
}
