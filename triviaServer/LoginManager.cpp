#include "LoginManager.h"

LoginManager::LoginManager(IDatabase* database)
{
	m_database = database;
}

LoginManager::~LoginManager() {}

bool LoginManager::isPassValid(const std::string& password) {
	// Password pattern - at least 1 uppercase and digit and lowercase and !@#$%^&* and at least 8 chars
	const std::regex pattern("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*]).{8,}$");
	// Returning regex match answer - checking is password is matching the pattern
	return std::regex_match(password, pattern);
}

bool LoginManager::isEmailValid(const std::string& email) {
	// Email pattern - starts with digits and chars - @ - domain ending (chars.chart....)
	const std::regex pattern("^[a-zA-Z0-9-]+@[a-zA-Z0-9-]+(\\.[a-zA-Z0-9-]+)*\\.[a-zA-Z]{2,}$");
	// Returning regex match answer - checking is email is matching the pattern
	return std::regex_match(email, pattern);
}

bool LoginManager::isPhoneValid(const std::string& phone) {
	// Phone pattern - prefix starts with 0 + another digit (0?) ++ 3 first phone numbers (05?) ++ 6 rest numbers
	const std::regex pattern("^05[012345689][0-9]{7}$");
	// Returning regex match answer - checking is email is matching the pattern
	return std::regex_match(phone, pattern);
}

bool LoginManager::isDateValid(const std::string& date) {
	// Date pattern - 2 digits / 2 digits / 4 digits
	const std::regex pattern("^[0-9]{2}[/.][0-9]{2}[/.][0-9]{4}$");
	// Returning regex match answer - checking is date is matching the pattern
	return std::regex_match(date, pattern);
}

bool LoginManager::isAddressValid(const std::string& address) {
	// Address pattern - Street, Apt, City - chars, digits, chars
	const std::regex pattern("^\s*[^,]+,\s*[^,]+,\s*[^,]+$");
	// Returning regex match answer - checking is address is matching the pattern
	return std::regex_match(address, pattern);
}


bool LoginManager::signin(const std::string& userName, const std::string& password, std::shared_ptr<LoggedUser> user)
{
	// Check if the username and password match the database records.
	if (!m_database->doesPasswordMatch(userName, password)) 
	{
		return false;
	}

	// Check if the user is already logged in.
	for (auto user : m_loggedUsers)
	{
		if (user->getUsername() == userName)
		{
			return false;
		}
	}

	// Add user to the logged users list
	m_loggedUsers.push_back(user);
	return true;
}

std::string LoginManager::getUsernameBySocket(const SOCKET socket)
{
	for (auto user : m_loggedUsers)
	{
		if (*user->getSocket().get() == socket)
		{
			return user->getUsername();
		}
	}
	return "";
}

bool LoginManager::signup(const std::string& userName, const std::string& password, const std::string& email, const std::string& address, const std::string& phoneNumber, const std::string& birthDate, std::shared_ptr<LoggedUser> user)
{
	// Validate all user input fields.
	if (isPassValid(password) && isEmailValid(email) && isAddressValid(address) && isPhoneValid(phoneNumber) && isDateValid(birthDate))
	{
		try
		{
			// Try adding the new user to the database.
			m_database->addNewUser(userName, password, email, address, phoneNumber, birthDate);
			m_loggedUsers.push_back(user);
		}
		catch (const DataBaseException&)
		{
			return false;
		}
		// Signup succeeded
		return true;
	}
	// If any validation failed
	return false;
}

bool LoginManager::logout(const std::string& userName) {
	int cout = 0;

	// Iterate through the list of logged users.
	for (auto& user : m_loggedUsers) {
		// If the username matches.
		if (user->getUsername() == userName) {
			// Remove the user from the vector using their index.
			m_loggedUsers.erase(m_loggedUsers.begin() + cout);
			return true;
		}
		cout++;
	}
	// User not found in the logged list.
	return false;
}
