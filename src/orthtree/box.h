// Copyright 2024 oldnick85

#pragma once

#include <algorithm>
#include <array>
#include <concepts>

#include "./common.h"
#include "./point.h"

namespace orthtree
{

template <typename TCoord = float, uint DIM = 2>
    requires std::floating_point<TCoord>
class Box
{
  public:
    using Point_t = Point<TCoord, DIM>;

    Box() = default;

    Box(const Point_t& pnt1, const Point_t& pnt2)
    {
        for (uint i = 0; i < DIM; ++i)
        {
            std::tie(m_pnt_min[i], m_pnt_max[i]) = std::minmax(pnt1[i], pnt2[i]);
        }
    }

    Box(Point_t&& pnt)
    {
        m_pnt_min = std::move(pnt);
        m_pnt_max = m_pnt_min;
    }

    Box(const Point_t& pnt)
    {
        m_pnt_min = pnt;
        m_pnt_max = m_pnt_min;
    }

    const Point_t& PntMin() const { return m_pnt_min; };
    Point_t PntMid() const { return Point_t::Mid(m_pnt_min, m_pnt_max); };
    TCoord PntMid(std::size_t x) const { return Point_t::Mid(m_pnt_min, m_pnt_max, x); };
    const Point_t& PntMax() const { return m_pnt_max; };

    std::string Str() const { return m_pnt_min.Str() + "-" + m_pnt_max.Str(); }

    Box<TCoord, DIM> Intersection(const Box<TCoord, DIM>& other) const
    {
        Box<TCoord, DIM> inter;
        for (uint i = 0; i < DIM; ++i)
        {
            if (m_pnt_min[i] > other.m_pnt_min[i])
                inter.m_pnt_min[i] = m_pnt_min[i];
            else
                inter.m_pnt_min[i] = other.m_pnt_min[i];
            if (m_pnt_max[i] < other.m_pnt_max[i])
                inter.m_pnt_max[i] = m_pnt_max[i];
            else
                inter.m_pnt_max[i] = other.m_pnt_max[i];
        }
        return inter;
    }

    bool Intersect(const Box<TCoord, DIM>& other) const
    {
        for (uint i = 0; i < DIM; ++i)
        {
            if ((m_pnt_min[i] > other.m_pnt_max[i]) || (m_pnt_max[i] < other.m_pnt_min[i]))
                return false;
        }
        return true;
    }

    bool Contain(const Box<TCoord, DIM>& other) const
    {
        for (uint i = 0; i < DIM; ++i)
        {
            if ((m_pnt_min[i] > other.m_pnt_min[i]) || (m_pnt_max[i] < other.m_pnt_max[i]))
                return false;
        }
        return true;
    }

    bool ContainStrict(const Box<TCoord, DIM>& other) const
    {
        for (uint i = 0; i < DIM; ++i)
        {
            if ((m_pnt_min[i] >= other.m_pnt_min[i]) || (m_pnt_max[i] <= other.m_pnt_max[i]))
                return false;
        }
        return true;
    }

    /**
     * @brief Check if this box contains one or more of the other box orthants
     * 
     * @param other - other box
     * @return true - contains
     * @return false - not contains
     */
    bool ContainOrthant(const Box<TCoord, DIM>& other) const
    {
        for (uint i = 0; i < DIM; ++i)
        {
            const auto mid = (other.m_pnt_min[i] + other.m_pnt_max[i]) / 2;
            if (!((m_pnt_min[i] <= other.m_pnt_min[i]) and (m_pnt_max[i] >= mid)))
                return false;
            if (!((m_pnt_min[i] <= mid) and (m_pnt_max[i] >= other.m_pnt_max[i])))
                return false;
        }
        return true;
    }

    /**
     * @brief Check if this box contains other box in one of self orthants
     * 
     * @param other - other box
     * @return true - contains
     * @return false - not contains
     */
    bool ContainInOrthant(const Box<TCoord, DIM>& other) const
    {
        for (uint i = 0; i < DIM; ++i)
        {
            if (other.PntMax()[i] >= PntMax()[i])
                return false;
            if (other.PntMin()[i] <= PntMin()[i])
                return false;
            const auto mid = PntMid(i);
            if ((other.PntMin()[i] <= mid) and (other.PntMax()[i] >= mid))
                return false;
        }
        return true;
    }

  private:
    Point_t m_pnt_min;
    Point_t m_pnt_max;
};

}  // namespace orthtree