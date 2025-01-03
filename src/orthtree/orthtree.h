// Copyright 2024 oldnick85

#pragma once

#include <array>
#include <concepts>
#include <unordered_set>

#include "./box.h"
#include "./common.h"
#include "./node.h"

namespace orthtree
{

/**
 * @brief Orthtree class
 * 
 * @tparam TValue - type of stored values
 * @tparam TCoord - type of coordinates (should be real numbers)
 * @tparam DIM - area dimensions (number of coordinates)
 * @tparam GROUP_COUNT - number of values ​​in one tree node, exceeding which will lead to an attempt to split the node
 * @tparam NODES_SHARE_VAL - different values can belong to several nodes (WARNING: "true" still in process of implementation)
 */
template <typename TValue, typename TCoord = float, uint DIM = 2, uint GROUP_COUNT = 10, bool NODES_SHARE_VAL = false>
    requires std::floating_point<TCoord>
class Tree
{
  public:
    using Box_t  = Box<TCoord, DIM>;
    using Node_t = Node<TValue, TCoord, DIM, GROUP_COUNT, NODES_SHARE_VAL>;

    /**
     * @brief Construct a new orthtree object
     * 
     * @param area - space that tree covers
     */
    Tree(const Box_t& area) : m_root(area, 1) {}

    void Clear() { m_root.Clear(); }

    /**
     * @brief Check in tree contains value
     * 
     * @param val - value
     * @return true - contains
     * @return false - not contains
     */
    bool Contains(TValue val) const { return m_all_values.contains(val); }

    /**
     * @brief Get box by value
     * 
     * @param val - value
     * @return corresponding box
     */
    Box_t GetBox(TValue val) const
    {
        ORTHTREE_DEBUG_ASSERT(m_all_values.contains(val), "No such value");
        const auto iter = m_all_values.find(val);
        return iter->second;
    }

    /**
     * @brief Add value
     * 
     * @param val - value to add
     * @param box - corresponding box
     * 
     * @details val MUST not have been added previously and box MUST be inside tree area
     */
    void Add(TValue val, const Box_t& box)
    {
        ORTHTREE_DEBUG_ASSERT(area().Contain(box), "Out of area");
        ORTHTREE_DEBUG_ASSERT(!m_all_values.contains(val), "Already have value");
        m_root.Add(val, box);
        m_all_values.emplace(val, box);
    }

    /**
     * @brief Change value
     * 
     * @param val - value to add
     * @param box - new corresponding box
     * 
     * @details val MUST have been added previously and box MUST be inside tree area
     */
    void Change(TValue val, const Box_t& box)
    {
        ORTHTREE_DEBUG_ASSERT(area().Contain(box), "Out of area");
        ORTHTREE_DEBUG_ASSERT(m_all_values.contains(val), "No such value");
        // TODO: optimize box change - affect only necessary nodes
        Del(val);
        Add(val, box);
    }

    /**
     * @brief Delete value
     * 
     * @param val - value to delete
     * 
     * @details val MUST have been added previously
     */
    void Del(TValue val)
    {
        ORTHTREE_DEBUG_ASSERT(m_all_values.contains(val), "No such value");
        const auto& box = GetBox(val);
        m_root.Del(val, box);
        m_all_values.erase(val);
    }

    /**
     * @brief Find intersected values
     * 
     * @return vector of pairs of values which corresponding boxes is intersecting
     */
    std::vector<std::array<TValue, 2>> FindIntersected() const
    {
        std::vector<std::array<TValue, 2>> inter;
        m_root.FindIntersected(inter);
        return inter;
    }

    /**
     * @brief Find values intersected with given box
     * 
     * @param box - box to intersect with
     * @return set of values intersected with the box
     */
    std::unordered_set<TValue> FindIntersected(const Box_t& box) const
    {
        std::unordered_set<TValue> inter;
        m_root.FindIntersected(box, inter);
        return inter;
    }

    /**
     * @brief Find values intersected with box of given value
     * 
     * @param val - value to intersect with
     * @return set of values intersected with the value
     */
    std::unordered_set<TValue> FindIntersected(const TValue& val) const
    {
        ORTHTREE_DEBUG_ASSERT(m_all_values.contains(val), "No such value");
        std::unordered_set<TValue> inter;
        const auto iter = m_all_values.find(val);
        const auto box  = iter->second;
        m_root.FindIntersected(box, inter);
        ORTHTREE_DEBUG_ASSERT(inter.contains(val), "No intersection with self");
        inter.erase(val);
        return inter;
    }

    /**
     * @brief Traverse thru tree
     * 
     * @param on_level_cb - callback called on new reached deep level
     * @param on_value_cb - callback called on new discovered value
     */
    void TraverseDeep(std::function<void(const Box_t& area, uint lvl)> on_level_cb,
                      std::function<void(const Box_t& box, const TValue& val, uint lvl)> on_value_cb)
    {
        m_root.TraverseDeep(on_level_cb, on_value_cb);
    }

    /**
     * @brief Get tree area
     * 
     * @return tree area 
     */
    const Box_t& area() const { return m_root.area(); }

    /**
     * @brief Get the all values with boxes
     * 
     * @return map of values with boxes
     */
    const std::unordered_map<TValue, Box_t>& GetAllValues() const { return m_all_values; }

  private:
    Node_t m_root;
    std::unordered_map<TValue, Box_t> m_all_values;
};

}  // namespace orthtree