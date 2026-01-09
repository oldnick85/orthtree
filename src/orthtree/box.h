// Copyright 2024 oldnick85

#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <optional>

#include "./common.h"
#include "./point.h"

namespace orthtree
{

/**
 * @class Box
 * @brief Axis-aligned bounding box (AABB) in D-dimensional space
 * 
 * @tparam TCoord Floating-point type for coordinates (must satisfy std::floating_point)
 * @tparam DIM Dimension of the space (must be greater than 0)
 * 
 * Represents a rectangular (or hyper-rectangular) box defined by minimum and maximum
 * points along each axis. All operations assume axis-aligned boxes.
 */
template <typename TCoord = float, std::size_t DIM = 2>
    requires std::floating_point<TCoord> && (DIM > 0)
class Box
{
  public:
    using Point_t = Point<TCoord, DIM>;  ///< Type alias for Point class with matching template parameters

    /**
     * @brief Default constructor
     * @note Creates an uninitialized box. Coordinates should be set before use.
     */
    Box() noexcept = default;

    /**
     * @brief Constructs a box from two opposite corner points
     * @param pnt1 First corner point
     * @param pnt2 Second corner point
     * @note Automatically orders coordinates to ensure m_pnt_min <= m_pnt_max
     */
    Box(const Point_t& pnt1, const Point_t& pnt2) noexcept
    {
        for (std::size_t i = 0; i < DIM; ++i)
        {
            std::tie(m_pnt_min[i], m_pnt_max[i]) = std::minmax(pnt1[i], pnt2[i]);
        }
    }

    /**
     * @brief Constructs a zero-volume box from a single point
     * @param pnt The point to create box from
     * @note Both minimum and maximum points will be set to the same value
     */
    explicit Box(const Point_t& pnt) noexcept : m_pnt_min(pnt), m_pnt_max(pnt) {}

    /**
     * @brief Constructs a zero-volume box from a single point (move version)
     * @param pnt The point to create box from (will be moved from)
     */
    explicit Box(Point_t&& pnt) noexcept : m_pnt_min(std::move(pnt)), m_pnt_max(m_pnt_min) {}

    /**
     * @brief Gets the minimum corner point of the box
     * @return Const reference to the minimum point
     */
    const Point_t& PntMin() const { return m_pnt_min; };

    /**
     * @brief Calculates the midpoint (center) of the box
     * @return Point representing the center of the box
     */
    Point_t PntMid() const { return Point_t::Mid(m_pnt_min, m_pnt_max); };

    /**
     * @brief Calculates the midpoint coordinate along a specific dimension
     * @param x Dimension index (0-based)
     * @return Midpoint coordinate along the specified dimension
     */
    TCoord PntMid(std::size_t x) const { return Point_t::Mid(m_pnt_min, m_pnt_max, x); };

    /**
     * @brief Gets the maximum corner point of the box
     * @return Const reference to the maximum point
     */
    const Point_t& PntMax() const { return m_pnt_max; };

    /**
     * @brief Creates a string representation of the box
     * @return String in format "min_point-max_point"
     */
    std::string Str() const { return m_pnt_min.Str() + "-" + m_pnt_max.Str(); }

    /**
     * @brief Computes the intersection of this box with another box
     * @param other The box to intersect with
     * @return std::optional containing the intersection box if it exists,
     *         or std::nullopt if boxes don't intersect
     */
    std::optional<Box<TCoord, DIM>> Intersection(const Box<TCoord, DIM>& other) const
    {
        Box<TCoord, DIM> inter;
        for (std::size_t i = 0; i < DIM; ++i)
        {
            // Compute intersection range for each dimension
            inter.m_pnt_min[i] = std::max(m_pnt_min[i], other.m_pnt_min[i]);
            inter.m_pnt_max[i] = std::min(m_pnt_max[i], other.m_pnt_max[i]);

            // If min > max in any dimension, boxes don't intersect
            if (inter.m_pnt_min[i] > inter.m_pnt_max[i])
            {
                return std::nullopt;
            }
        }
        return inter;
    }

    /**
     * @brief Checks if this box intersects with another box
     * @param other The box to check intersection with
     * @return true if boxes intersect (share at least one point),
     *         false otherwise
     */
    bool Intersect(const Box<TCoord, DIM>& other) const
    {
        for (std::size_t i = 0; i < DIM; ++i)
        {
            // Check for separation along dimension i
            if ((m_pnt_min[i] > other.m_pnt_max[i]) || (m_pnt_max[i] < other.m_pnt_min[i]))
            {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Checks if this box completely contains another box
     * @param other The box to check containment for
     * @return true if other box is entirely inside this box (including boundaries),
     *         false otherwise
     */
    bool Contain(const Box<TCoord, DIM>& other) const
    {
        for (std::size_t i = 0; i < DIM; ++i)
        {
            // Check if other box extends beyond this box in any dimension
            if ((m_pnt_min[i] > other.m_pnt_min[i]) || (m_pnt_max[i] < other.m_pnt_max[i]))
            {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Checks if this box strictly contains another box (no boundary contact)
     * @param other The box to check strict containment for
     * @return true if other box is entirely inside this box without touching boundaries,
     *         false otherwise
     */
    bool ContainStrict(const Box<TCoord, DIM>& other) const
    {
        for (std::size_t i = 0; i < DIM; ++i)
        {
            // Check if other box touches or extends beyond boundaries
            if ((m_pnt_min[i] >= other.m_pnt_min[i]) || (m_pnt_max[i] <= other.m_pnt_max[i]))
            {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Check if this box contains one or more of the other box orthants
     * 
     * An orthant is a D-dimensional generalization of a quadrant (2D) or octant (3D).
     * This checks if the box overlaps with at least one orthant of the other box.
     * 
     * @param other The box whose orthants to check
     * @return true if this box contains at least one orthant of the other box,
     *         false otherwise
     */
    bool ContainOrthant(const Box<TCoord, DIM>& other) const
    {
        for (std::size_t i = 0; i < DIM; ++i)
        {
            const auto mid = (other.m_pnt_min[i] + other.m_pnt_max[i]) / 2;

            // Check lower half of the orthant
            if (!((m_pnt_min[i] <= other.m_pnt_min[i]) and (m_pnt_max[i] >= mid)))
            {
                return false;
            }

            // Check upper half of the orthant
            if (!((m_pnt_min[i] <= mid) and (m_pnt_max[i] >= other.m_pnt_max[i])))
            {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Check if this box contains other box in one of self orthants
     * 
     * Checks if the other box is completely contained within one orthant
     * (subdivision along midpoints) of this box.
     * 
     * @param other The box to check containment for
     * @return true if other box is entirely contained within a single orthant of this box,
     *         false otherwise
     */
    bool ContainInOrthant(const Box<TCoord, DIM>& other) const
    {
        for (std::size_t i = 0; i < DIM; ++i)
        {
            // Check if other box extends beyond this box boundaries
            if ((other.PntMax()[i] >= PntMax()[i]) or (other.PntMin()[i] <= PntMin()[i]))
            {
                return false;
            }

            const auto mid = PntMid(i);
            // Check if other box straddles the midpoint
            if ((other.PntMin()[i] <= mid) and (other.PntMax()[i] >= mid))
            {
                return false;
            }
        }
        return true;
    }

  private:
    Point_t m_pnt_min;  ///< Minimum corner point (lower bounds in all dimensions)
    Point_t m_pnt_max;  ///< Maximum corner point (upper bounds in all dimensions)
};

}  // namespace orthtree