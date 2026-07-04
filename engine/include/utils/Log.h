#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>

class Log
{
  public:
    static void info(const std::string &message) {
    #ifndef DEBUG
        (void)message;
    #else
        std::cout << "\033[34mINFO\033[0m: " << message << std::endl; // blue
    #endif
    }

    static void warning(const std::string &message) {
    #ifndef DEBUG
        (void)message;
    #else
        std::cout << "\033[33mWARNING\033[0m: " << message << std::endl; // yellow
    #endif
    }

    static void error(const std::string &message) {
    #ifndef DEBUG
        (void)message;
    #else
        std::cerr << "\033[31mERROR\033[0m: " << message << std::endl; // red
    #endif
    }

    static void debug(const std::string &message) {
    #ifndef DEBUG
        (void)message;
    #else
        std::cout << "\033[32mDEBUG\033[0m: " << message << std::endl; // green
    #endif
    }
};

#endif
