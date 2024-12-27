#include <gtest/gtest.h>

#include "./point.h"
#include "./vector.h"

using namespace orthtree;

// NOLINTBEGIN
TEST(Point, Base)
{
    using Point_t  = Point<float, 2>;
    using Vector_t = Vector<float, 2>;
    Point_t pnt;
    ASSERT_EQ(pnt[0], 0.0);
    ASSERT_EQ(pnt[1], 0.0);
    Vector_t vect1{1.0, 2.0};
    pnt += vect1;
    ASSERT_EQ(pnt[0], 1.0);
    ASSERT_EQ(pnt[1], 2.0);
    pnt += vect1 * 2.0;
    ASSERT_EQ(pnt[0], 3.0);
    ASSERT_EQ(pnt[1], 6.0);
}
// NOLINTEND