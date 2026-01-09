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

/**
 * @brief Enumeration representing the two orthants (subdivisions) of a bisection.
 * 
 * In a binary space partitioning scheme, each dimension can be divided into
 * two halves: Low (lower half) and High (upper half).
 */
enum SectionOrthant
{
    Low  = 0, /**< Lower half of the bisected dimension */
    High = 1, /**< Upper half of the bisected dimension */
};

/**
 * @brief Template class for recursive binary space partitioning (bisection).
 * 
 * This class implements a recursive bisection of D-dimensional space along
 * successive dimensions (from X down to 1). When X reaches 1, it terminates
 * recursion and creates leaf nodes of type TNode.
 * 
 * @tparam TValue The type of value stored in the tree (typically an identifier)
 * @tparam TNode The type of leaf node used when bisection terminates
 * @tparam TCoord Coordinate type (must be floating-point)
 * @tparam DIM Dimensionality of the space (must be > 0)
 * @tparam X Current dimension being bisected (decrements recursively until 1)
 * 
 * @note This is a recursive template where X decreases with each recursion level
 */
template <typename TValue, typename TNode, typename TCoord, std::size_t DIM, std::size_t X>
    requires std::floating_point<TCoord> && (DIM > 0)
class BiSection
{
  public:
    /// Type alias for a point in D-dimensional space
    using Point_t = Point<TCoord, DIM>;
    /// Type alias for an axis-aligned bounding box
    using Box_t = Box<TCoord, DIM>;

    /**
     * @brief Constructs a bisection by dividing the given area along dimension X.
     * 
     * Creates two child sections: Low (lower half) and High (upper half).
     * The division is made at the midpoint of dimension (X-1) since dimensions
     * are 1-indexed in this implementation.
     * 
     * @param area The bounding box to bisect
     * @param level Current tree depth level (passed to child sections)
     */
    BiSection(const Box_t& area, const uint level)
        : section({{Box_t(area.PntMin(), area.PntMax().MidTo(area.PntMin(), X - 1)), level},
                   {Box_t(area.PntMin().MidTo(area.PntMax(), X - 1), area.PntMax()), level}})
    {}

    /**
     * @brief Adds a value with its associated bounding box to the appropriate child sections.
     * 
     * If the value's box straddles the bisection plane (midpoint of dimension X-1),
     * it will be added to both child sections. Otherwise, it goes to only one.
     * 
     * @param val The value to add
     * @param box Bounding box associated with the value
     * @param area The current area being bisected (for midpoint calculation)
     */
    void Add(TValue val, const Box_t& box, const Box_t& area)
    {
        const auto area_mid = (area.PntMax()[X - 1] + area.PntMin()[X - 1]) / 2;
        if (box.PntMin()[X - 1] <= area_mid)
            section[Low].Add(val, box, area);
        if (box.PntMax()[X - 1] >= area_mid)
            section[High].Add(val, box, area);
    }

    /**
     * @brief Removes a value from the appropriate child sections.
     * 
     * Similar to Add(), determines which child sections contain the value
     * and removes it from those sections.
     * 
     * @param val The value to remove
     * @param box Bounding box associated with the value
     * @param area The current area being bisected (for midpoint calculation)
     */
    void Del(TValue val, const Box_t& box, const Box_t& area)
    {
        const auto area_mid = (area.PntMax()[X - 1] + area.PntMin()[X - 1]) / 2;
        if (box.PntMin()[X - 1] <= area_mid)
            section[Low].Del(val, box, area);
        if (box.PntMax()[X - 1] >= area_mid)
            section[High].Del(val, box, area);
    }

    /**
     * @brief Recursively collects all values from all child sections.
     * 
     * @param values Output map to accumulate value->box pairs
     */
    void GatherAllValuesDeep(std::unordered_map<TValue, Box_t>& values)
    {
        section[Low].GatherAllValuesDeep(values);
        section[High].GatherAllValuesDeep(values);
    }

    /**
     * @brief Finds all intersecting value pairs within this section.
     * 
     * Recursively queries child sections and accumulates intersecting pairs.
     * 
     * @param inter Output vector to store pairs of intersecting values
     */
    void FindIntersected(std::vector<std::array<TValue, 2>>& inter) const
    {
        section[Low].FindIntersected(inter);
        section[High].FindIntersected(inter);
    }

    /**
     * @brief Finds all values intersecting with a given box.
     * 
     * @param box The query box to test intersections against
     * @param inter Output set to accumulate intersecting values
     */
    void FindIntersected(const Box_t& box, std::unordered_set<TValue>& inter) const
    {
        section[Low].FindIntersected(box, inter);
        section[High].FindIntersected(box, inter);
    }

