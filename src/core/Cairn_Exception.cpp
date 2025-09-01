#include "Cairn_Exception.h"

#include <iostream>

using namespace std ;

Cairn_Exception::Cairn_Exception (const QString &message, const int &level) : QException() {
    mMessage=message ;
    mError=level ;
    if (message != "" && level != 0)
    {
        qCritical () << " Error detected : " << (message) << " error value : " << level;
    }
}

Cairn_Exception::Cairn_Exception(const std::string& message, const int& level)
{
    mMessage = QString(message.c_str());
    mError = level;
    if (message != "" && level != 0)
    {
        qCritical() << " Error detected : " << mMessage << " error value : " << level;
    }
}

Cairn_Exception::Cairn_Exception(const char* message, const int& level)
{
    Cairn_Exception(std::string(message),level);
}
