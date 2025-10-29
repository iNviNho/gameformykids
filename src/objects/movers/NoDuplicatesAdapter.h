#ifndef NODUPLICATESADAPTER_H
#define NODUPLICATESADAPTER_H

#include <iterator>
#include <type_traits>
#include <optional>
#include "Intersection.h"

template<typename OutputIt>
class NoDuplicatesAdapter
{
public:

    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

private:

    OutputIt wrapped;

    float threshSq;

    std::optional<Intersection> prev;

    /**
     * Since 't' is determined from the location ('x','y','z'), and 'y' is determined
     * from 'x' and 'z', we only need to check 2D distance in 'x' and 'z'.
     */
    static constexpr float distanceSq(const Intersection& prev, const Intersection& other) noexcept
    {
        const float dx = other.point.x - prev.point.x;
        const float dz = other.point.z - prev.point.z;
        return dx * dx + dz * dz;
    }

public:

    constexpr NoDuplicatesAdapter(const OutputIt& wrapped, const float threshSq) noexcept(std::is_nothrow_copy_constructible_v<OutputIt>)
    : wrapped(wrapped), threshSq(threshSq), prev{}
    { }

    constexpr NoDuplicatesAdapter(OutputIt&& wrapped, const float threshSq) noexcept(std::is_nothrow_move_constructible_v<OutputIt>)
    : wrapped(std::move(wrapped)), threshSq(threshSq), prev{}
    { }

    constexpr const OutputIt& getWrapped() const noexcept
    {
        return wrapped;
    }

    constexpr NoDuplicatesAdapter& operator*() noexcept
    {
        return *this;
    }

    constexpr NoDuplicatesAdapter& operator++() noexcept
    {
        return *this;
    }

    constexpr NoDuplicatesAdapter& operator++(int) noexcept
    {
        return *this;
    }

    constexpr NoDuplicatesAdapter& operator=(const Intersection& v)
    {
        if (prev && distanceSq(*prev, v) < threshSq)
            return *this;
        *wrapped++ = v;
        prev = v;
        return *this;
    }
};

#endif
