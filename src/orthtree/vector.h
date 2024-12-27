// Copyright 2024 oldnick85

#pragma once

#include <array>
#include <concepts>

#include "./common.h"

namespace orthtree
{

/**
 * @brief Geometry vector
 * 
 * @tparam TCoord - type of coordinate
 * @tparam DIM - dimensions, number of coordinates
 */
template <typename TCoord = float, uint DIM = 2>
    requires std::floating_point<TCoord>
class Vector
{
  public:
    Vector() { std::fill(m_coord.begin(), m_coord.end(), TCoord{}); }

    Vector(std::initializer_list<TCoord> lst) { std::copy(lst.begin(), lst.end(), std::begin(m_coord)); }

    Vector(const Vector<TCoord, DIM>& rhs) : m_coord(rhs.m_coord) {}

    Vector(Vector<TCoord, DIM>&& rhs) { m_coord = std::move(rhs.m_coord); }

    Vector(const std::array<TCoord, DIM>& coord) : m_coord(coord) {}

    uint size() const { return m_coord.size(); }

    TCoord operator[](const std::size_t pos) const { return m_coord[pos]; }
    TCoord& operator[](const std::size_t pos) { return m_coord[pos]; }

    Vector<TCoord, DIM>& operator=(const Vector<TCoord, DIM>& rhs)
    {
        std::copy(rhs.m_coord.begin(), rhs.m_coord.end(), m_coord.begin());
        return *this;
    }

    Vector<TCoord, DIM>& operator*=(const TCoord coeff)
    {
        for (uint i = 0; i < DIM; ++i)
            m_coord[i] *= coeff;
        return *this;
    }

    Vector<TCoord, DIM> operator*(const TCoord coeff) const
    {
        Vector<TCoord, DIM> lhs(*this);
        lhs *= coeff;
        return lhs;
    }

    Vector<TCoord, DIM>& operator+=(const Vector<TCoord, DIM>& rhs)
    {
        for (uint i = 0; i < DIM; ++i)
            m_coord[i] += rhs[i];
        return *this;
    }

    Vector<TCoord, DIM> operator+(const Vector<TCoord, DIM>& rhs) const
    {
        Vector<TCoord, DIM> lhs(*this);
        lhs += rhs;
        return lhs;
    }

    Vector<TCoord, DIM>& operator-=(const Vector<TCoord, DIM>& rhs)
    {
        for (uint i = 0; i < DIM; ++i)
            m_coord[i] -= rhs[i];
        return *this;
    }

    Vector<TCoord, DIM> operator-(const Vector<TCoord, DIM>& rhs) const
    {
        Vector<TCoord, DIM> lhs(*this);
        lhs -= rhs;
        return lhs;
    }

  private:
    std::array<TCoord, DIM> m_coord;
};

}  // namespace orthtree