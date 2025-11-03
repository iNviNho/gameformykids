#ifndef LERP_H
#define LERP_H

#include <cmath>

inline
float lerp(const float a, const float b, const float t)
{
    return std::fma(t, b - a, a);
}

#endif
