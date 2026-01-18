// Copyright 2024 oldnick85

#pragma once

#include <array>
#include <concepts>
#include <format>
#include <span>
#include <string>
#include <vector>

#include "./common.h"
#include "./vector.h"

namespace orthtree
{

/**
 * @brief A point in D-dimensional space with floating-point coordinates.
 * 
 * @tparam TCoord The floating-point type for coordinates (default: float).
 * @tparam DIM The number of dimensions (default: 2).
 * 
 * @requires std::floating_point<TCoord> - TCoord must be a floating-point type.
 * 
 * This class represents a point in D-dimensional space and provides
 * basic geometric operations such as translation, midpoint calculation,
 * and coordinate access.
 */
template <typename TCoord = float, std::size_t DIM = 2>
    requires std::floating_point<TCoord> && (DIM > 0)
class Point
{
  public:
    /// Alias for the vector type used with this point
    using Vector_t = Vector<TCoord, DIM>;

    /**
     * @brief Default constructor. Initializes all coordinates to zero.
     */
    Point() noexcept { m_coord.fill(TCoord{}); }

    /**
     * @brief Constructs a point from an initializer list.
     * 
     * @param lst Initializer list containing exactly DIM coordinates.
     * 
     * @note In debug builds, asserts that lst.size() == DIM.
     */
    Point(std::initializer_list<TCoord> lst) noexcept
    {
        ORTHTREE_DEBUG_ASSERT(lst.size() == DIM, "Invalid number of coordinates");
        std::copy(lst.begin(), lst.end(), std::begin(m_coord));
    }

    /**
     * @brief Constructs a point from a span of coordinates.
     * 
     * @param lst Span containing exactly DIM coordinates.
     * 
     * @note In debug builds, asserts that lst.size() == DIM.
     */
    Point(const std::span<const TCoord> lst) noexcept
    {
        ORTHTREE_DEBUG_ASSERT(lst.size() == DIM, "Invalid number of coordinates");
        std::copy(lst.begin(), lst.end(), std::begin(m_coord));
    }

    /**
     * @brief Copy constructor.
     * 
     * @param rhs Point to copy from.
     */
    Point(const Point<TCoord, DIM>& rhs) noexcept { m_coord = rhs.m_coord; }

    /**
     * @brief Move constructor.
     * 
     * @param rhs Point to move from.
     */
    Point(Point<TCoord, DIM>&& rhs) noexcept { m_coord = std::move(rhs.m_coord); }

    /**
     * @brief Returns a string representation of the point.
     * 
     * @return std::string String in format "(coord_index; y; ...)".
     */
    std::string Str() const
    {
        std::string str{"("};
        for (std::size_t i = 0; i < DIM; ++i)
        {
            str += std::format("{}", m_coord[i]);
            if (i != DIM - 1)
            {
                str += ";";  // Separate coordinates with semicolons
            }
        }
        str += ")";
        return str;
    }

    /**
     * @brief Returns the number of dimensions.
     * 
     * @return std::size_t Always returns DIM.
     */
    std::size_t size() const noexcept { return m_coord.size(); }

    /**
     * @brief Const accessor for coordinate at specified position.
     * 
     * @param pos Coordinate index (0-based).
     * @return TCoord Coordinate value.
     * 
     * @note In debug builds, asserts that pos < DIM.
     */
    TCoord operator[](std::size_t pos) const noexcept
    {
        ORTHTREE_DEBUG_ASSERT(pos < DIM, "Index out of bounds");
        return m_coord[pos];
    }

    /**
     * @brief Mutable accessor for coordinate at specified position.
     * 
     * @param pos Coordinate index (0-based).
     * @return TCoord& Reference to coordinate value.
     * 
     * @note In debug builds, asserts that pos < DIM.
     */
    TCoord& operator[](std::size_t pos) noexcept
    {
        ORTHTREE_DEBUG_ASSERT(pos < DIM, "Index out of bounds");
        return m_coord[pos];
    }

    /**
     * @brief Move assignment operator.
     * 
     * @param rhs Point to move from.
     * @return Point<TCoord, DIM>& Reference to this point.
     */
    Point<TCoord, DIM>& operator=(Point<TCoord, DIM>&& rhs) noexcept
    {
        m_coord = std::move(rhs.m_coord);
        return *this;
    }

    /**
     * @brief Copy assignment operator.
     * 
     * @param rhs Point to copy from.
     * @return Point<TCoord, DIM>& Reference to this point.
     */
    Point<TCoord, DIM>& operator=(const Point<TCoord, DIM>& rhs)
    {
        std::copy(rhs.m_coord.begin(), rhs.m_coord.end(), std::begin(m_coord));
        return *this;
    }

    /**
     * @brief Translates this point by adding a vector.
     * 
     * @param rhs Vector to add.
     * @return Point<TCoord, DIM>& Reference to this point after translation.
     */
    Point<TCoord, DIM>& operator+=(const Vector<TCoord, DIM>& rhs)
    {
        for (std::size_t i = 0; i < DIM; ++i)
        {
            m_coord[i] += rhs[i];
        }
        return *this;
    }