    /**
     * @brief Performs a deep traversal of the tree structure.
     * 
     * Calls callback functions for each tree level and each value encountered.
     * 
     * @param on_level_cb Callback invoked for each tree node/area
     * @param on_value_cb Callback invoked for each value in the tree
     */
    void TraverseDeep(std::function<void(const Box_t& area, uint lvl)> on_level_cb,
                      std::function<void(const Box_t& box, const TValue& val, uint lvl)> on_value_cb)
    {
        section[Low].TraverseDeep(on_level_cb, on_value_cb);
        section[High].TraverseDeep(on_level_cb, on_value_cb);
    }

  private:
    /// Array of two child sections (Low and High) for the next dimension (X-1)
    std::array<BiSection<TValue, TNode, TCoord, DIM, X - 1>, 2> section;
};

/**
 * @brief Terminal specialization of BiSection when X=1.
 * 
 * This is the base case of the recursion where we stop creating BiSection
 * objects and instead create TNode leaf nodes.
 * 
 * @tparam TValue The type of value stored in the tree
 * @tparam TNode The type of leaf node (will be Node template instantiation)
 * @tparam TCoord Coordinate type (must be floating-point)
 * @tparam DIM Dimensionality of the space (must be > 0)
 */
template <typename TValue, typename TNode, typename TCoord, std::size_t DIM>
    requires std::floating_point<TCoord> && (DIM > 0)
class BiSection<TValue, TNode, TCoord, DIM, 1>
{
  public:
    /// Type alias for a point in D-dimensional space
    using Point_t = Point<TCoord, DIM>;
    /// Type alias for an axis-aligned bounding box
    using Box_t = Box<TCoord, DIM>;

    /**
     * @brief Constructs the terminal bisection by creating two leaf nodes.
     * 
     * Divides the area along the first dimension (dimension 0) and creates
     * two TNode objects as children.
     * 
     * @param area The bounding box to bisect
     * @param level Current tree depth level (passed to child nodes)
     */
    BiSection(const Box_t& area, const uint level)
        : section({TNode(Box_t(area.PntMin(), area.PntMax().MidTo(area.PntMin(), 0)), level),
                   TNode(Box_t(area.PntMin().MidTo(area.PntMax(), 0), area.PntMax()), level)})
    {}

    /**
     * @brief Adds a value to the appropriate leaf node(s).
     * 
     * @param val The value to add
     * @param box Bounding box associated with the value
     * @param area The current area being bisected (for midpoint calculation)
     */
    void Add(TValue val, const Box_t& box, const Box_t& area)
    {
        const auto area_mid = (area.PntMax()[0] + area.PntMin()[0]) / 2;
        if (box.PntMax()[0] >= area_mid)
            section[High].Add(val, box);
        if (box.PntMin()[0] <= area_mid)
            section[Low].Add(val, box);
    }

    /**
     * @brief Removes a value from the appropriate leaf node(s).
     * 
     * @param val The value to remove
     * @param box Bounding box associated with the value
     * @param area The current area being bisected (for midpoint calculation)
     */
    void Del(TValue val, const Box_t& box, const Box_t& area)
    {
        const auto area_mid = (area.PntMax()[0] + area.PntMin()[0]) / 2;
        if (box.PntMin()[0] <= area_mid)
            section[Low].Del(val, box);
        if (box.PntMax()[0] >= area_mid)
            section[High].Del(val, box);
    }

    /**
     * @brief Recursively collects all values from all leaf nodes.
     * 
     * @param values Output map to accumulate value->box pairs
     */
    void GatherAllValuesDeep(std::unordered_map<TValue, Box_t>& values)
    {
        section[Low].GatherAllValuesDeep(values);
        section[High].GatherAllValuesDeep(values);
    }

    /**
     * @brief Finds all intersecting value pairs within this section.
     * 
     * @param inter Output vector to store pairs of intersecting values
     */
    void FindIntersected(std::vector<std::array<TValue, 2>>& inter) const
    {
        section[Low].FindIntersected(inter);
        section[High].FindIntersected(inter);
    }

    /**
     * @brief Finds all values intersecting with a given box.
     * 
     * @param box The query box to test intersections against
     * @param inter Output set to accumulate intersecting values
     */
    void FindIntersected(const Box_t& box, std::unordered_set<TValue>& inter) const
    {
        section[Low].FindIntersected(box, inter);
        section[High].FindIntersected(box, inter);
    }

    /**
     * @brief Performs a deep traversal of the leaf nodes.
     * 
     * @param on_level_cb Callback invoked for each tree node/area
     * @param on_value_cb Callback invoked for each value in the tree
     */
    void TraverseDeep(std::function<void(const Box_t& area, uint lvl)> on_level_cb,
                      std::function<void(const Box_t& box, const TValue& val, uint lvl)> on_value_cb)
    {
        section[Low].TraverseDeep(on_level_cb, on_value_cb);
        section[High].TraverseDeep(on_level_cb, on_value_cb);
    }

