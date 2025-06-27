#include <exception>
#include <string>
#include <iostream>

// Custom exception type for requests related errors.
class RequestException : public std::exception
{
public:

    RequestException(const std::string& message)
    {
        this->_message = "Request Error: " + message + "\n";
    }

    virtual char const* what() const throw()
    {
        return _message.c_str();
    }
private:
	std::string _message;
};