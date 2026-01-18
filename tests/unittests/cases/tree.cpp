// Copyright 2024 oldnick85

#include <gtest/gtest.h>

#include <print>
#include <unordered_set>

#include "./orthtree.h"

#include "../../common/test_common.h"

using orthtree::Tree;

constexpr std::size_t SCALE = 10;

// NOLINTBEGIN
TEST(Tree, Base)
{
    using Tree_t = Tree<int, float, 2, 2>;
    using Box_t  = Tree_t::Box_t;
    Tree_t tree{Box_t{{0.0, 0.0}, {8.0, 8.0}}};

    tree.Add(1, Box_t{{1.0, 1.0}});
    tree.Add(2, Box_t{{1.0, 3.0}});
    tree.Add(3, Box_t{{3.0, 3.0}});

    tree.Add(4, Box_t{{5.0, 1.0}});
    tree.Add(5, Box_t{{5.0, 3.0}});
    tree.Add(6, Box_t{{7.0, 3.0}});

    std::function<void(const Box_t& area, uint lvl)> on_level_cb = [](const Box_t& area, uint lvl) {
        std::println("{}Level {}: {}", std::string(lvl * 2, ' '), lvl, area.Str());
    };
    std::function<void(const Box_t& box, const int& val, uint lvl)> on_value_cb = [](const Box_t& box, const int& val,
                                                                                     uint lvl) {
        std::println("{}Value {}: {}", std::string(lvl * 2, ' '), val, box.Str());
    };
    tree.TraverseDeep(on_level_cb, on_value_cb);
}

TEST(Tree, IntersectBoxRandom2D)
{
    constexpr std::size_t COUNT = 10 * SCALE;
    using Tree_t                = Tree<std::size_t, float, 2, 20>;
    using Box_t                 = Tree_t::Box_t;
    using Point_t               = Box_t::Point_t;
    Tree_t tree{Box_t{{0.0, 0.0}, {10.0, 10.0}}};

    GenerateRandomBoxes<Tree_t, Box_t, Point_t, float, 2>(COUNT, tree, 0.0, 10.0, 0.0, 10.0);
    auto boxes = tree.GetAllValues();

    for (std::size_t i = 0; i < COUNT; ++i)
    {
        const auto tree_inters = tree.FindIntersected(i);
        std::unordered_set<std::size_t> brute_inters;
        for (std::size_t box_i = 0; box_i < COUNT; ++box_i)
        {
            if (i == box_i)
                continue;
            const auto& box = boxes[box_i];
            if (boxes[i].Intersect(box))
                brute_inters.insert(box_i);
        }
        ASSERT_EQ(tree_inters, brute_inters);
    }
}

TEST(Tree, IntersectBoxRandom3D)
{
    constexpr std::size_t COUNT = 10 * SCALE;
    constexpr float area_size   = 10.0;
    using Tree_t                = Tree<std::size_t, float, 3, 20>;
    using Box_t                 = Tree_t::Box_t;
    using Point_t               = Box_t::Point_t;

    Tree_t tree{Box_t{{0.0, 0.0, 0.0}, {area_size, area_size, area_size}}};

    GenerateRandomBoxes<Tree_t, Box_t, Point_t, float, 3>(COUNT, tree, 0.0, area_size, 0.0, area_size);
    auto boxes = tree.GetAllValues();

    for (std::size_t i = 0; i < COUNT; ++i)
    {
        const auto tree_inters = tree.FindIntersected(i);
        std::unordered_set<std::size_t> brute_inters;
        for (std::size_t box_i = 0; box_i < COUNT; ++box_i)
        {
            if (i == box_i)
                continue;
            const auto& box = boxes[box_i];
            if (boxes[i].Intersect(box))
                brute_inters.insert(box_i);
        }
        ASSERT_EQ(tree_inters, brute_inters);
    }
}

