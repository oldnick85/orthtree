// Copyright 2024 oldnick85

#pragma once

#include <array>
#include <concepts>
#include <string>
#include <vector>

#include "./common.h"
#include "./vector.h"

namespace orthtree
{

template <typename TCoord = float, std::size_t DIM = 2>
    requires std::floating_point<TCoord>
class Point
{
  public:
    using Vector_t = Vector<TCoord, DIM>;

    Point() { std::fill(m_coord.begin(), m_coord.end(), TCoord{}); }

    Point(std::initializer_list<TCoord> lst)
    {
        ORTHTREE_DEBUG_ASSERT(lst.size() == DIM, "Invalid number of coordinates");
        std::copy(lst.begin(), lst.end(), std::begin(m_coord));
    }

    Point(std::vector<TCoord> lst)
    {
        ORTHTREE_DEBUG_ASSERT(lst.size() == DIM, "Invalid number of coordinates");
        std::copy(lst.begin(), lst.end(), std::begin(m_coord));
    }

    Point(const Point<TCoord, DIM>& rhs) { m_coord = rhs.m_coord; }

    Point(Point<TCoord, DIM>&& rhs) { m_coord = std::move(rhs.m_coord); }

    std::string Str() const
    {
        std::string str{"("};
        for (uint i = 0; i < DIM; ++i)
            str += std::to_string(m_coord[i]) + ";";
        str += ")";
        return str;
    }

    uint size() const { return m_coord.size(); }

    TCoord operator[](std::size_t pos) const { return m_coord[pos]; }
    TCoord& operator[](std::size_t pos) { return m_coord[pos]; }

    Point<TCoord, DIM>& operator=(Point<TCoord, DIM>&& rhs)
    {
        m_coord = std::move(rhs.m_coord);
        return *this;
    }

    Point<TCoord, DIM>& operator=(const Point<TCoord, DIM>& rhs)
    {
        std::copy(rhs.m_coord.begin(), rhs.m_coord.end(), std::begin(m_coord));
        return *this;
    }

    Point<TCoord, DIM>& operator+=(const Vector<TCoord, DIM>& rhs)
    {
        for (uint i = 0; i < DIM; ++i)
            m_coord[i] += rhs[i];
        return *this;
    }

    Point<TCoord, DIM> operator+(const Vector<TCoord, DIM>& rhs) const
    {
        Point<TCoord, DIM> lhs(*this);
        lhs += rhs;
        return lhs;
    }

    Point<TCoord, DIM>& operator-=(const Vector<TCoord, DIM>& rhs)
    {
        for (uint i = 0; i < DIM; ++i)
            m_coord[i] -= rhs[i];
        return *this;
    }

    Point<TCoord, DIM> operator-(const Vector<TCoord, DIM>& rhs) const
    {
        Point<TCoord, DIM> lhs(*this);
        lhs -= rhs;
        return lhs;
    }

    Vector<TCoord, DIM> operator-(const Point<TCoord, DIM>& rhs) const
    {
        Vector<TCoord, DIM> res(this->m_coord);
        for (uint i = 0; i < DIM; ++i)
            res[i] -= rhs[i];
        return res;
    }

    static Point<TCoord, DIM> Mid(const Point<TCoord, DIM>& pnt1, const Point<TCoord, DIM>& pnt2)
    {
        Point<TCoord, DIM> pnt;
        for (uint i = 0; i < DIM; ++i)
            pnt[i] = (pnt1[i] + pnt2[i]) / 2;
        return pnt;
    }

    static TCoord Mid(const Point<TCoord, DIM>& pnt1, const Point<TCoord, DIM>& pnt2, std::size_t x)
    {
        ORTHTREE_DEBUG_ASSERT(x < DIM, "Invalid dimension");
        return (pnt1[x] + pnt2[x]) / 2;
    }

    Point<TCoord, DIM> MidTo(const Point<TCoord, DIM>& pnt_to, uint x_i) const
    {
        Point<TCoord, DIM> pnt{*this};
        pnt[x_i] = (m_coord[x_i] + pnt_to[x_i]) / 2;
        return pnt;
    }

  private:
    std::array<TCoord, DIM> m_coord;
};

}  // namespace orthtree