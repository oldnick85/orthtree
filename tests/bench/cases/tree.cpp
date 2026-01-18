// Copyright 2024 oldnick85

#include <chrono>

#include "./orthtree.h"

#include "../../common/test_common.h"

using namespace orthtree;

// NOLINTBEGIN

void TreeIntersectPairsRandom2D_Cuba()
{
    printf("\n ================\n TreeIntersectPairsRandom2D_Cuba\n ================\n");
    printf(
        "As a model for the benchmark, we consider the problem of finding all human collisions in a state\n"
        "similar to Cuba. It has an area of 100,000 square kilometers (a square with a side of 333 km.) and\n"
        "a population of 10M people. Let's assume that each person occupies a square with a side of 0.5 m.\n");
    printf("Benchmark has started, it may take some time...\n");

    constexpr std::size_t COUNT = 11 * 1000 * 1000;
    constexpr float area_sz     = 333000.0;
    using Tree_t                = Tree<std::size_t, float, 2>;
    using Box_t                 = Tree_t::Box_t;
    using Point_t               = Box_t::Point_t;
    Tree_t tree{Box_t{{0.0, 0.0}, {area_sz, area_sz}}};

    GenerateRandomBoxes<Tree_t, Box_t, Point_t, float, 2>(COUNT, tree, 0.0, area_sz, 0.5, 0.5);
    const auto start{std::chrono::steady_clock::now()};
    const auto tree_inters = tree.FindIntersected();
    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};
    const auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    printf("Benchmark took %ld ms.\n", elapsed_ms);
    printf("Found %zu intersections.\n", tree_inters.size());
}

void TreeIntersectPairsRandom3D_Fog()
{
    printf("\n ================\n TreeIntersectPairsRandom3D_Fog\n ================\n");
    printf(
        "As a model for the benchmark, we consider fog and we will assume that the size of the droplets in\n"
        "the fog is 0.01 mm, and the fog contains about 1000 water droplets per 1 cubic centimeter of space.\n"
        "We will do calculations for 1 liter of fog with 1M droplets.\n");
    printf("Benchmark has started, it may take some time...\n");

    constexpr std::size_t COUNT = 1 * 1000 * 1000;
    constexpr float area_sz     = 100.0;
    using Tree_t                = Tree<std::size_t, float, 3>;
    using Box_t                 = Tree_t::Box_t;
    using Point_t               = Box_t::Point_t;
    Tree_t tree{Box_t{{0.0, 0.0, 0.0}, {area_sz, area_sz, area_sz}}};

    GenerateRandomBoxes<Tree_t, Box_t, Point_t, float, 3>(COUNT, tree, 0.0, area_sz, 0.01f, 0.01f);
    const auto start{std::chrono::steady_clock::now()};
    const auto tree_inters = tree.FindIntersected();
    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};
    const auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    printf("Benchmark took %ld ms.\n", elapsed_ms);
    printf("Found %zu intersections.\n", tree_inters.size());
}

// NOLINTEND