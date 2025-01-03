// Copyright 2024 oldnick85

#pragma once

#include <random>
#include <unordered_map>
#include <vector>

#include "./orthtree.h"

template <typename Tree_t, typename Box_t, typename Point_t, typename TCoord, std::size_t DIM>
void GenerateRandomBoxes(const std::size_t count, Tree_t& tree, float area_min, float area_max, float box_sz_min,
                         float box_sz_max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis_sz(box_sz_min, box_sz_max);
    std::size_t id = 0;
    for (std::size_t i = 0; i < count; ++i)
    {
        std::vector<TCoord> coords_min;
        std::vector<TCoord> coords_max;
        for (std::size_t i = 0; i < DIM; ++i)
        {
            const auto sz = dis_sz(gen);
            std::uniform_real_distribution<float> dis_pos(area_min, area_max - sz);
            const auto pos = dis_pos(gen);
            coords_min.push_back(pos);
            coords_max.push_back(pos + sz);
        }
        tree.Add(id, Box_t{Point_t{coords_min}, Point_t{coords_max}});
        ++id;
    }
}

template <typename Tree_t, typename Box_t, typename Point_t, typename TCoord, std::size_t DIM>
void AddRandomBox(const std::size_t id, Tree_t& tree, float area_min, float area_max, float box_sz_min,
                  float box_sz_max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis_sz(box_sz_min, box_sz_max);

    std::vector<TCoord> coords_min;
    std::vector<TCoord> coords_max;
    for (std::size_t i = 0; i < DIM; ++i)
    {
        const auto sz = dis_sz(gen);
        std::uniform_real_distribution<float> dis_pos(area_min, area_max - sz);
        const auto pos = dis_pos(gen);
        coords_min.push_back(pos);
        coords_max.push_back(pos + sz);
    }
    tree.Add(id, Box_t{Point_t{coords_min}, Point_t{coords_max}});
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