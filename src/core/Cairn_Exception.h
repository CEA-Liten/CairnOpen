#ifndef CAIRN_EXCEPTION_H
#define CAIRN_EXCEPTION_H


#include "CairnCore_global.h"

class CAIRNCORESHARED_EXPORT Cairn_Exception : public std::exception
{
public:
    Cairn_Exception(const std::string& message = "", int level = 0) ;
    Cairn_Exception(const char* message, int level = 0);

    const char* what() const noexcept override {
        return mMessage.c_str();
    }

    // because throw is not functionnal in FBSF up to now
    void setMessage(const std::string &message) { mMessage=message; }
    void setError(const int &error) { mError=error; }

    std::string message() const { return mMessage; }
    int error() const { return mError; }

private:
    std::string mMessage ;
    int mError ;
};

#endif // CAIRN_EXCEPTION_H
