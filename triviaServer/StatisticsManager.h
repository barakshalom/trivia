#include "IDatabase.h"
#include <vector>
#include <mutex>

// The StatisticsManager class is responsible for retrieving
// statistical data from the database related to player performance.
class StatisticsManager
{
public:
	// Constructor and destructor 
	StatisticsManager(IDatabase* database);
	virtual ~StatisticsManager();
	
	// Returns the top high scores (top 3).
	std::vector<std::string> getHighScore() const;

	// Returns statistics for a specific user.
	std::vector<std::string> getUserStatistics(const std::string& userName) const;
private:
	IDatabase* m_database; // Pointer to database.
	mutable std::mutex _mutex;
};