  private:
    /// Array of two leaf nodes (Low and High) for the final dimension
    std::array<TNode, 2> section;
};

/**
 * @brief Main node class for the orthtree (spatial tree structure).
 * 
 * Implements an adaptive tree that can subdivide space when the number of
 * elements exceeds a threshold. Can operate in two modes:
 * 1. NODES_SHARE_VAL = false: Strict containment (boxes must fit entirely in one orthant)
 * 2. NODES_SHARE_VAL = true: Boxes can span multiple orthants (shared between nodes)
 * 
 * @tparam TValue The type of value stored in the tree
 * @tparam TCoord Coordinate type (must be floating-point)
 * @tparam DIM Dimensionality of the space (must be > 0)
 * @tparam GROUP_COUNT Maximum number of elements in a node before subdivision
 * @tparam NODES_SHARE_VAL Whether boxes can be shared between nodes
 */
template <typename TValue, typename TCoord = float, std::size_t DIM = 2, std::size_t GROUP_COUNT = 10,
          bool NODES_SHARE_VAL = false>
    requires std::floating_point<TCoord> && (DIM > 0) && (GROUP_COUNT > 0)
class Node
{
  public:
    /// Type alias for an axis-aligned bounding box
    using Box_t = Box<TCoord, DIM>;
    /// Type alias for this node type (for self-reference)
    using Node_t = Node<TValue, TCoord, DIM, GROUP_COUNT>;
    /// Type alias for the bisection structure used for subdivision
    using BiSection_t = BiSection<TValue, Node_t, TCoord, DIM, DIM>;

    /**
     * @brief Constructs a node with the given spatial area and tree level.
     * 
     * @param area The spatial area covered by this node
     * @param level Depth level in the tree (0 for root)
     */
    Node(const Box_t& area, const uint level) : m_area(area), m_level(level) {}

    /**
     * @brief Clears all values and destroys any sub-nodes.
     */
    void Clear()
    {
        m_bucket.clear();
        m_sub_nodes = nullptr;
    }

    /**
     * @brief Adds a value with its associated bounding box to the tree.
     * 
     * The value is either stored in this node's bucket or recursively added
     * to sub-nodes if subdivision exists or should be created.
     * 
     * @param val The value to add
     * @param box Bounding box associated with the value
     * 
     * @throw Assertion failure if the box is outside the node's area
     *        (depending on NODES_SHARE_VAL mode)
     */
    void Add(TValue val, const Box_t& box)
    {
        // Validate box is within node area based on mode
        if (NODES_SHARE_VAL)
        {
            ORTHTREE_DEBUG_ASSERT(m_area.Intersect(box), "Out of area");
        }
        else
        {
            ORTHTREE_DEBUG_ASSERT(m_area.ContainStrict(box), "Out of area");
        }

        // Check if we should store in this node or subdivide
        if ((!CanFallDeeper(box)) or ((m_bucket.size() < GROUP_COUNT) and (m_sub_nodes == nullptr)))
        {
            // Store in this node's bucket
            m_bucket.emplace(val, box);
        }
        else
        {
            // Need to subdivide
            if (m_sub_nodes == nullptr)
            {
                // Create subdivision and re-distribute existing values
                m_sub_nodes = std::make_unique<BiSection_t>(m_area, m_level + 1);
                std::unordered_map<TValue, Box_t> new_bucket;
                for (const auto& v_it : m_bucket)
                {
                    if (CanFallDeeper(v_it.second))
                    {
                        // Move to sub-nodes
                        m_sub_nodes->Add(v_it.first, v_it.second, m_area);
                    }
                    else
                    {
                        // Keep in this node
                        new_bucket.insert(v_it);
                    }
                }
                m_bucket = std::move(new_bucket);
            }
            // Add new value to sub-nodes
            m_sub_nodes->Add(val, box, m_area);
        }
        ++m_values_count;
    }

    /**
     * @brief Removes a value from the tree.
     * 
     * If removal causes the total count to drop below GROUP_COUNT,
     * the tree collapses sub-nodes back into this node.
     * 
     * @param val The value to remove
     * @param box Bounding box associated with the value (for locating it)
     * 
     * @throw Assertion failure if value not found or bucket size invalid
     */
    void Del(TValue val, const Box_t& box)
    {
        --m_values_count;
        if (m_values_count <= GROUP_COUNT)
        {
            // Collapse sub-nodes if they exist
            if (m_sub_nodes != nullptr)
            {
                m_sub_nodes->GatherAllValuesDeep(m_bucket);
                m_sub_nodes = nullptr;
            }
            m_bucket.erase(val);
            ORTHTREE_DEBUG_ASSERT(m_bucket.size() <= GROUP_COUNT, "Too many elements in bucket");
        }
        else
        {
            // Remove from bucket or sub-nodes
            if (m_bucket.erase(val) == 0)
            {
                ORTHTREE_DEBUG_ASSERT(m_sub_nodes != nullptr, "No value in node");
                m_sub_nodes->Del(val, box, m_area);
            }
        }
    }

