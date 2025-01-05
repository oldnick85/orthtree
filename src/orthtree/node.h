// Copyright 2024 oldnick85

#pragma once

#include <array>
#include <concepts>
#include <functional>
#include <unordered_map>
#include <unordered_set>

#include "./box.h"
#include "./common.h"

namespace orthtree
{

enum SectionOrthant
{
    Low  = 0,
    High = 1,
};

template <typename TValue, typename TNode, typename TCoord, uint DIM, uint X>
    requires std::floating_point<TCoord>
class BiSection
{
  public:
    using Point_t = Point<TCoord, DIM>;
    using Box_t   = Box<TCoord, DIM>;

    BiSection(const Box_t& area, const uint level)
        : section({{Box_t(area.PntMin(), area.PntMax().MidTo(area.PntMin(), X - 1)), level},
                   {Box_t(area.PntMin().MidTo(area.PntMax(), X - 1), area.PntMax()), level}})
    {}

    void Add(TValue val, const Box_t& box, const Box_t& area)
    {
        const auto area_mid = (area.PntMax()[X - 1] + area.PntMin()[X - 1]) / 2;
        if (box.PntMin()[X - 1] <= area_mid)
            section[Low].Add(val, box, area);
        if (box.PntMax()[X - 1] >= area_mid)
            section[High].Add(val, box, area);
    }

    void Del(TValue val, const Box_t& box, const Box_t& area)
    {
        const auto area_mid = (area.PntMax()[X - 1] + area.PntMin()[X - 1]) / 2;
        if (box.PntMin()[X - 1] <= area_mid)
            section[Low].Del(val, box, area);
        if (box.PntMax()[X - 1] >= area_mid)
            section[High].Del(val, box, area);
    }

    void GatherAllValuesDeep(std::unordered_map<TValue, Box_t>& values)
    {
        section[Low].GatherAllValuesDeep(values);
        section[High].GatherAllValuesDeep(values);
    }

    void FindIntersected(std::vector<std::array<TValue, 2>>& inter) const
    {
        section[Low].FindIntersected(inter);
        section[High].FindIntersected(inter);
    }

    void FindIntersected(const Box_t& box, std::unordered_set<TValue>& inter) const
    {
        section[Low].FindIntersected(box, inter);
        section[High].FindIntersected(box, inter);
    }

    void TraverseDeep(std::function<void(const Box_t& area, uint lvl)> on_level_cb,
                      std::function<void(const Box_t& box, const TValue& val, uint lvl)> on_value_cb)
    {
        section[Low].TraverseDeep(on_level_cb, on_value_cb);
        section[High].TraverseDeep(on_level_cb, on_value_cb);
    }

  private:
    std::array<BiSection<TValue, TNode, TCoord, DIM, X - 1>, 2> section;
};

template <typename TValue, typename TNode, typename TCoord, uint DIM>
    requires std::floating_point<TCoord>
class BiSection<TValue, TNode, TCoord, DIM, 1>
{
  public:
    using Point_t = Point<TCoord, DIM>;
    using Box_t   = Box<TCoord, DIM>;

    BiSection(const Box_t& area, const uint level)
        : section({TNode(Box_t(area.PntMin(), area.PntMax().MidTo(area.PntMin(), 0)), level),
                   TNode(Box_t(area.PntMin().MidTo(area.PntMax(), 0), area.PntMax()), level)})
    {}

    void Add(TValue val, const Box_t& box, const Box_t& area)
    {
        const auto area_mid = (area.PntMax()[0] + area.PntMin()[0]) / 2;
        if (box.PntMax()[0] >= area_mid)
            section[High].Add(val, box);
        if (box.PntMin()[0] <= area_mid)
            section[Low].Add(val, box);
    }

    void Del(TValue val, const Box_t& box, const Box_t& area)
    {
        const auto area_mid = (area.PntMax()[0] + area.PntMin()[0]) / 2;
        if (box.PntMin()[0] <= area_mid)
            section[Low].Del(val, box);
        if (box.PntMax()[0] >= area_mid)
            section[High].Del(val, box);
    }

    void GatherAllValuesDeep(std::unordered_map<TValue, Box_t>& values)
    {
        section[Low].GatherAllValuesDeep(values);
        section[High].GatherAllValuesDeep(values);
    }

    void FindIntersected(std::vector<std::array<TValue, 2>>& inter) const
    {
        section[Low].FindIntersected(inter);
        section[High].FindIntersected(inter);
    }

    void FindIntersected(const Box_t& box, std::unordered_set<TValue>& inter) const
    {
        section[Low].FindIntersected(box, inter);
        section[High].FindIntersected(box, inter);
    }

    void TraverseDeep(std::function<void(const Box_t& area, uint lvl)> on_level_cb,
                      std::function<void(const Box_t& box, const TValue& val, uint lvl)> on_value_cb)
    {
        section[Low].TraverseDeep(on_level_cb, on_value_cb);
        section[High].TraverseDeep(on_level_cb, on_value_cb);
    }

  private:
    std::array<TNode, 2> section;
};

template <typename TValue, typename TCoord = float, uint DIM = 2, uint GROUP_COUNT = 10, bool NODES_SHARE_VAL = false>
    requires std::floating_point<TCoord>
class Node
{
  public:
    using Box_t       = Box<TCoord, DIM>;
    using Node_t      = Node<TValue, TCoord, DIM, GROUP_COUNT>;
    using BiSection_t = BiSection<TValue, Node_t, TCoord, DIM, DIM>;

