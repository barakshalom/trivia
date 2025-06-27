#include "Room.h"
#include <unordered_map>
#include <mutex>

// RoomManager is responsible for creating, tracking, and managing all rooms on the server.
// It allows users to create rooms, join/leave them, fetch room details, and delete rooms.
class RoomManager
{
public:
	RoomManager() = default;
	
	// Creates a new room with given data and admin user, returns a shared ptr to the created room.
	std::shared_ptr<Room> createRoom(const std::shared_ptr<LoggedUser>, RoomData& data);
	
	// Deletes the room with the given id from the room list.
	void deleteRoom(const int id);
	
	// Allows a user to join a room by id.
    // Returns a pair: (true if joined successfully, room ptr if exists).
	std::pair<bool, std::shared_ptr<Room>> joinRoom(const int id, const std::shared_ptr<LoggedUser> user);
	
	// Returns true if a room with the given id exists.
	bool getRoomState(const int id) const;

	// Returns a list of all active RoomData (metadata for all rooms).
	std::vector<RoomData> getRooms() const;

	// Returns a shared_ptr to a specific room by id.
	std::shared_ptr<Room> getRoom(const int id);

	// Removes a user from a room.
	void leaveRoom(std::shared_ptr<LoggedUser> user, std::shared_ptr<Room> room);
private:
	// A map from room ID to the actual Room object.
	std::unordered_map<int, std::shared_ptr<Room>> m_rooms;
	
	// Mutex to make the class thread-safe for concurrent access.
	mutable std::mutex _mutex; // Mutable is just saying that you can use it when const.
};