TEST(Tree, IntersectPairsRandom2D)
{
    constexpr std::size_t COUNT = 3 * SCALE;
    using Tree_t                = Tree<std::size_t, float, 2, 20>;
    using Box_t                 = Tree_t::Box_t;
    using Point_t               = Box_t::Point_t;
    Tree_t tree{Box_t{{0.0, 0.0}, {10.0, 10.0}}};

    GenerateRandomBoxes<Tree_t, Box_t, Point_t, float, 2>(COUNT, tree, 0.0, 10.0, 0.0, 10.0);

    const auto tree_inters                               = tree.FindIntersected();
    std::vector<std::array<std::size_t, 2>> brute_inters = IntersectPairsBrute(tree.GetAllValues());

    ASSERT_EQ(tree_inters.size(), brute_inters.size());

    for (const auto& tree_pair : tree_inters)
    {
        bool found =
            (std::any_of(brute_inters.cbegin(), brute_inters.cend(), [tree_pair](const std::array<std::size_t, 2>& p) {
                return (((tree_pair[0] == p[0]) and (tree_pair[1] == p[1])) or
                        ((tree_pair[0] == p[1]) and (tree_pair[1] == p[0])));
            }));
        ASSERT_TRUE(found);
    }

    for (const auto& brute_pair : brute_inters)
    {
        bool found =
            (std::any_of(tree_inters.cbegin(), tree_inters.cend(), [brute_pair](const std::array<std::size_t, 2>& p) {
                return (((brute_pair[0] == p[0]) and (brute_pair[1] == p[1])) or
                        ((brute_pair[0] == p[1]) and (brute_pair[1] == p[0])));
            }));
        ASSERT_TRUE(found);
    }
}

TEST(Tree, IntersectPairsRandom3D)
{
    constexpr std::size_t COUNT = 3 * SCALE;
    using Tree_t                = Tree<std::size_t, float, 3, 20>;
    using Box_t                 = Tree_t::Box_t;
    using Point_t               = Box_t::Point_t;
    Tree_t tree{Box_t{{0.0, 0.0, 0.0}, {10.0, 10.0, 10.0}}};

    GenerateRandomBoxes<Tree_t, Box_t, Point_t, float, 3>(COUNT, tree, 0.0, 10.0, 0.0, 10.0);

    const auto tree_inters                               = tree.FindIntersected();
    std::vector<std::array<std::size_t, 2>> brute_inters = IntersectPairsBrute(tree.GetAllValues());

    ASSERT_EQ(tree_inters.size(), brute_inters.size());

    for (const auto& tree_pair : tree_inters)
    {
        bool found =
            (std::any_of(brute_inters.cbegin(), brute_inters.cend(), [tree_pair](const std::array<std::size_t, 2>& p) {
                return (((tree_pair[0] == p[0]) and (tree_pair[1] == p[1])) or
                        ((tree_pair[0] == p[1]) and (tree_pair[1] == p[0])));
            }));
        ASSERT_TRUE(found);
    }

    for (const auto& brute_pair : brute_inters)
    {
        bool found =
            (std::any_of(tree_inters.cbegin(), tree_inters.cend(), [brute_pair](const std::array<std::size_t, 2>& p) {
                return (((brute_pair[0] == p[0]) and (brute_pair[1] == p[1])) or
                        ((brute_pair[0] == p[1]) and (brute_pair[1] == p[0])));
            }));
        ASSERT_TRUE(found);
    }
}

TEST(Tree, AddDelIntersectPairsRandom2D)
{
    constexpr std::size_t ROUNDS = SCALE;
    constexpr std::size_t COUNT  = 100;
    using Tree_t                 = Tree<std::size_t, float, 2, 20>;
    using Box_t                  = Tree_t::Box_t;
    using Point_t                = Box_t::Point_t;
    Tree_t tree{Box_t{{0.0, 0.0}, {10.0, 10.0}}};

    GenerateRandomBoxes<Tree_t, Box_t, Point_t, float, 2>(COUNT, tree, 0.0, 10.0, 0.0, 10.0);

    std::size_t id = COUNT;
    for (std::size_t r_i = 0; r_i < ROUNDS; ++r_i)
    {
        {
            const auto del_val = tree.GetAllValues().begin()->first;
            tree.Del(del_val);
            ++id;
            AddRandomBox<Tree_t, Box_t, Point_t, float, 2>(id, tree, 0.0, 10.0, 0.0, 10.0);
        }

        const auto tree_inters                               = tree.FindIntersected();
        std::vector<std::array<std::size_t, 2>> brute_inters = IntersectPairsBrute(tree.GetAllValues());

        ASSERT_EQ(tree_inters.size(), brute_inters.size());

        for (const auto& tree_pair : tree_inters)
        {
            bool found = (std::any_of(brute_inters.cbegin(), brute_inters.cend(),
                                      [tree_pair](const std::array<std::size_t, 2>& p) {
                                          return (((tree_pair[0] == p[0]) and (tree_pair[1] == p[1])) or
                                                  ((tree_pair[0] == p[1]) and (tree_pair[1] == p[0])));
                                      }));
            ASSERT_TRUE(found);
        }

        for (const auto& brute_pair : brute_inters)
        {
            bool found = (std::any_of(tree_inters.cbegin(), tree_inters.cend(),
                                      [brute_pair](const std::array<std::size_t, 2>& p) {
                                          return (((brute_pair[0] == p[0]) and (brute_pair[1] == p[1])) or
                                                  ((brute_pair[0] == p[1]) and (brute_pair[1] == p[0])));
                                      }));
            ASSERT_TRUE(found);
        }
    }
}

