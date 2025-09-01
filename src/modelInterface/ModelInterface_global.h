#ifndef MODELINTERFACE_GLOBAL_H
#define MODELINTERFACE_GLOBAL_H

#if defined(_MSC_VER)
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#else
//  do nothing and hope for the best?
#define EXPORT
#define IMPORT
#pragma warning Unknown dynamic link import/export semantics.
#endif

#if defined(MODELINTERFACE_LIBRARY)
#  define MODELINTERFACESHARED_EXPORT EXPORT
#else
#  define MODELINTERFACESHARED_EXPORT IMPORT
#endif


#include <vector>
#include <string>


#endif // MODELINTERFACE_GLOBAL_H


