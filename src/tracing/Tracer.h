#ifndef MYGAME_TRACER_H
#define MYGAME_TRACER_H


#include <chrono>
#include <iostream>
#include <string>


class Tracer
{
public:
    explicit Tracer(const std::string& name)
        : name(name),
          startTime(std::chrono::high_resolution_clock::now())
    {}

    ~Tracer()
    {
        auto end = std::chrono::high_resolution_clock::now();

        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                end - startTime
            );

        std::cout
            << "[TRACE] End: " << name
            << " took "
            << duration.count()
            << " ms"
            << std::endl;
    }

private:
    std::string name;

    std::chrono::high_resolution_clock::time_point startTime;
};

#define TRACE_ME() Tracer timer(__FUNCTION__)

#endif
