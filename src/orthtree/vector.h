// Copyright 2024 oldnick85

#pragma once

#include <array>
#include <cmath>
#include <concepts>

#include "./common.h"

namespace orthtree
{

/**
 * @brief A geometric vector in DIM-dimensional space.
 * 
 * @tparam TCoord The type of coordinate values (must be floating-point).
 * @tparam DIM The number of dimensions (must be positive).
 * 
 * @note This class provides basic vector operations including arithmetic,
 *       dot product, normalization, and length calculations.
 */
template <typename TCoord = float, std::size_t DIM = 2>
    requires std::floating_point<TCoord> && (DIM > 0)
class Vector
{
  public:
    /**
     * @brief Default constructor. Initializes all coordinates to zero.
     */
    Vector() noexcept { m_coord.fill(TCoord{}); }

    /**
     * @brief Constructs a vector from an initializer list.
     * 
     * @param lst Initializer list containing coordinate values.
     * @note The list must contain exactly DIM elements.
     */
    Vector(std::initializer_list<TCoord> lst) noexcept { std::copy(lst.begin(), lst.end(), std::begin(m_coord)); }

    /**
     * @brief Copy constructor.
     * 
     * @param rhs Vector to copy from.
     */
    Vector(const Vector<TCoord, DIM>& rhs) noexcept : m_coord(rhs.m_coord) {}

    /**
     * @brief Move constructor.
     * 
     * @param rhs Vector to move from.
     */
    Vector(Vector<TCoord, DIM>&& rhs) noexcept { m_coord = std::move(rhs.m_coord); }

    /**
     * @brief Constructs a vector from an array of coordinates.
     * 
     * @param coord Array containing coordinate values.
     */
    Vector(const std::array<TCoord, DIM>& coord) noexcept : m_coord(coord) {}

    /**
     * @brief Constructs a vector from a span of coordinate values.
     * 
     * @param lst Span containing coordinate values.
     * @throw std::invalid_argument if span size doesn't match DIM (in debug builds).
     */
    Vector(const std::span<const TCoord> lst) noexcept
    {
        ORTHTREE_DEBUG_ASSERT(lst.size() == DIM, "Invalid number of coordinates");
        std::copy(lst.begin(), lst.end(), std::begin(m_coord));
    }

    /**
     * @brief Returns the number of dimensions of the vector.
     * 
     * @return Number of dimensions (DIM).
     */
    std::size_t size() const noexcept { return m_coord.size(); }

    /**
     * @brief Const accessor for coordinate at specified position.
     * 
     * @param pos Index of the coordinate (0-based).
     * @return Value of the coordinate at position pos.
     */
    TCoord operator[](const std::size_t pos) const noexcept
    {
        ORTHTREE_DEBUG_ASSERT(pos < DIM, "Index out of bounds");
        return m_coord[pos];
    }

    /**
     * @brief Non-const accessor for coordinate at specified position.
     * 
     * @param pos Index of the coordinate (0-based).
     * @return Reference to the coordinate at position pos.
     */
    TCoord& operator[](const std::size_t pos) noexcept
    {
        ORTHTREE_DEBUG_ASSERT(pos < DIM, "Index out of bounds");
        return m_coord[pos];
    }

    /**
     * @brief Copy assignment operator.
     * 
     * @param rhs Vector to copy from.
     * @return Reference to this vector.
     */
    Vector<TCoord, DIM>& operator=(const Vector<TCoord, DIM>& rhs)
    {
        std::copy(rhs.m_coord.begin(), rhs.m_coord.end(), m_coord.begin());
        return *this;
    }

    /**
     * @brief Multiplies this vector by a scalar coefficient.
     * 
     * @param coeff Scalar coefficient to multiply by.
     * @return Reference to this vector (after multiplication).
     */
    Vector<TCoord, DIM>& operator*=(const TCoord coeff)
    {
        for (std::size_t i = 0; i < DIM; ++i)
        {
            m_coord[i] *= coeff;
        }
        return *this;
    }

    /**
     * @brief Returns a new vector equal to this vector multiplied by a scalar.
     * 
     * @param coeff Scalar coefficient to multiply by.
     * @return New vector equal to (*this) * coeff.
     */
    [[nodiscard]] Vector<TCoord, DIM> operator*(const TCoord coeff) const
    {
        Vector<TCoord, DIM> lhs(*this);
        lhs *= coeff;
        return lhs;
    }

