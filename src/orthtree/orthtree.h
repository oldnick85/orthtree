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
 * @brief Orthogonal tree (octree/quadtree/kd-tree) for spatial indexing
 * 
 * @tparam TValue Type of values stored in the tree. Must support hashing and equality comparison.
 * @tparam TCoord Type of coordinate values. Must satisfy std::floating_point concept.
 * @tparam DIM Dimensionality of the spatial domain (1, 2, 3, etc.).
 * @tparam GROUP_COUNT Maximum number of values per node before splitting occurs.
 * @tparam NODES_SHARE_VAL Whether values can belong to multiple nodes (experimental feature).
 * 
 * @note When NODES_SHARE_VAL is true, values overlapping node boundaries will be stored in multiple nodes.
 *       Warning: This feature is still under development and may have performance implications.
 * 
 * @details The tree recursively subdivides space into axis-aligned boxes. Each node contains
 *          values whose bounding boxes intersect with the node's spatial region. When a node
 *          exceeds GROUP_COUNT values, it splits into 2^DIM child nodes.
 * 
 * @example
 * @code
 * // Create a quadtree for 2D space
 * Tree<int, float, 2, 10> tree(Box<float, 2>{{0, 0}, {100, 100}});
 * tree.Add(1, Box<float, 2>{{10, 10}, {20, 20}});
 * auto intersections = tree.FindIntersected();
 * @endcode
 */
template <typename TValue, typename TCoord = float, std::size_t DIM = 2, std::size_t GROUP_COUNT = 10,
          bool NODES_SHARE_VAL = false>
    requires std::floating_point<TCoord> && (DIM > 0) && (GROUP_COUNT > 0)
class Tree
{
  public:
    /// Type alias for the axis-aligned bounding box
    using Box_t = Box<TCoord, DIM>;

    /// Type alias for tree nodes
    using Node_t = Node<TValue, TCoord, DIM, GROUP_COUNT, NODES_SHARE_VAL>;

    /**
     * @brief Constructs a new orthogonal tree covering the specified spatial region
     * 
     * @param area The root bounding box defining the spatial extent of the tree
     * 
     * @note The tree root is initialized at level 1 (root level)
     */
    Tree(const Box_t& area) : m_root(area, 1) {}

    /**
     * @brief Removes all values and resets the tree structure
     * 
     * @post The tree contains no values and consists only of the root node
     */
    void Clear() { m_root.Clear(); }

    /**
     * @brief Checks if a value exists in the tree
     * 
     * @param val Value to search for
     * @return true if the value exists in the tree
     * @return false if the value is not found
     * 
     * @note Complexity: O(1) average case, O(n) worst case due to hash map lookup
     */
    bool Contains(TValue val) const { return m_all_values.contains(val); }

    /**
     * @brief Retrieves the bounding box associated with a value
     * 
     * @param val Value whose bounding box is requested
     * @return Box_t The bounding box associated with the value
     * 
     * @throw std::out_of_range in debug builds if value doesn't exist
     * @pre The value must have been previously added to the tree
     */
    Box_t GetBox(TValue val) const
    {
        ORTHTREE_DEBUG_ASSERT(m_all_values.contains(val), "No such value");
        const auto iter = m_all_values.find(val);
        return iter->second;
    }

    /**
     * @brief Adds a new value with its associated bounding box to the tree
     * 
     * @param val Value to add (must be unique in the tree)
     * @param box Bounding box defining the value's spatial extent
     * 
     * @pre The box must be completely contained within the tree's root area
     * @pre The value must not already exist in the tree
     * 
     * @post The value is inserted into appropriate leaf nodes and registered in the global map
     * 
     * @throw std::logic_error in debug builds if preconditions are violated
     */
    void Add(TValue val, const Box_t& box)
    {
        ORTHTREE_DEBUG_ASSERT(area().Contain(box), "Out of area");
        ORTHTREE_DEBUG_ASSERT(!m_all_values.contains(val), "Already have value");
        m_root.Add(val, box);
        m_all_values.emplace(val, box);
    }