TEST(Tree, AddDelIntersectPairsRandom3D)
{
    constexpr std::size_t ROUNDS = SCALE;
    constexpr std::size_t COUNT  = 100;
    using Tree_t                 = Tree<std::size_t, float, 3, 20>;
    using Box_t                  = Tree_t::Box_t;
    using Point_t                = Box_t::Point_t;
    Tree_t tree{Box_t{{0.0, 0.0, 0.0}, {10.0, 10.0, 10.0}}};

    GenerateRandomBoxes<Tree_t, Box_t, Point_t, float, 3>(COUNT, tree, 0.0, 10.0, 0.0, 10.0);

    std::size_t id = COUNT;
    for (std::size_t r_i = 0; r_i < ROUNDS; ++r_i)
    {
        {
            const auto del_val = tree.GetAllValues().begin()->first;
            tree.Del(del_val);
            ++id;
            AddRandomBox<Tree_t, Box_t, Point_t, float, 3>(id, tree, 0.0, 10.0, 0.0, 10.0);
        }

        const auto tree_inters                               = tree.FindIntersected();
        std::vector<std::array<std::size_t, 2>> brute_inters = IntersectPairsBrute(tree.GetAllValues());

        ASSERT_EQ(tree_inters.size(), brute_inters.size());

        for (const auto& tree_pair : tree_inters)
        {
            bool found = (std::any_of(brute_inters.cbegin(), brute_inters.cend(),
                                      [tree_pair](const std::array<std::size_t, 2>& p) {
                                          return (((tree_pair[0] == p[0]) and (tree_pair[1] == p[1])) or
                                                  ((tree_pair[0] == p[1]) and (tree_pair[1] == p[0])));
                                      }));
            ASSERT_TRUE(found);
        }

        for (const auto& brute_pair : brute_inters)
        {
            bool found = (std::any_of(tree_inters.cbegin(), tree_inters.cend(),
                                      [brute_pair](const std::array<std::size_t, 2>& p) {
                                          return (((brute_pair[0] == p[0]) and (brute_pair[1] == p[1])) or
                                                  ((brute_pair[0] == p[1]) and (brute_pair[1] == p[0])));
                                      }));
            ASSERT_TRUE(found);
        }
    }
}

/**
 * @brief Test basic tree construction and single value operations
 * 
 * @details Verifies fundamental operations: construction, addition, 
 *          containment checks, and bounding box retrieval.
 */
TEST(Tree, BasicOperationsAndValidation)
{
    using Tree_t = Tree<int, float, 2, 2>;
    using Box_t  = Tree_t::Box_t;

    // Test 1: Tree construction with valid area
    Tree_t tree{Box_t{{0.0, 0.0}, {8.0, 8.0}}};
    EXPECT_EQ(tree.area(), (Box_t{{0.0, 0.0}, {8.0, 8.0}}));

    // Test 2: Add single value and verify it exists
    const int value_id = 42;
    const Box_t value_box{{1.0, 1.0}, {2.0, 2.0}};

    tree.Add(value_id, value_box);
    EXPECT_TRUE(tree.Contains(value_id));
    EXPECT_FALSE(tree.Contains(99));  // Non-existent value

    // Test 3: Retrieve bounding box for added value
    EXPECT_EQ(tree.GetBox(value_id), value_box);

    // Test 4: Remove value and verify it's gone
    tree.Del(value_id);
    EXPECT_FALSE(tree.Contains(value_id));
    EXPECT_TRUE(tree.GetAllValues().empty());
}

/**
 * @brief Test boundary condition handling for zero-volume boxes
 * 
 * @details Tests edge cases with degenerate (zero-size) boxes and
 *          boxes that touch domain boundaries.
 */
