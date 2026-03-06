#ifndef CARMEN_EXPORT_H
#define CARMEN_EXPORT_H

/*
 * CARMEN_API marks symbols that are part of the public library interface.
 *
 * On GCC/Clang the attribute is always present; it becomes meaningful
 * when the shared library is built with -fvisibility=hidden (only
 * CARMEN_API symbols are then exported).  For static builds the
 * attribute is harmless because default visibility is already the norm.
 *
 * On Windows, callers that link to the DLL should define CARMEN_DLL
 * so the symbols are declared __declspec(dllimport).  The library
 * build itself defines CARMEN_DLL_EXPORT to get __declspec(dllexport).
 */

#if defined(_WIN32) || defined(__CYGWIN__)
  #ifdef CARMEN_DLL_EXPORT
    #define CARMEN_API __declspec(dllexport)
  #elif defined(CARMEN_DLL)
    #define CARMEN_API __declspec(dllimport)
  #else
    #define CARMEN_API
  #endif
#elif defined(__GNUC__) && (__GNUC__ >= 4)
  #define CARMEN_API __attribute__((visibility("default")))
#else
  #define CARMEN_API
#endif

#endif
