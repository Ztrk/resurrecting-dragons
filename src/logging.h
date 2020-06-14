#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <sstream>

inline std::ostringstream &noop() {
    static std::ostringstream strstream;
    return strstream;
}

#define LOG_LEVEL_ALL 0
#define LOG_LEVEL_TRACE 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_WARN 3
#define LOG_LEVEL_NONE 4

#define LOG_LEVEL LOG_LEVEL_INFO

#if LOG_LEVEL <= LOG_LEVEL_TRACE
    #define TRACE std::cout
#else
    #define TRACE noop()
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
    #define DEBUG std::cout
#else
    #define DEBUG noop()
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
    #define INFO std::cout
#else
    #define INFO noop()
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARN
    #define WARN std::cout
#else
    #define WARN noop()
#endif

#endif
