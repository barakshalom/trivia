#include <exception>
#include <string>
#include <iostream>

// Custom exception type for database related errors.
class DataBaseException : public std::exception
{
public:
    // Constructor that builds the exception message.
    DataBaseException(const std::string& message)
    {
        this->_message = "Database Error: " + message + "\n";
    }

    virtual char const* what() const throw()
    {
        return _message.c_str();
    }
private:
	std::string _message;
};
