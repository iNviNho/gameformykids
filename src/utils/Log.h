

#ifndef LOG_H
#define LOG_H

#include <iostream>

class Log {
    static std::string getIsoTime();
public:
    static void log(const std::string& message) {
        std::cout << message << std::endl;
    }

    static void log(const int& message) {
        std::cout << message << std::endl;
    }

    static void logError(const std::string& message) {
        std::cerr << getIsoTime() << " ERROR: " << message << std::endl;
    }

    static void logWarning(const std::string& message) {
        std::cerr << getIsoTime() << " WARNING: " << message << std::endl;
    }

    static void logInfo(const std::string& message) {
        std::cout << "[" << getIsoTime() << "] " <<  "INFO: " << message << std::endl;
    }

    static void logInfo(const float& message) {
        std::cout << "[" << getIsoTime() << "] " <<  "INFO: " << message << std::endl;
    }
};



#endif //LOG_H