    Node(const Box_t& area, const uint level) : m_area(area), m_level(level) {}
    ~Node() { delete m_sub_nodes; }

    void Clear()
    {
        m_bucket.clear();
        delete m_sub_nodes;
        m_sub_nodes = nullptr;
    }

    void Add(TValue val, const Box_t& box)
    {
        if (NODES_SHARE_VAL)
        {
            ORTHTREE_DEBUG_ASSERT(m_area.Intersect(box), "Out of area");
        }
        else
        {
            ORTHTREE_DEBUG_ASSERT(m_area.ContainStrict(box), "Out of area");
        }

        if ((!CanFallDeeper(box)) or ((m_bucket.size() < GROUP_COUNT) and (m_sub_nodes == nullptr)))
        {
            m_bucket.emplace(val, box);
        }
        else
        {
            if (m_sub_nodes == nullptr)
            {
                m_sub_nodes = new BiSection_t{m_area, m_level + 1};
                std::unordered_map<TValue, Box_t> new_bucket;
                for (const auto& v_it : m_bucket)
                {
                    if (CanFallDeeper(v_it.second))
                    {
                        m_sub_nodes->Add(v_it.first, v_it.second, m_area);
                    }
                    else
                    {
                        new_bucket.insert(v_it);
                    }
                }
                m_bucket = std::move(new_bucket);
            }
            m_sub_nodes->Add(val, box, m_area);
        }
        ++m_values_count;
    }

    void Del(TValue val, const Box_t& box)
    {
        --m_values_count;
        if (m_values_count <= GROUP_COUNT)
        {
            if (m_sub_nodes != nullptr)
            {
                m_sub_nodes->GatherAllValuesDeep(m_bucket);
                delete m_sub_nodes;
                m_sub_nodes = nullptr;
            }
            m_bucket.erase(val);
            ORTHTREE_DEBUG_ASSERT(m_bucket.size() <= GROUP_COUNT, "Too many elements in bucket");
        }
        else
        {
            if (m_bucket.erase(val) == 0)
            {
                ORTHTREE_DEBUG_ASSERT(m_sub_nodes != nullptr, "No value in node");
                m_sub_nodes->Del(val, box, m_area);
            }
        }
    }

    void GatherAllValuesDeep(std::unordered_map<TValue, Box_t>& values)
    {
        std::unordered_map<TValue, Box_t> res;
        if (m_sub_nodes != nullptr)
        {
            m_sub_nodes->GatherAllValuesDeep(values);
        }
        values.merge(m_bucket);
        ORTHTREE_DEBUG_ASSERT(m_bucket.empty(), "Doubled elements");
    }

    void FindIntersected(std::vector<std::array<TValue, 2>>& inter) const
    {
        auto iter1 = m_bucket.begin();
        while (iter1 != m_bucket.end())
        {
            auto iter2       = iter1;
            const auto& box1 = iter1->second;
            ++iter2;
            while (iter2 != m_bucket.end())
            {
                const auto& box2 = iter2->second;
                if (box1.Intersect(box2))
                    inter.push_back({iter1->first, iter2->first});
                ++iter2;
            }
            if (m_sub_nodes)
            {
                std::unordered_set<TValue> inter_box1;
                m_sub_nodes->FindIntersected(box1, inter_box1);
                for (const auto& val : inter_box1)
                    inter.push_back({iter1->first, val});
            }
            ++iter1;
        }
        if (m_sub_nodes)
            m_sub_nodes->FindIntersected(inter);
    }

    void FindIntersected(const Box_t& box, std::unordered_set<TValue>& inter) const
    {
        for (const auto& v_it : m_bucket)
        {
            if (box.Intersect(v_it.second))
                inter.insert(v_it.first);
        }
        if (m_sub_nodes)
            m_sub_nodes->FindIntersected(box, inter);
    }

    void TraverseDeep(std::function<void(const Box_t& area, uint lvl)> on_level_cb,
                      std::function<void(const Box_t& box, const TValue& val, uint lvl)> on_value_cb)
    {
        on_level_cb(m_area, m_level);
        for (const auto& val_it : m_bucket)
            on_value_cb(val_it.second, val_it.first, m_level);

        if (m_sub_nodes != nullptr)
            m_sub_nodes->TraverseDeep(on_level_cb, on_value_cb);
    }

    const Box_t& area() const { return m_area; }

  private:
    bool FitSection(const Box_t& box) const
    {
        for (uint i = 0; i < DIM; ++i)
        {
            const auto area_mid = (m_area.PntMax()[i] - m_area.PntMin()[i]) / 2;
            if (!((box.PntMax()[i] <= area_mid) or (box.PntMin()[i] >= area_mid)))
                return false;
        }
        return true;
    }

    bool CanFallDeeper(const Box_t& box) const
    {
        return NODES_SHARE_VAL ? !box.ContainOrthant(m_area) : m_area.ContainInOrthant(box);
    }

    const Box_t m_area;
    const uint m_level;
    std::unordered_map<TValue, Box_t> m_bucket;
    std::size_t m_values_count = 0;
    BiSection_t* m_sub_nodes   = nullptr;
};

}  // namespace orthtree