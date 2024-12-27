// Copyright 2024 oldnick85
#include <random>

#include <gtest/gtest.h>

#include "./orthtree.h"

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

TEST(Tree, IntersectBoxRandom)
{
    constexpr std::size_t COUNT = 100;
    using Tree_t                = Tree<std::size_t, float, 2, 20>;
    using Box_t                 = Tree_t::Box_t;
    Tree_t tree{Box_t{{0.0, 0.0}, {10.0, 10.0}}};

    std::random_device rd;   // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<float> dis(0.0, 10.0);

    std::vector<Box_t> boxes;
    boxes.reserve(COUNT);
    for (std::size_t i = 0; i < COUNT; ++i)
        boxes.emplace_back(Box_t{{dis(gen), dis(gen)}, {dis(gen), dis(gen)}});

    for (std::size_t i = 0; i < COUNT; ++i)
    {
        const auto& box = boxes[i];
        tree.Add(i, box);
    }

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

template <typename Box_t>
std::vector<std::array<std::size_t, 2>> IntersectPairsBrute(std::vector<Box_t> boxes)
{
    std::vector<std::array<std::size_t, 2>> brute_inters;
    for (std::size_t i = 0; i < boxes.size() - 1; ++i)
        for (std::size_t j = i + 1; j < boxes.size(); ++j)
        {
            const auto& box1 = boxes[i];
            const auto& box2 = boxes[j];
            if (box1.Intersect(box2))
                brute_inters.push_back({i, j});
        }
    return brute_inters;
}

template <typename Box_t>
std::vector<std::array<std::size_t, 2>> IntersectPairsBrute(const std::unordered_map<std::size_t, Box_t>& boxes)
{
    std::vector<std::array<std::size_t, 2>> brute_inters;
    for (const auto& box1_it : boxes)
        for (const auto& box2_it : boxes)
        {
            if (box1_it.first <= box2_it.first)
                continue;
            const auto& box1 = box1_it.second;
            const auto& box2 = box2_it.second;
            if (box1.Intersect(box2))
                brute_inters.push_back({box1_it.first, box2_it.first});
        }
    return brute_inters;
}

TEST(Tree, IntersectPairsRandom)
{
    constexpr std::size_t COUNT = 1000;
    using Tree_t                = Tree<std::size_t, float, 2, 20>;
    using Box_t                 = Tree_t::Box_t;
    Tree_t tree{Box_t{{0.0, 0.0}, {10.0, 10.0}}};

    std::random_device rd;   // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<float> dis(0.0, 10.0);

    std::vector<Box_t> boxes;
    boxes.reserve(COUNT);
    for (std::size_t i = 0; i < COUNT; ++i)
        boxes.emplace_back(Box_t{{dis(gen), dis(gen)}, {dis(gen), dis(gen)}});

    for (std::size_t i = 0; i < COUNT; ++i)
    {
        const auto& box = boxes[i];
        tree.Add(i, box);
    }

    const auto tree_inters                               = tree.FindIntersected();
    std::vector<std::array<std::size_t, 2>> brute_inters = IntersectPairsBrute(boxes);

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

TEST(Tree, AddDelIntersectPairsRandom)
{
    constexpr std::size_t ROUNDS = 10000;
    constexpr std::size_t COUNT  = 100;
    using Tree_t                 = Tree<std::size_t, float, 2, 20>;
    using Box_t                  = Tree_t::Box_t;
    Tree_t tree{Box_t{{0.0, 0.0}, {10.0, 10.0}}};

    std::random_device rd;   // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<float> dis(0.0, 10.0);

    std::size_t id = 0;
    for (std::size_t i = 0; i < COUNT; ++i)
    {
        ++id;
        tree.Add(id, Box_t{{dis(gen), dis(gen)}, {dis(gen), dis(gen)}});
    }

    for (std::size_t r_i = 0; r_i < ROUNDS; ++r_i)
    {
        {
            const auto del_val = tree.GetAllValues().begin()->first;
            tree.Del(del_val);
            ++id;
            tree.Add(id, Box_t{{dis(gen), dis(gen)}, {dis(gen), dis(gen)}});
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