TEST(Tree, ZeroVolumeAndBoundaryBoxes)
{
    using Tree_t = Tree<int, float, 2, 5>;
    using Box_t  = Tree_t::Box_t;

    Tree_t tree{Box_t{{-0.1, -0.1}, {100.1, 100.1}}};

    // Test 1: Single point box (zero volume)
    tree.Add(1, Box_t{{10.0, 10.0}, {10.0, 10.0}});
    EXPECT_TRUE(tree.Contains(1));

    // Test 2: Box with zero width but positive height
    tree.Add(2, Box_t{{20.0, 20.0}, {20.0, 30.0}});
    EXPECT_TRUE(tree.Contains(2));

    // Test 3: Box with zero height but positive width
    tree.Add(3, Box_t{{30.0, 30.0}, {40.0, 30.0}});
    EXPECT_TRUE(tree.Contains(3));

    // Test 4: Large box
    tree.Add(4, Box_t{{0.0, 0.0}, {100.0, 100.0}});
    EXPECT_TRUE(tree.Contains(3));

    // Verify all boxes can be retrieved
    const auto& all_values = tree.GetAllValues();
    EXPECT_EQ(all_values.size(), 4);

    // Test intersections with boundary boxes
    const auto intersections = tree.FindIntersected();
    // At minimum, box 4 (full area) should intersect with everything
    EXPECT_GE(intersections.size(), 3);
}

/**
 * @brief Test tree subdivision behavior with GROUP_COUNT = 1
 * 
 * @details Tests extreme case where each node splits immediately
 *          after getting a single value. Validates proper tree
 *          structure maintenance.
 */
TEST(Tree, ImmediateSubdivision)
{
    using Tree_t = Tree<int, float, 2, 1>;  // GROUP_COUNT = 1 forces immediate split
    using Box_t  = Tree_t::Box_t;

    Tree_t tree{Box_t{{0.0, 0.0}, {8.0, 8.0}}};

    // Add values that will force subdivision
    tree.Add(1, Box_t{{1.0, 1.0}, {2.0, 2.0}});
    tree.Add(2, Box_t{{5.0, 5.0}, {6.0, 6.0}});
    tree.Add(3, Box_t{{1.0, 5.0}, {2.0, 6.0}});
    tree.Add(4, Box_t{{5.0, 1.0}, {6.0, 2.0}});

    // All values should be present
    EXPECT_EQ(tree.GetAllValues().size(), 4);

    // Test intersection finding
    const auto intersections = tree.FindIntersected();

    // In this configuration, no boxes should intersect (they're in different quadrants)
    EXPECT_TRUE(intersections.empty());

    // Add overlapping boxes to create intersections
    tree.Add(5, Box_t{{1.5, 1.5}, {2.5, 2.5}});  // Overlaps with value 1
    tree.Add(6, Box_t{{5.5, 5.5}, {6.5, 6.5}});  // Overlaps with value 2

    const auto new_intersections = tree.FindIntersected();
    EXPECT_GE(new_intersections.size(), 2);  // At least (1,5) and (2,6)
}

/**
 * @brief Test with different floating-point precision types
 * 
 * @details Ensures tree works correctly with both float and double
 *          coordinate types, checking precision-related behaviors.
 */
TEST(Tree, FloatingPointPrecision)
{
    // Test with float precision
    {
        using Tree_t = Tree<int, float, 2, 5>;
        using Box_t  = Tree_t::Box_t;

        Tree_t tree{Box_t{{0.0f, 0.0f}, {1.0f, 1.0f}}};

        // Add values with coordinates at precision boundaries
        tree.Add(1, Box_t{{0.1f, 0.1f}, {0.2f, 0.2f}});
        tree.Add(2, Box_t{{0.1000001f, 0.1000001f}, {0.2000001f, 0.2000001f}});

        // Values should be distinct despite float precision
        EXPECT_TRUE(tree.Contains(1));
        EXPECT_TRUE(tree.Contains(2));
        EXPECT_NE(tree.GetBox(1), tree.GetBox(2));
    }

    // Test with double precision
    {
        using Tree_t = Tree<int, double, 2, 5>;
        using Box_t  = Tree_t::Box_t;

        Tree_t tree{Box_t{{0.0, 0.0}, {1.0, 1.0}}};

        // Add values with high precision differences
        tree.Add(1, Box_t{{0.1, 0.1}, {0.2, 0.2}});
        tree.Add(2, Box_t{{0.1000000001, 0.1000000001}, {0.2000000001, 0.2000000001}});

        // Values should be distinct
        EXPECT_TRUE(tree.Contains(1));
        EXPECT_TRUE(tree.Contains(2));
        EXPECT_NE(tree.GetBox(1), tree.GetBox(2));
    }
}

/**
 * @brief Test Clear() functionality and tree reusability
 * 
 * @details Verifies that Clear() properly resets the tree and
 *          that the tree can be reused after clearing.
 */
