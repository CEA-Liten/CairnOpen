#ifndef MODELINTERFACE_GLOBAL_H
#define MODELINTERFACE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MODELINTERFACE_LIBRARY)
#  define MODELINTERFACESHARED_EXPORT Q_DECL_EXPORT
#else
#  define MODELINTERFACESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MODELINTERFACE_GLOBAL_H


