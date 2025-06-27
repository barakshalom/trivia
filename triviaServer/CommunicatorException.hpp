#include <exception>
#include <string>
#include <iostream>

// Custom exception type for communication related errors.
class CommunicatorException : public std::exception
{
public:
    // Constructor that builds the exception message.
    CommunicatorException(const std::string& message)
    {
        this->_message = "Comms Error: " + message + "\n";
    }

    virtual char const* what() const throw()
    {
        return _message.c_str();
    }
private:
	std::string _message;
};