TEST(Tree, ClearAndReuse)
{
    using Tree_t = Tree<int, float, 2, 3>;
    using Box_t  = Tree_t::Box_t;

    Tree_t tree{Box_t{{-0.1, -0.1}, {10.1, 10.1}}};

    // Populate tree with values
    for (int i = 0; i < 10; ++i)
    {
        tree.Add(i, Box_t{{float(i), float(i)}, {float(i + 1), float(i + 1)}});
    }

    EXPECT_FALSE(tree.GetAllValues().empty());
    EXPECT_EQ(tree.GetAllValues().size(), 10);

    // Clear tree and verify emptiness
    tree.Clear();

    EXPECT_TRUE(tree.GetAllValues().empty());
    EXPECT_EQ(tree.FindIntersected().size(), 0);

    // Reuse tree after clearing
    tree.Add(100, Box_t{{1.0, 1.0}, {2.0, 2.0}});
    tree.Add(101, Box_t{{1.5, 1.5}, {2.5, 2.5}});

    EXPECT_EQ(tree.GetAllValues().size(), 2);

    // Should find intersection between overlapping boxes
    const auto intersections = tree.FindIntersected();
    EXPECT_EQ(intersections.size(), 1);
}

/**
 * @brief Test tree with 1D dimension (binary tree)
 * 
 * @details Validates tree functionality in 1-dimensional space,
 *          which is essentially a binary tree for intervals.
 */
TEST(Tree, OneDimensionalTree)
{
    using Tree_t = Tree<int, float, 1, 2>;  // 1D tree
    using Box_t  = Tree_t::Box_t;

    Tree_t tree{Box_t{{0.0}, {100.0}}};

    // Add 1D intervals
    tree.Add(1, Box_t{{10.0}, {20.0}});
    tree.Add(2, Box_t{{15.0}, {25.0}});  // Overlaps with 1
    tree.Add(3, Box_t{{50.0}, {60.0}});  // Disjoint

    EXPECT_EQ(tree.GetAllValues().size(), 3);

    // Test intersection finding
    const auto intersections = tree.FindIntersected();

    // Should find exactly one intersection: (1, 2)
    EXPECT_EQ(intersections.size(), 1);
    if (!intersections.empty())
    {
        const auto& pair = intersections[0];
        EXPECT_TRUE((pair[0] == 1 && pair[1] == 2) || (pair[0] == 2 && pair[1] == 1));
    }

    // Test query for specific value intersections
    const auto intersections_with_1 = tree.FindIntersected(1);
    EXPECT_EQ(intersections_with_1.size(), 1);
    EXPECT_TRUE(intersections_with_1.contains(2));
}

/**
 * @brief Test large GROUP_COUNT values preventing subdivision
 * 
 * @details Tests behavior when GROUP_COUNT is large enough that
 *          no subdivision occurs, essentially making the tree
 *          a single node storing all values.
 */
TEST(Tree, NoSubdivisionBehavior)
{
    using Tree_t = Tree<int, float, 2, 1000>;  // Very large GROUP_COUNT
    using Box_t  = Tree_t::Box_t;

    Tree_t tree{Box_t{{-0.1, -0.1}, {105.1, 105.1}}};

    // Add many values - tree should not subdivide
    for (int i = 0; i < 500; ++i)
    {
        const float x = static_cast<float>(i % 10) * 10.0f;
        const float y = static_cast<float>(i / 100) * 10.0f;
        tree.Add(i, Box_t{{x, y}, {x + 5.0f, y + 5.0f}});
    }

    EXPECT_EQ(tree.GetAllValues().size(), 500);

    // Find intersections - should work correctly even without subdivision
    const auto intersections = tree.FindIntersected();

    // Should find some intersections (many boxes overlap in this configuration)
    EXPECT_GT(intersections.size(), 0);

    // Verify intersections are correct via brute force
    const auto& all_values = tree.GetAllValues();
    std::vector<std::array<int, 2>> brute_intersections;

    for (const auto& [val1, box1] : all_values)
    {
        for (const auto& [val2, box2] : all_values)
        {
            if (val1 >= val2)
                continue;  // Avoid duplicates and self
            if (box1.Intersect(box2))
            {
                brute_intersections.push_back({val1, val2});
            }
        }
    }

    EXPECT_EQ(intersections.size(), brute_intersections.size());
}