    /**
     * @brief Updates the bounding box of an existing value
     * 
     * @param val Value to update
     * @param box New bounding box for the value
     * 
     * @note This operation performs deletion followed by insertion. For better performance,
     *       consider using Del() and Add() separately when batch updating multiple values.
     * 
     * @pre The value must exist in the tree
     * @pre The new box must be within the tree's root area
     * 
     * @todo Optimize to update only affected nodes instead of full remove/add
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
     * @brief Removes a value from the tree
     * 
     * @param val Value to remove
     * 
     * @pre The value must exist in the tree
     * @post The value is removed from all nodes containing it and from the global map
     */
    void Del(TValue val)
    {
        ORTHTREE_DEBUG_ASSERT(m_all_values.contains(val), "No such value");
        const auto& box = GetBox(val);
        m_root.Del(val, box);
        m_all_values.erase(val);
    }

    /**
     * @brief Finds all intersecting pairs of values in the tree
     * 
     * @return std::vector<std::array<TValue, 2>> Vector of intersecting value pairs
     * 
     * @note Each pair is reported once (undirected). Complexity depends on spatial
     *       distribution but is generally O(n log n) for uniformly distributed values.
     * 
     * @warning For large trees with many intersections, this can generate significant memory usage
     */
    std::vector<std::array<TValue, 2>> FindIntersected() const
    {
        std::vector<std::array<TValue, 2>> inter;
        m_root.FindIntersected(inter);
        return inter;
    }

    /**
     * @brief Finds all values intersecting with a given bounding box
     * 
     * @param box Query bounding box
     * @return std::unordered_set<TValue> Set of values intersecting the query box
     * 
     * @note The returned set may include values whose bounding boxes only partially
     *       intersect the query box.
     */
    std::unordered_set<TValue> FindIntersected(const Box_t& box) const
    {
        std::unordered_set<TValue> inter;
        m_root.FindIntersected(box, inter);
        return inter;
    }

    /**
     * @brief Finds all values intersecting with a given value (excluding self)
     * 
     * @param val Value to check intersections for
     * @return std::unordered_set<TValue> Set of values intersecting with the given value
     * 
     * @pre The value must exist in the tree
     * @note The result excludes the query value itself
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
     * @brief Performs a deep traversal of the tree structure
     * 
     * @param on_level_cb Callback invoked when entering a new tree level
     * @param on_value_cb Callback invoked for each value encountered
     * 
     * @details The traversal visits nodes in depth-first order. The level callback
     *          receives the node's bounding box and its depth level. The value callback
     *          receives the value's bounding box, the value itself, and the depth level.
     * 
     * @note This is primarily useful for debugging, visualization, or tree analysis
     */
    void TraverseDeep(std::function<void(const Box_t& area, uint lvl)> on_level_cb,
                      std::function<void(const Box_t& box, const TValue& val, uint lvl)> on_value_cb)
    {
        m_root.TraverseDeep(on_level_cb, on_value_cb);
    }

    /**
     * @brief Returns the root bounding box (spatial extent of the entire tree)
     * 
     * @return const Box_t& Reference to the root bounding box
     */
    const Box_t& area() const { return m_root.area(); }

    /**
     * @brief Returns all values stored in the tree with their associated bounding boxes
     * 
     * @return const std::unordered_map<TValue, Box_t>& Map of value->box pairs
     * 
     * @note The returned map provides O(1) average access to any value's bounding box
     */
    const std::unordered_map<TValue, Box_t>& GetAllValues() const { return m_all_values; }

  private:
    /// Root node of the tree
    Node_t m_root;

    /// Global registry mapping values to their bounding boxes
    std::unordered_map<TValue, Box_t> m_all_values;
};

}  // namespace orthtree