#include "MeshIntersecter.h"
#include <tuple>
#include <algorithm>
#include <glm/vec3.hpp>
#include "NoDuplicatesAdapter.h"

void MeshIntersecter::merge(std::vector<Intersection>& accumInters, std::vector<Intersection>& addInters, std::vector<Intersection>& mergeBuf)
{
    mergeBuf.resize(accumInters.size() + addInters.size());
    const NoDuplicatesAdapter last = std::merge(accumInters.begin(), accumInters.end(),
        addInters.begin(), addInters.end(),
        NoDuplicatesAdapter(mergeBuf.begin(), DIST_SQ),
        [](const Intersection& a, const Intersection& b) { return a.t < b.t; });
    mergeBuf.erase(last.getWrapped(), mergeBuf.end());
    accumInters = std::move(mergeBuf);
    addInters.clear();
    mergeBuf.clear();
}

void MeshIntersecter::computeIntersections(const glm::vec2& start, const glm::vec2& end, std::vector<Intersection>& inters)
{
    // `start` -> `end` line equation
    const float dx = end.x - start.x;
    const float dy = end.y - start.y;
    const float c = (start.y * dx) - (start.x * dy);
    //  ax + by + c = 0 where (a,b,c) = (dy, -dx, c)

    const float fabs_dx = std::fabs(dx);
    const float fabs_dy = std::fabs(dy);

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
            const glm::vec2 inter{ x, std::fma(dy, x, c) / dx };

            // interpolate height along triangle side
            // - equivalent to terrain.GetHeightOfTerrain(inter.x, inter.y)
            const float y_floor = std::floor(inter.y);
            const float h_floor = terrain.getHeight(x, y_floor);
            const float h_ceil = terrain.getHeight(x, std::ceil(inter.y));
            const float h_interp = lerp(h_floor, h_ceil, inter.y - y_floor);

            inters.emplace_back(glm::vec3{ inter.x, h_interp, inter.y }, (inter.x - start.x) / dx);
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
            const glm::vec2 inter{ std::fma(dx, y, -c) / dy, y };

            // interpolate height along triangle side
            // - equivalent to terrain.GetHeightOfTerrain(inter.x, inter.y)
            const float x_floor = std::floor(inter.x);
            const float h_floor = terrain.getHeight(x_floor, y);
            const float h_ceil = terrain.getHeight(std::ceil(inter.x), y);
            const float h_interp = lerp(h_floor, h_ceil, inter.x - x_floor);

            addInters.emplace_back(glm::vec3{ inter.x, h_interp, inter.y }, (inter.y - start.y) / dy);
        }
        if (rev)
            std::reverse(addInters.begin(), addInters.end());

        merge(inters, addInters, mergeBuf);
    }

    // diagonal triangle sides
    const float dy_dx = dy + dx;
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
                const float inter_y = std::fma(dy, x + y, c) / dy_dx;
                const float inter_x = x + y - inter_y;
                const glm::vec2 inter{ inter_x, inter_y };

                // interpolate height along triangle side
                // - equivalent to terrain.GetHeightOfTerrain(inter.x, inter.y)
                const float x_floor = std::floor(inter.x);
                const float h_bottom = terrain.getHeight(x_floor, std::ceil(inter.y));
                const float h_top = terrain.getHeight(std::ceil(inter.x), std::floor(inter.y));
                const float h_interp = lerp(h_bottom, h_top, inter.x - x_floor);

                addInters.emplace_back(glm::vec3{ inter.x, h_interp, inter.y }, (inter.x - start.x) / dx);
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
                const float inter_y = std::fma(dy, x + y, c) / dy_dx;
                const float inter_x = x + y - inter_y;
                const glm::vec2 inter{ inter_x, inter_y };

                // interpolate height along triangle side
                // - equivalent to terrain.GetHeightOfTerrain(inter.x, inter.y)
                const float x_floor = std::floor(inter.x);
                const float h_bottom = terrain.getHeight(x_floor, std::ceil(inter.y));
                const float h_top = terrain.getHeight(std::ceil(inter.x), std::floor(inter.y));
                const float h_interp = lerp(h_bottom, h_top, inter.x - x_floor);

                addInters.emplace_back(glm::vec3{ inter.x, h_interp, inter.y }, (inter.y - start.y) / dy);
            }
        }
        if (rev)
            std::reverse(addInters.begin(), addInters.end());

        merge(inters, addInters, mergeBuf);
    }
}
