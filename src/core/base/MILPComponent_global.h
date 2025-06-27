//This file is not used anymore.
//It has been replaced by CairnCore_global.h
#ifndef MILPComponent_global_H
#define MILPComponent_global_H

#include <QtCore/qglobal.h>
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QObject>

#if defined(MILPCOMPONENT_LIBRARY)
#  define MILPCOMPONENTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MILPCOMPONENTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MILPComponent_global_H
