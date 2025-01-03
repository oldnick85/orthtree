// Copyright 2024 oldnick85

#include <gtest/gtest.h>

#include "./orthtree.h"

#include "../../common/test_common.h"

using namespace orthtree;

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
        printf("%sLevel %u: %s\n", std::string(lvl * 2, ' ').c_str(), lvl, area.Str().c_str());
    };
    std::function<void(const Box_t& box, const int& val, uint lvl)> on_value_cb = [](const Box_t& box, const int& val,
                                                                                     uint lvl) {
        printf("%sValue %u: %s\n", std::string(lvl * 2, ' ').c_str(), val, box.Str().c_str());
    };
    tree.TraverseDeep(on_level_cb, on_value_cb);
}

TEST(Tree, IntersectBoxRandom2D)
{
    constexpr std::size_t COUNT = 1000;
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
    constexpr std::size_t COUNT = 1000;
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
    constexpr std::size_t COUNT = 300;
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
    constexpr std::size_t COUNT = 300;
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
    constexpr std::size_t ROUNDS = 100;
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
    constexpr std::size_t ROUNDS = 100;
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
// NOLINTEND