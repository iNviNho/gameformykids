

#include "Log.h"
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

std::string Log::getIsoTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto tm_now = *std::localtime(&time_t_now);

    std::ostringstream oss;
    oss << std::put_time(&tm_now, "%Y-%m-%dT%H:%M:%S");
    auto duration = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
    oss << "." << std::setfill('0') << std::setw(3) << milliseconds << "Z";

    return oss.str();
}