    /**
     * @brief Adds another vector to this vector.
     * 
     * @param rhs Vector to add.
     * @return Reference to this vector (after addition).
     */
    Vector<TCoord, DIM>& operator+=(const Vector<TCoord, DIM>& rhs)
    {
        for (std::size_t i = 0; i < DIM; ++i)
        {
            m_coord[i] += rhs[i];
        }
        return *this;
    }

    /**
     * @brief Returns a new vector equal to the sum of this vector and another.
     * 
     * @param rhs Vector to add.
     * @return New vector equal to (*this) + rhs.
     */
    [[nodiscard]] Vector<TCoord, DIM> operator+(const Vector<TCoord, DIM>& rhs) const
    {
        Vector<TCoord, DIM> lhs(*this);
        lhs += rhs;
        return lhs;
    }

    /**
     * @brief Subtracts another vector from this vector.
     * 
     * @param rhs Vector to subtract.
     * @return Reference to this vector (after subtraction).
     */
    Vector<TCoord, DIM>& operator-=(const Vector<TCoord, DIM>& rhs)
    {
        for (std::size_t i = 0; i < DIM; ++i)
        {
            m_coord[i] -= rhs[i];
        }
        return *this;
    }

    /**
     * @brief Returns a new vector equal to the difference of this vector and another.
     * 
     * @param rhs Vector to subtract.
     * @return New vector equal to (*this) - rhs.
     */
    [[nodiscard]] Vector<TCoord, DIM> operator-(const Vector<TCoord, DIM>& rhs) const
    {
        Vector<TCoord, DIM> lhs(*this);
        lhs -= rhs;
        return lhs;
    }

    /**
     * @brief Equality comparison operator.
     * 
     * @param rhs Vector to compare with.
     * @return true if all coordinates are equal, false otherwise.
     */
    bool operator==(const Vector<TCoord, DIM>& rhs) const { return m_coord == rhs.m_coord; }

    /**
     * @brief Inequality comparison operator.
     * 
     * @param rhs Vector to compare with.
     * @return true if any coordinate differs, false otherwise.
     */
    bool operator!=(const Vector<TCoord, DIM>& rhs) const { return !(*this == rhs); }

    /**
     * @brief Computes the dot product of this vector with another.
     * 
     * @param rhs Vector to compute dot product with.
     * @return Dot product (scalar) of the two vectors.
     */
    [[nodiscard]] TCoord Dot(const Vector& rhs) const noexcept
    {
        TCoord result{};
        for (std::size_t i = 0; i < DIM; ++i)
        {
            result += m_coord[i] * rhs.m_coord[i];
        }
        return result;
    }

    /**
     * @brief Computes the squared length (magnitude) of this vector.
     * 
     * @return Squared length of the vector (dot product with itself).
     */
    [[nodiscard]] TCoord Length2() const noexcept { return Dot(*this); }

    /**
     * @brief Computes the length (magnitude) of this vector.
     * 
     * @return Length of the vector (square root of squared length).
     */
    [[nodiscard]] TCoord Length() const noexcept { return std::sqrt(Length2()); }

    /**
     * @brief Normalizes this vector to unit length.
     * 
     * @return Reference to this vector (after normalization).
     * @note If the vector length is very small (close to zero),
     *       the vector remains unchanged to avoid division by zero.
     */
    Vector& Normalize() noexcept
    {
        TCoord len = Length();
        if (len > std::numeric_limits<TCoord>::epsilon())
        {
            *this *= (TCoord{1} / len);
        }
        return *this;
    }

    // Iterators for range-based for loops and STL algorithms

    auto begin() noexcept { return m_coord.begin(); }
    auto end() noexcept { return m_coord.end(); }
    auto begin() const noexcept { return m_coord.begin(); }
    auto end() const noexcept { return m_coord.end(); }
    auto cbegin() const noexcept { return m_coord.cbegin(); }
    auto cend() const noexcept { return m_coord.cend(); }

  private:
    std::array<TCoord, DIM> m_coord;  ///< Array storing the coordinate values.
};

}  // namespace orthtree