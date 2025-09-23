#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <glm/vec3.hpp>

struct Intersection {
    glm::vec3 point;
    float t;

    Intersection() noexcept = default;

    constexpr Intersection(glm::vec3&& point, float t) noexcept :
    point(std::move(point)), t(t)
    { }
};

#endif
