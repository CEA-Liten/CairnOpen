#ifndef CAIRNCORE_GLOBAL_H
#define CAIRNCORE_GLOBAL_H


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

#if defined(CAIRNCORE_LIBRARY)
#  define CAIRNCORESHARED_EXPORT EXPORT
#else
#  define CAIRNCORESHARED_EXPORT IMPORT
#endif

#include "CairnLogger.h"
#include "CairnObject.h"


#include "json.hpp"
using json = nlohmann::json;
using ojson = nlohmann::ordered_json;

#include <filesystem>
namespace fs = std::filesystem;

typedef unsigned int uint;

#endif // CAIRNCORE_GLOBAL_H
