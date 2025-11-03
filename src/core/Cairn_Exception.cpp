#include "Cairn_Exception.h"
#include <iostream>

using namespace std;

Cairn_Exception::Cairn_Exception(const std::string& message, const int& level)
    : mMessage(message), mError(level) 
{
    if (!message.empty() && level != 0) {
        cCritical() << "Error detected: " << message << ", error value: " << std::to_string(level);
    }
}

Cairn_Exception::Cairn_Exception(const char* message, const int& level)
    : Cairn_Exception(std::string(message), level)
{ }