    /**
     * @brief Returns a new point translated by a vector.
     * 
     * @param rhs Vector to add.
     * @return Point<TCoord, DIM> New translated point.
     */
    Point<TCoord, DIM> operator+(const Vector<TCoord, DIM>& rhs) const
    {
        Point<TCoord, DIM> lhs(*this);
        lhs += rhs;
        return lhs;
    }

    /**
     * @brief Translates this point by subtracting a vector.
     * 
     * @param rhs Vector to subtract.
     * @return Point<TCoord, DIM>& Reference to this point after translation.
     */
    Point<TCoord, DIM>& operator-=(const Vector<TCoord, DIM>& rhs)
    {
        for (std::size_t i = 0; i < DIM; ++i)
        {
            m_coord[i] -= rhs[i];
        }
        return *this;
    }

    /**
     * @brief Returns a new point translated by subtracting a vector.
     * 
     * @param rhs Vector to subtract.
     * @return Point<TCoord, DIM> New translated point.
     */
    Point<TCoord, DIM> operator-(const Vector<TCoord, DIM>& rhs) const
    {
        Point<TCoord, DIM> lhs(*this);
        lhs -= rhs;
        return lhs;
    }

    /**
     * @brief Returns the vector from rhs point to this point.
     * 
     * @param rhs The starting point.
     * @return Vector<TCoord, DIM> Vector from rhs to this point.
     */
    Vector<TCoord, DIM> operator-(const Point<TCoord, DIM>& rhs) const
    {
        Vector<TCoord, DIM> res(this->m_coord);
        for (std::size_t i = 0; i < DIM; ++i)
        {
            res[i] -= rhs[i];
        }
        return res;
    }

    /**
     * @brief Equality comparison operator.
     * 
     * @param rhs Point to compare with.
     * @return true If all coordinates are equal.
     * @return false Otherwise.
     */
    bool operator==(const Point<TCoord, DIM>& rhs) const { return m_coord == rhs.m_coord; }

    /**
     * @brief Inequality comparison operator.
     * 
     * @param rhs Point to compare with.
     * @return true If any coordinate differs.
     * @return false Otherwise.
     */
    bool operator!=(const Point<TCoord, DIM>& rhs) const { return !(*this == rhs); }

    // Iterators for range-based for loops and STL algorithms
    auto begin() noexcept { return m_coord.begin(); }
    auto end() noexcept { return m_coord.end(); }
    auto begin() const noexcept { return m_coord.begin(); }
    auto end() const noexcept { return m_coord.end(); }
    auto cbegin() const noexcept { return m_coord.cbegin(); }
    auto cend() const noexcept { return m_coord.cend(); }

    /**
     * @brief Computes the midpoint between two points.
     * 
     * @param pnt1 First point.
     * @param pnt2 Second point.
     * @return Point<TCoord, DIM> Midpoint between pnt1 and pnt2.
     */
    static Point<TCoord, DIM> Mid(const Point<TCoord, DIM>& pnt1, const Point<TCoord, DIM>& pnt2)
    {
        Point<TCoord, DIM> pnt;
        for (std::size_t i = 0; i < DIM; ++i)
        {
            pnt[i] = (pnt1[i] + pnt2[i]) / 2;
        }
        return pnt;
    }

    /**
     * @brief Computes the midpoint coordinate along a specific axis.
     * 
     * @param pnt1 First point.
     * @param pnt2 Second point.
     * @param coord_index Axis index (0-based).
     * @return TCoord Midpoint coordinate along axis coord_index.
     * 
     * @note In debug builds, asserts that coord_index < DIM.
     */
    static TCoord Mid(const Point<TCoord, DIM>& pnt1, const Point<TCoord, DIM>& pnt2, std::size_t coord_index)
    {
        ORTHTREE_DEBUG_ASSERT(coord_index < DIM, "Invalid dimension");
        return (pnt1[coord_index] + pnt2[coord_index]) / 2;
    }

    /**
     * @brief Creates a new point that is the midpoint between this point
     *        and another point along a specific axis, with other coordinates
     *        taken from this point.
     * 
     * @param pnt_to The other point.
     * @param x_i Axis index (0-based).
     * @return Point<TCoord, DIM> New point with midpoint coordinate along axis x_i.
     */
    Point<TCoord, DIM> MidTo(const Point<TCoord, DIM>& pnt_to, std::size_t x_i) const
    {
        Point<TCoord, DIM> pnt{*this};
        pnt[x_i] = (m_coord[x_i] + pnt_to[x_i]) / 2;
        return pnt;
    }

  private:
    /// Internal storage of coordinates as a fixed-size array
    std::array<TCoord, DIM> m_coord;
};

}  // namespace orthtree