/**
 * @brief Test value update (Change method) functionality
 * 
 * @details Validates that updating a value's bounding box
 *          correctly removes old associations and establishes
 *          new ones based on the updated position.
 */
TEST(Tree, ValueUpdateAndMovement)
{
    using Tree_t = Tree<int, float, 2, 3>;
    using Box_t  = Tree_t::Box_t;

    Tree_t tree{Box_t{{0.0, 0.0}, {100.0, 100.0}}};

    // Add initial values
    tree.Add(1, Box_t{{10.0, 10.0}, {20.0, 20.0}});
    tree.Add(2, Box_t{{15.0, 15.0}, {25.0, 25.0}});  // Overlaps with 1
    tree.Add(3, Box_t{{50.0, 50.0}, {60.0, 60.0}});  // Disjoint

    // Verify initial intersection
    auto intersections = tree.FindIntersected();
    EXPECT_EQ(intersections.size(), 1);  // (1, 2)

    // Move value 2 away from value 1
    tree.Change(2, Box_t{{70.0, 70.0}, {80.0, 80.0}});

    // Verify no more intersection between 1 and 2
    intersections = tree.FindIntersected();
    EXPECT_TRUE(intersections.empty());

    // Move value 2 to overlap with value 3
    tree.Change(2, Box_t{{55.0, 55.0}, {65.0, 65.0}});

    // Verify new intersection between 2 and 3
    intersections = tree.FindIntersected();
    EXPECT_EQ(intersections.size(), 1);  // (2, 3)

    // Verify bounding box was updated
    EXPECT_EQ(tree.GetBox(2), (Box_t{{55.0, 55.0}, {65.0, 65.0}}));
}

/**
 * @brief Stress test with many values and operations
 * 
 * @details Tests tree performance and correctness under load
 *          with mixed operations (add, remove, update, query).
 */
TEST(Tree, StressTestMixedOperations)
{
    using Tree_t = Tree<std::size_t, float, 2, 10>;
    using Box_t  = Tree_t::Box_t;

    Tree_t tree{Box_t{{-0.1, -0.1}, {1010.1, 1010.1}}};

    constexpr std::size_t NUM_OPERATIONS = 1000;
    constexpr std::size_t MAX_VALUES     = 200;

    std::size_t next_id = 0;

    for (std::size_t op = 0; op < NUM_OPERATIONS; ++op)
    {
        const auto operation = op % 10;

        if (operation < 7 && tree.GetAllValues().size() < MAX_VALUES)
        {
            // Add operation (70% chance when under limit)
            const float x = static_cast<float>(op % 50) * 20.0f;
            const float y = static_cast<float>((op / 50) % 20) * 20.0f;
            tree.Add(next_id, Box_t{{x, y}, {x + 10.0f, y + 10.0f}});
            ++next_id;
        }
        else if (operation < 9 && !tree.GetAllValues().empty())
        {
            // Remove operation (20% chance when not empty)
            const auto& values = tree.GetAllValues();
            if (!values.empty())
            {
                auto it = values.begin();
                std::advance(it, op % values.size());
                tree.Del(it->first);
            }
        }
        else if (!tree.GetAllValues().empty())
        {
            // Update operation (10% chance when not empty)
            const auto& values = tree.GetAllValues();
            if (!values.empty())
            {
                auto it = values.begin();
                std::advance(it, op % values.size());
                const auto new_x = static_cast<float>((op + 1) % 50) * 20.0f;
                const auto new_y = static_cast<float>(((op + 1) / 50) % 20) * 20.0f;
                tree.Change(it->first, Box_t{{new_x, new_y}, {new_x + 10.0f, new_y + 10.0f}});
            }
        }

        // Periodically validate state
        if (op % 100 == 0)
        {
            // Verify all stored values are reachable
            for (const auto& [val, box] : tree.GetAllValues())
            {
                EXPECT_TRUE(tree.Contains(val));
                EXPECT_EQ(tree.GetBox(val), box);
            }

            // Verify no duplicate values
            const auto& values = tree.GetAllValues();
            std::unordered_set<std::size_t> unique_values;
            for (const auto& [val, _] : values)
            {
                EXPECT_FALSE(unique_values.contains(val));
                unique_values.insert(val);
            }
        }
    }

    // Final validation: check all intersections are correct
    if (!tree.GetAllValues().empty())
    {
        const auto tree_intersections  = tree.FindIntersected();
        const auto brute_intersections = IntersectPairsBrute(tree.GetAllValues());

        EXPECT_EQ(tree_intersections.size(), brute_intersections.size());
    }
}

// NOLINTEND