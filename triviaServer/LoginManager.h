#pragma once
#include <vector>
#include "SqliteDataBase.h"
#include "LoggedUser.hpp"
#include "DataBaseException.hpp"
#include <regex>

// Handles user authentication and session management.
// This class is responsible for managing login, signup, and logout operations,
// verifying credentials with the database, and tracking currently logged users.
class LoginManager {
public:
    // Constrictor and destructor.
    LoginManager(IDatabase* database);
    virtual ~LoginManager();

    /*
    * Logs out a user by removing them from the list of currently logged users.
    * Input: username - username of the user to log out.
    * Output: true if the user was found and removed, false otherwise.
    */
    bool logout(const std::string& userName);

    /*
    * Attempts to sign up a new user by validating input and inserting them into the database.
    * Input: all user info for sign up.
    * Output: true if the signup was successful, false otherwise.
    */
    bool signup(const std::string& userName, const std::string& password, const std::string& email, const std::string& address, const std::string& phoneNumber, const std::string& birthDate, std::shared_ptr<LoggedUser> user);
   
    /*
    * Attempts to sign in a user by verifying input and ensuring they are not already logged in.
    * Input: user name and password to login.
    * Output: true if the login was successful, false otherwise.
    */
    bool signin(const std::string& userName, const std::string& password, std::shared_ptr<LoggedUser> user);
    
    /*
    * Retrieves the username associated with a given client socket.
    * Input: socket The socket to search for.
    * Output: corresponding username if found. otherwise, an empty string.
    */
    std::string getUsernameBySocket(const SOCKET socket);

private:
    IDatabase* m_database;
    std::vector<std::shared_ptr<LoggedUser>> m_loggedUsers;

    // Validation checking for all sing up information.
    bool isPassValid(const std::string& password);
    bool isAddressValid(const std::string& address);
    bool isDateValid(const std::string& date);
    bool isEmailValid(const std::string& email);
    bool isPhoneValid(const std::string& address);
};