    /**
     * @brief Recursively collects all values from this node and all sub-nodes.
     * 
     * @param values Output map to accumulate value->box pairs
     */
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

    /**
     * @brief Finds all pairs of intersecting values within this node's subtree.
     * 
     * Performs pairwise intersection tests between:
     * 1. Values in this node's bucket
     * 2. Values in this node's bucket vs values in sub-nodes
     * 3. Values within sub-nodes (recursively)
     * 
     * @param inter Output vector to store pairs of intersecting values
     */
    void FindIntersected(std::vector<std::array<TValue, 2>>& inter) const
    {
        // Check intersections within this node's bucket
        auto iter1 = m_bucket.begin();
        while (iter1 != m_bucket.end())
        {
            auto iter2       = iter1;
            const auto& box1 = iter1->second;
            ++iter2;

            // Pairwise tests within bucket
            while (iter2 != m_bucket.end())
            {
                const auto& box2 = iter2->second;
                if (box1.Intersect(box2))
                {
                    inter.push_back({iter1->first, iter2->first});
                }
                ++iter2;
            }

            // Check intersections between bucket value and sub-node values
            if (m_sub_nodes != nullptr)
            {
                std::unordered_set<TValue> inter_box1;
                m_sub_nodes->FindIntersected(box1, inter_box1);
                for (const auto& val : inter_box1)
                {
                    inter.push_back({iter1->first, val});
                }
            }
            ++iter1;
        }

        // Recursively check intersections within sub-nodes
        if (m_sub_nodes != nullptr)
        {
            m_sub_nodes->FindIntersected(inter);
        }
    }

    /**
     * @brief Finds all values intersecting with a given query box.
     * 
     * @param box The query box to test intersections against
     * @param inter Output set to accumulate intersecting values
     */
    void FindIntersected(const Box_t& box, std::unordered_set<TValue>& inter) const
    {
        // Check intersections with bucket values
        for (const auto& v_it : m_bucket)
        {
            if (box.Intersect(v_it.second))
            {
                inter.insert(v_it.first);
            }
        }

        // Recursively check sub-nodes
        if (m_sub_nodes != nullptr)
        {
            m_sub_nodes->FindIntersected(box, inter);
        }
    }

    /**
     * @brief Performs a deep traversal of the tree.
     * 
     * Invokes callbacks for each node area and each value encountered.
     * 
     * @param on_level_cb Callback invoked for each tree node/area
     * @param on_value_cb Callback invoked for each value in the tree
     */
    void TraverseDeep(std::function<void(const Box_t& area, uint lvl)> on_level_cb,
                      std::function<void(const Box_t& box, const TValue& val, uint lvl)> on_value_cb)
    {
        on_level_cb(m_area, m_level);
        for (const auto& val_it : m_bucket)
            on_value_cb(val_it.second, val_it.first, m_level);

        if (m_sub_nodes != nullptr)
        {
            m_sub_nodes->TraverseDeep(on_level_cb, on_value_cb);
        }
    }

    /**
     * @brief Returns the spatial area covered by this node.
     * 
     * @return const Box_t& Reference to the node's bounding box
     */
    const Box_t& area() const { return m_area; }

  private:
    /**
     * @brief Checks if a box fits entirely within one orthant of this node.
     * 
     * A box fits if for every dimension, both min and max coordinates
     * are on the same side of the midpoint.
     * 
     * @param box The box to test
     * @return true if box fits in one orthant, false if it straddles a midpoint
     */
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

    /**
     * @brief Determines if a box can be pushed deeper into the tree.
     * 
     * In NODES_SHARE_VAL mode, a box can fall deeper if it doesn't completely
     * contain an orthant of this node. Otherwise, it can fall deeper if it
     * fits entirely within one orthant.
     * 
     * @param box The box to test
     * @return true if box should be stored in sub-nodes, false otherwise
     */
    bool CanFallDeeper(const Box_t& box) const
    {
        return NODES_SHARE_VAL ? !box.ContainOrthant(m_area) : m_area.ContainInOrthant(box);
    }

    const Box_t m_area;  ///< Spatial area covered by this node
    const uint m_level;  ///< Depth level in tree (0 for root)

    /// Bucket storing values when not subdivided (value -> bounding box)
    std::unordered_map<TValue, Box_t> m_bucket;

    /// Total count of values in this node and all sub-nodes
    std::size_t m_values_count = 0;

    /// Pointer to bisection structure when node is subdivided
    std::unique_ptr<BiSection_t> m_sub_nodes;
};

}  // namespace orthtree