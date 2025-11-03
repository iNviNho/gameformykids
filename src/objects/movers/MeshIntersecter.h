#ifndef MESHINTERSECTER_H
#define MESHINTERSECTER_H

#include <vector>
#include <type_traits>
#include <cmath>
#include <glm/vec2.hpp>
#include "Intersection.h"
#include "../../terrain/Terrain.h"

class MeshIntersecter {

    /**
     * Threshold on the distance squared used to determine if two intersection points are identical.
     *
     * If the distance squared between two intersection points is less than this value, they are considered identical.
     */
    static constexpr const float DIST_SQ = 0.00000001f;

    const Terrain& terrain;

    /**
     * Temporary buffer used to hold intersections to be added to the final result.
     */
    std::vector<Intersection> addInters;

    /**
     * Temporary buffer used to hold the intersections merge result.
     */
    std::vector<Intersection> mergeBuf;

    static void merge(std::vector<Intersection>& accumInters, std::vector<Intersection>& addInters, std::vector<Intersection>& mergeBuf);

    static int next(const float f)
    {
        // if 'f' is integral, 'f' itself is returned
        const float f_ceil = std::ceil(f);
        return (f_ceil == f) ? (static_cast<int>(f_ceil) + 1) : static_cast<int>(f_ceil);
    }

    static int prev(const float f)
    {
        // if 'f' is integral, 'f' itself is returned
        const float f_floor = std::floor(f);
        return (f_floor == f) ? (static_cast<int>(f_floor) - 1) : static_cast<int>(f_floor);
    }

public:

    MeshIntersecter(const Terrain& terrain) noexcept(std::is_nothrow_default_constructible_v< std::vector<Intersection> >) : terrain(terrain), addInters{}, mergeBuf{} {}

    /**
     * Compute intersections between a line segment and the terrain mesh on the x-z plane. The intersections never include
     * the start and end position of the line segment.
     *
     * @param[in] start The start position of the line segment in the x-z plane.
     * @param[in] end The end position of the line segment in the x-z plane.
     * @param[out] inters Destination buffer to hold the computed intersections. The intersections are sorted
     *                    from @c start to @c end along the line segment.
     */
    void computeIntersections(const glm::vec2& start, const glm::vec2& end, std::vector<Intersection>& inters);
};

#endif
