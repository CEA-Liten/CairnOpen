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
