// Copyright 2024 oldnick85

#pragma once

#include <array>
#include <concepts>

#include "./common.h"
#include "./vector.h"

namespace orthtree
{

template <typename TCoord = float, uint N = 2>
    requires std::floating_point<TCoord>
class Point
{
  public:
    using Vector_t = Vector<TCoord, N>;

    Point() { std::fill(m_coord.begin(), m_coord.end(), TCoord{}); }

    Point(std::initializer_list<TCoord> lst) { std::copy(lst.begin(), lst.end(), std::begin(m_coord)); }

    Point(const Point<TCoord, N>& rhs) { m_coord = rhs.m_coord; }
    Point(Point<TCoord, N>&& rhs) { m_coord = std::move(rhs.m_coord); }

    std::string Str() const
    {
        std::string str{"("};
        for (uint i = 0; i < N; ++i)
            str += std::to_string(m_coord[i]) + ";";
        str += ")";
        return str;
    }

    uint size() const { return m_coord.size(); }

    TCoord operator[](std::size_t pos) const { return m_coord[pos]; }
    TCoord& operator[](std::size_t pos) { return m_coord[pos]; }

    Point<TCoord, N>& operator=(Point<TCoord, N>&& rhs)
    {
        m_coord = std::move(rhs.m_coord);
        return *this;
    }

    Point<TCoord, N>& operator=(const Point<TCoord, N>& rhs)
    {
        std::copy(rhs.m_coord.begin(), rhs.m_coord.end(), std::begin(m_coord));
        return *this;
    }

    Point<TCoord, N>& operator+=(const Vector<TCoord, N>& rhs)
    {
        for (uint i = 0; i < N; ++i)
            m_coord[i] += rhs[i];
        return *this;
    }

    Point<TCoord, N> operator+(const Vector<TCoord, N>& rhs) const
    {
        Point<TCoord, N> lhs(*this);
        lhs += rhs;
        return lhs;
    }

    Point<TCoord, N>& operator-=(const Vector<TCoord, N>& rhs)
    {
        for (uint i = 0; i < N; ++i)
            m_coord[i] -= rhs[i];
        return *this;
    }

    Point<TCoord, N> operator-(const Vector<TCoord, N>& rhs) const
    {
        Point<TCoord, N> lhs(*this);
        lhs -= rhs;
        return lhs;
    }

    Vector<TCoord, N> operator-(const Point<TCoord, N>& rhs) const
    {
        Vector<TCoord, N> res(this->m_coord);
        for (uint i = 0; i < N; ++i)
            res[i] -= rhs[i];
        return res;
    }

    static Point<TCoord, N> Mid(const Point<TCoord, N>& pnt1, const Point<TCoord, N>& pnt2)
    {
        Point<TCoord, N> pnt;
        for (uint i = 0; i < N; ++i)
            pnt[i] = (pnt1[i] + pnt2[i]) / 2;
        return pnt;
    }

    static TCoord Mid(const Point<TCoord, N>& pnt1, const Point<TCoord, N>& pnt2, std::size_t x)
    {
        ORTHTREE_DEBUG_ASSERT(x < N, "Invalid dimension");
        return (pnt1[x] + pnt2[x]) / 2;
    }

    Point<TCoord, N> MidTo(const Point<TCoord, N>& pnt_to, uint x_i) const
    {
        Point<TCoord, N> pnt{*this};
        pnt[x_i] = (m_coord[x_i] + pnt_to[x_i]) / 2;
        return pnt;
    }

  private:
    std::array<TCoord, N> m_coord;
};

}  // namespace orthtree