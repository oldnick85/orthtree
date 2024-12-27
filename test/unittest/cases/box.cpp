// Copyright 2024 oldnick85

#include <gtest/gtest.h>

#include "./box.h"
#include "./point.h"

using namespace orthtree;

// NOLINTBEGIN
TEST(Box, Base)
{
    using Box_t   = Box<float, 2>;
    using Point_t = Box_t::Point_t;
    Point_t pnt1{1.0, 3.0};
    Point_t pnt2{2.0, 2.0};
    Box_t box{pnt1, pnt2};
    ASSERT_EQ(box.PntMin()[0], 1.0);
    ASSERT_EQ(box.PntMin()[1], 2.0);
    ASSERT_EQ(box.PntMax()[0], 2.0);
    ASSERT_EQ(box.PntMax()[1], 3.0);

    Box_t box0{{0.0, 0.0}, {4.0, 2.0}};
    Box_t box1{{0.5, 0.5}, {1.0, 1.0}};
    Box_t box2{{2.0, -1.0}, {3.0, 3.0}};
    Box_t box3{{3.5, -1.0}, {5.0, 1.0}};
    Box_t box4{{5.5, 0.5}, {6.5, 1.5}};
    Box_t box5{{-5.5, -0.5}, {6.5, 10.5}};
    ASSERT_TRUE(box0.Intersect(box1));
    ASSERT_TRUE(box0.Intersect(box2));
    ASSERT_TRUE(box0.Intersect(box3));
    ASSERT_FALSE(box0.Intersect(box4));
    ASSERT_TRUE(box0.Intersect(box5));
}
// NOLINTEND