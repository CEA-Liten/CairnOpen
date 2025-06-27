#ifndef CAIRNCORE_GLOBAL_H
#define CAIRNCORE_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QObject>

#if defined(CAIRNCORE_LIBRARY)
#  define CAIRNCORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define CAIRNCORESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CAIRNCORE_GLOBAL_H
