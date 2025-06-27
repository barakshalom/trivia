#pragma once
#include "Game.h"
#include "Room.h"
#include "SqliteDataBase.h"

// Manages active trivia games and handles game related operations.
// Responsible for creating and deleting games, storing ongoing game sessions,
// and submitting player statistics to the database after a game ends.
class GameManager {
public:
    // Constructor
    GameManager(IDatabase* database);

    /*
    * Creates a new game instance for a given room.
    * Input: room - pointer to the room for which to start a game.
    * Output: Shared pointer to the newly created game.
    */
    std::shared_ptr<Game> createGame(Room* room);

    /*
    * Deletes an existing game by its id.
    * Input: gameId - id of the game to delete.
    * Output: none.
    */
    void deleteGame(const unsigned int gameId);

    /*
    * Submits a player's game results to the database.
    * Input: data - struct containing the player's performance stats.
    * Output: none.
    */
    void submitGameStatsToDB(PlayerResults data);
private:
    IDatabase* m_database; // Pointer to the application's database.
    std::vector<std::shared_ptr<Game>> m_games; // Container for all active games.
    unsigned int m_nextGameId; // Counter for assigning unique game ids.
    std::mutex _mutex;
};
