#include "MeshIntersecter.h"
#include <tuple>
#include <algorithm>
#include <utility>
#include <glm/vec3.hpp>
#include "NoDuplicatesAdapter.h"
#include "../../utils/lerp.h"

void MeshIntersecter::merge(std::vector<Intersection>& accumInters, std::vector<Intersection>& addInters, std::vector<Intersection>& mergeBuf)
{
    mergeBuf.resize(accumInters.size() + addInters.size());
    const NoDuplicatesAdapter last = std::merge(accumInters.begin(), accumInters.end(),
        addInters.begin(), addInters.end(),
        NoDuplicatesAdapter(mergeBuf.begin(), DIST_SQ),
        [](const Intersection& a, const Intersection& b) { return a.time < b.time; });
    mergeBuf.erase(last.getWrapped(), mergeBuf.end());
    accumInters = std::move(mergeBuf);
    addInters.clear();
    mergeBuf.clear();
}

void MeshIntersecter::computeIntersections(const glm::vec2& start, const glm::vec2& end, std::vector<Intersection>& inters)
{
    // `start` -> `end` line equation
    const float deltaX = end.x - start.x;
    const float deltaY = end.y - start.y;
    const float c = (start.y * deltaX) - (start.x * deltaY);
    //  ax + by + c = 0 where (a,b,c) = (dy, -dx, c)

    const float fabs_dx = std::fabs(deltaX);
    const float fabs_dy = std::fabs(deltaY);

    // triangle sides that are vertical in the heightmap
    if (fabs_dx > 0.0f) {
        const auto [low, high, rev] = (start.x <= end.x) ? std::make_tuple(start, end, false) :
            std::make_tuple(end, start, true);
        const int xStart = next(low.x);
        const int xEnd = prev(high.x);
        if (xEnd >= xStart)
            inters.reserve(xEnd - xStart + 1);
        for (int x = xStart; x <= xEnd; ++x) {
            // line (1, 0, -x)
            const glm::vec2 inter{ x, std::fma(deltaY, x, c) / deltaX };

            // interpolate height along triangle side
            // - equivalent to terrain.GetHeightOfTerrain(inter.x, inter.y)
            const float y_floor = std::floor(inter.y);
            const float h_floor = terrain.GetHeight(x, static_cast<int>(y_floor));
            const float h_ceil = terrain.GetHeight(x, static_cast<int>(std::ceil(inter.y)));
            const float h_interp = lerp(h_floor, h_ceil, inter.y - y_floor);

            inters.emplace_back(glm::vec3{ inter.x, h_interp, inter.y }, (inter.x - start.x) / deltaX);
        }
        if (rev)
            std::reverse(inters.begin(), inters.end());
    }

    // triangle sides that are horizontal in the heightmap
    if (fabs_dy > 0.0f) {
        const auto [low, high, rev] = (start.y <= end.y) ? std::make_tuple(start, end, false) :
            std::make_tuple(end, start, true);
        const int yStart = next(low.y);
        const int yEnd = prev(high.y);
        if (yEnd >= yStart)
            addInters.reserve(yEnd - yStart + 1);
        for (int y = yStart; y <= yEnd; ++y) {
            // line (0, 1, -y)
            const glm::vec2 inter{ std::fma(deltaX, y, -c) / deltaY, y };

            // interpolate height along triangle side
            // - equivalent to terrain.GetHeightOfTerrain(inter.x, inter.y)
            const float x_floor = std::floor(inter.x);
            const float h_floor = terrain.GetHeight(static_cast<int>(x_floor), y);
            const float h_ceil = terrain.GetHeight(static_cast<int>(std::ceil(inter.x)), y);
            const float h_interp = lerp(h_floor, h_ceil, inter.x - x_floor);

            addInters.emplace_back(glm::vec3{ inter.x, h_interp, inter.y }, (inter.y - start.y) / deltaY);
        }
        if (rev)
            std::reverse(addInters.begin(), addInters.end());

        merge(inters, addInters, mergeBuf);
    }

    // diagonal triangle sides
    const float dy_dx = deltaY + deltaX;
    if (dy_dx > 0.0f || dy_dx < 0.0f) {
        bool rev;
        if (fabs_dx > fabs_dy) {
            const auto [low, high, revx] = (start.x <= end.x) ? std::make_tuple(start, end, false) :
                std::make_tuple(end, start, true);
            rev = revx;

            const int y = static_cast<int>(std::floor(low.y));

            int xStart = static_cast<int>(std::ceil(low.x));
            if ((low.x + low.y - y - xStart) >= 0.0f)
                ++xStart;

            const int yEnd = static_cast<int>(std::floor(high.y));
            int xEnd = static_cast<int>(std::ceil(high.x));
            if ((high.x + high.y - yEnd - xEnd) <= 0.0f)
                --xEnd;
            xEnd = yEnd + xEnd - y;

            if (xEnd >= xStart)
                addInters.reserve(xEnd - xStart + 1);
            for (int x = xStart; x <= xEnd; ++x) {
                // line (1, 1, - y - x)
                const float inter_y = std::fma(deltaY, x + y, c) / dy_dx;
                const float inter_x = x + y - inter_y;
                const glm::vec2 inter{ inter_x, inter_y };

                // interpolate height along triangle side
                // - equivalent to terrain.GetHeightOfTerrain(inter.x, inter.y)
                const float x_floor = std::floor(inter.x);
                const float h_bottom = terrain.GetHeight(static_cast<int>(x_floor), static_cast<int>(std::ceil(inter.y)));
                const float h_top = terrain.GetHeight(static_cast<int>(std::ceil(inter.x)), static_cast<int>(std::floor(inter.y)));
                const float h_interp = lerp(h_bottom, h_top, inter.x - x_floor);

                addInters.emplace_back(glm::vec3{ inter.x, h_interp, inter.y }, (inter.x - start.x) / deltaX);
            }
        }
        else {
            const auto [low, high, revy] = (start.y <= end.y) ? std::make_tuple(start, end, false) :
                std::make_tuple(end, start, true);
            rev = revy;

            const int x = static_cast<int>(std::floor(low.x));

            int yStart = static_cast<int>(std::ceil(low.y));
            if ((low.x + low.y - yStart - x) >= 0.0f)
                ++yStart;

            const int xEnd = static_cast<int>(std::floor(high.x));
            int yEnd = static_cast<int>(std::ceil(high.y));
            if ((high.x + high.y - yEnd - xEnd) <= 0.0f)
                --yEnd;
            yEnd = yEnd + xEnd - x;

            if (yEnd >= yStart)
                addInters.reserve(yEnd - yStart + 1);
            for (int y = yStart; y <= yEnd; ++y) {
                // line (1, 1, - y - x)
                const float inter_y = std::fma(deltaY, x + y, c) / dy_dx;
                const float inter_x = x + y - inter_y;
                const glm::vec2 inter{ inter_x, inter_y };

                // interpolate height along triangle side
                // - equivalent to terrain.GetHeightOfTerrain(inter.x, inter.y)
                const float x_floor = std::floor(inter.x);
                const float h_bottom = terrain.GetHeight(static_cast<int>(x_floor), static_cast<int>(std::ceil(inter.y)));
                const float h_top = terrain.GetHeight(static_cast<int>(std::ceil(inter.x)), static_cast<int>(std::floor(inter.y)));
                const float h_interp = lerp(h_bottom, h_top, inter.x - x_floor);

                addInters.emplace_back(glm::vec3{ inter.x, h_interp, inter.y }, (inter.y - start.y) / deltaY);
            }
        }
        if (rev)
            std::reverse(addInters.begin(), addInters.end());

        merge(inters, addInters, mergeBuf);
    }
}
