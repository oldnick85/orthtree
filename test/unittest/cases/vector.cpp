// Copyright 2024 oldnick85

#include <gtest/gtest.h>

#include "./vector.h"

using namespace orthtree;

// NOLINTBEGIN
TEST(Vector, Base)
{
    using Vector_t = Vector<float, 2>;
    Vector_t vect;
    ASSERT_EQ(vect[0], 0.0);
    ASSERT_EQ(vect[1], 0.0);
    Vector_t vect1{1.0, 2.0};
    vect += vect1;
    ASSERT_EQ(vect[0], 1.0);
    ASSERT_EQ(vect[1], 2.0);
    vect *= 2.0;
    ASSERT_EQ(vect[0], 2.0);
    ASSERT_EQ(vect[1], 4.0);
}
// NOLINTEND