#ifndef MODEL_GLOBAL_H
#define MODEL_GLOBAL_H

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

#ifdef MODELS_LIBRARY
#define MODELS_DECLSPEC EXPORT
#else
#define MODELS_DECLSPEC IMPORT
#endif

#endif // MODEL_GLOBAL_H


