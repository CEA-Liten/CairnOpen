#ifndef CAIRN_EXCEPTION_H
#define CAIRN_EXCEPTION_H

#include <qglobal.h>

#include <QtCore>
#include <QException>

//#include "base/MILPComponent_global.h"
#include "CairnCore_global.h"

class CAIRNCORESHARED_EXPORT Cairn_Exception : public QException
{
public:
    void raise() const { throw *this; }
    Cairn_Exception *clone() const { return new Cairn_Exception(*this); }
    Cairn_Exception (const QString &message="", const int &level=0) ;

    // because throw is not functionnal in FBSF up to now
    void setMessage (const QString &message) {mMessage=message;}
    void setError (const int &error) {mError=error;}

    QString message () const {return mMessage;}
    int error () const {return mError;}

private:
    QString mMessage ;
    int mError ;
};

#endif // CAIRN_EXCEPTION_H
