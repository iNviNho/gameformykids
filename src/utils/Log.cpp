

#include "Log.h"
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

std::string Log::getIsoTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto tm_now = *std::gmtime(&time_t_now); // Use std::gmtime for UTC

    std::ostringstream oss;
    oss << std::put_time(&tm_now, "%Y-%m-%dT%H:%M:%S");

    auto duration = now.time_since_epoch();
    auto totalMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    auto milliseconds = totalMilliseconds - (totalSeconds * 1000); // Correct computation of fractional milliseconds

    oss << "." << std::setfill('0') << std::setw(3) << milliseconds << "Z";

    return oss.str();
}


