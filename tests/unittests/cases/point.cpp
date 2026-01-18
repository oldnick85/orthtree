// Copyright 2024 oldnick85

#include <gtest/gtest.h>
#include <array>
#include <sstream>
#include <string>

#include "./point.h"
#include "./vector.h"

using orthtree::Point;
using orthtree::Vector;

// NOLINTBEGIN

/**
 * @brief Test fixture for Point class tests.
 * 
 * This fixture provides commonly used types and helper functions
 * for testing the Point class in 2D and 3D configurations.
 */
class PointTest : public ::testing::Test
{
  protected:
    using Point2D  = Point<float, 2>;
    using Vector2D = Vector<float, 2>;
    using Point3D  = Point<float, 3>;
    using Vector3D = Vector<float, 3>;

    // Default tolerance for floating-point comparisons
    static constexpr float kEpsilon = 1e-6f;
};

/**
 * @brief Test basic construction and simple operations of Point class.
 * 
 * Verifies:
 * 1. Default constructor initializes to zero
 * 2. Vector addition assignment operator
 * 3. Scalar multiplication of vectors
 */
TEST_F(PointTest, BasicOperations)
{
    Point2D point;
    EXPECT_FLOAT_EQ(point[0], 0.0f);
    EXPECT_FLOAT_EQ(point[1], 0.0f);

    Vector2D vector{1.0f, 2.0f};
    point += vector;
    EXPECT_FLOAT_EQ(point[0], 1.0f);
    EXPECT_FLOAT_EQ(point[1], 2.0f);

    point += vector * 2.0f;
    EXPECT_FLOAT_EQ(point[0], 3.0f);
    EXPECT_FLOAT_EQ(point[1], 6.0f);
}

/**
 * @brief Test all constructors of Point class.
 * 
 * Verifies:
 * 1. Default constructor
 * 2. Initializer list constructor
 * 3. Span constructor
 * 4. Copy constructor
 * 5. Move constructor
 */
TEST_F(PointTest, Constructors)
{
    // Default constructor
    Point2D default_point;
    EXPECT_FLOAT_EQ(default_point[0], 0.0f);
    EXPECT_FLOAT_EQ(default_point[1], 0.0f);

    // Initializer list constructor
    Point2D list_point{3.5f, -2.1f};
    EXPECT_FLOAT_EQ(list_point[0], 3.5f);
    EXPECT_FLOAT_EQ(list_point[1], -2.1f);

    // Span constructor
    std::array<float, 2> arr = {1.1f, 2.2f};
    Point2D span_point{std::span{arr}};
    EXPECT_FLOAT_EQ(span_point[0], 1.1f);
    EXPECT_FLOAT_EQ(span_point[1], 2.2f);

    // Copy constructor
    Point2D original{7.0f, 8.0f};
    Point2D copy(original);
    EXPECT_FLOAT_EQ(copy[0], 7.0f);
    EXPECT_FLOAT_EQ(copy[1], 8.0f);

    // Move constructor
    Point2D source{9.0f, 10.0f};
    Point2D moved(std::move(source));
    EXPECT_FLOAT_EQ(moved[0], 9.0f);
    EXPECT_FLOAT_EQ(moved[1], 10.0f);
}

/**
 * @brief Test assignment operators of Point class.
 * 
 * Verifies:
 * 1. Copy assignment operator
 * 2. Move assignment operator
 */
TEST_F(PointTest, AssignmentOperators)
{
    Point2D point1{1.0f, 2.0f};
    Point2D point2{3.0f, 4.0f};

    // Copy assignment
    point2 = point1;
    EXPECT_FLOAT_EQ(point2[0], 1.0f);
    EXPECT_FLOAT_EQ(point2[1], 2.0f);

    // Move assignment
    Point2D point3{5.0f, 6.0f};
    point2 = std::move(point3);
    EXPECT_FLOAT_EQ(point2[0], 5.0f);
    EXPECT_FLOAT_EQ(point2[1], 6.0f);
}

/**
 * @brief Test coordinate access operators.
 * 
 * Verifies:
 * 1. Const index accessor
 * 2. Non-const index accessor (for modification)
 * 3. size() method returns correct dimension
 */
TEST_F(PointTest, CoordinateAccess)
{
    Point2D point{1.5f, 2.5f};

    // Const access
    EXPECT_FLOAT_EQ(point[0], 1.5f);
    EXPECT_FLOAT_EQ(point[1], 2.5f);

    // Non-const access and modification
    point[0] = 3.5f;
    point[1] = 4.5f;
    EXPECT_FLOAT_EQ(point[0], 3.5f);
    EXPECT_FLOAT_EQ(point[1], 4.5f);

    // Size method
    EXPECT_EQ(point.size(), 2u);
}

/**
 * @brief Test arithmetic operators with vectors.
 * 
 * Verifies:
 * 1. Addition operator (+) with vector
 * 2. Addition assignment operator (+=) with vector
 * 3. Subtraction operator (-) with vector
 * 4. Subtraction assignment operator (-=) with vector
 */
TEST_F(PointTest, ArithmeticWithVectors)
{
    Point2D point{1.0f, 2.0f};
    Vector2D vector{3.0f, 4.0f};

    // Addition operator
    Point2D sum = point + vector;
    EXPECT_FLOAT_EQ(sum[0], 4.0f);
    EXPECT_FLOAT_EQ(sum[1], 6.0f);

    // Addition assignment operator
    point += vector;
    EXPECT_FLOAT_EQ(point[0], 4.0f);
    EXPECT_FLOAT_EQ(point[1], 6.0f);

    // Subtraction operator
    Point2D diff = point - vector;
    EXPECT_FLOAT_EQ(diff[0], 1.0f);
    EXPECT_FLOAT_EQ(diff[1], 2.0f);

    // Subtraction assignment operator
    point -= vector;
    EXPECT_FLOAT_EQ(point[0], 1.0f);
    EXPECT_FLOAT_EQ(point[1], 2.0f);
}

/**
 * @brief Test point-to-point subtraction operator.
 * 
 * Verifies that subtracting two points returns the correct vector
 * representing the difference between them.
 */
TEST_F(PointTest, PointSubtraction)
{
    Point2D point1{5.0f, 6.0f};
    Point2D point2{2.0f, 3.0f};

    Vector2D difference = point1 - point2;
    EXPECT_FLOAT_EQ(difference[0], 3.0f);
    EXPECT_FLOAT_EQ(difference[1], 3.0f);

    // Test commutative property (point2 - point1 should give negative)
    Vector2D reverse = point2 - point1;
    EXPECT_FLOAT_EQ(reverse[0], -3.0f);
    EXPECT_FLOAT_EQ(reverse[1], -3.0f);
}

/**
 * @brief Test comparison operators.
 * 
 * Verifies:
 * 1. Equality operator (==) for identical and different points
 * 2. Inequality operator (!=) for identical and different points
 */
TEST_F(PointTest, ComparisonOperators)
{
    Point2D point1{1.0f, 2.0f};
    Point2D point2{1.0f, 2.0f};
    Point2D point3{1.0f, 3.0f};
    Point2D point4{3.0f, 2.0f};

    // Equality operator
    EXPECT_TRUE(point1 == point2);
    EXPECT_FALSE(point1 == point3);
    EXPECT_FALSE(point1 == point4);

    // Inequality operator
    EXPECT_FALSE(point1 != point2);
    EXPECT_TRUE(point1 != point3);
    EXPECT_TRUE(point1 != point4);
}

/**
 * @brief Test string representation method.
 * 
 * Verifies that Str() method returns properly formatted string
 * representation of the point.
 */
TEST_F(PointTest, StringRepresentation)
{
    Point2D point2d{1.5f, -2.5f};
    EXPECT_EQ(point2d.Str(), "(1.5;-2.5)");

    Point3D point3d{1.0f, 2.0f, 3.0f};
    EXPECT_EQ(point3d.Str(), "(1;2;3)");
}

/**
 * @brief Test midpoint calculation methods.
 * 
 * Verifies:
 * 1. Static Mid() method between two points
 * 2. Static Mid() method for specific axis
 * 3. Member MidTo() method for specific axis
 */
TEST_F(PointTest, MidpointCalculations)
{
    Point2D point1{0.0f, 0.0f};
    Point2D point2{4.0f, 6.0f};

    // Static Mid method (full point)
    Point2D midpoint = Point2D::Mid(point1, point2);
    EXPECT_FLOAT_EQ(midpoint[0], 2.0f);
    EXPECT_FLOAT_EQ(midpoint[1], 3.0f);

    // Static Mid method for specific axis
    float mid_x = Point2D::Mid(point1, point2, 0);
    float mid_y = Point2D::Mid(point1, point2, 1);
    EXPECT_FLOAT_EQ(mid_x, 2.0f);
    EXPECT_FLOAT_EQ(mid_y, 3.0f);

    // Member MidTo method for specific axis
    Point2D mid_to = point1.MidTo(point2, 0);  // Only x coordinate changes
    EXPECT_FLOAT_EQ(mid_to[0], 2.0f);          // Midpoint in x
    EXPECT_FLOAT_EQ(mid_to[1], 0.0f);          // y stays from point1

    // Test with negative coordinates
    Point2D point3{-2.0f, -4.0f};
    Point2D point4{2.0f, 4.0f};
    midpoint = Point2D::Mid(point3, point4);
    EXPECT_FLOAT_EQ(midpoint[0], 0.0f);
    EXPECT_FLOAT_EQ(midpoint[1], 0.0f);
}

/**
 * @brief Test iterator functionality.
 * 
 * Verifies that Point class provides proper iterators for
 * range-based for loops and STL algorithms.
 */
TEST_F(PointTest, Iterators)
{
    Point2D point{1.0f, 2.0f};

    // Test non-const iterators
    float sum = 0.0f;
    for (float coord : point)
    {
        sum += coord;
    }
    EXPECT_FLOAT_EQ(sum, 3.0f);

    // Test const iterators
    const Point2D const_point{3.0f, 4.0f};
    sum = 0.0f;
    for (float coord : const_point)
    {
        sum += coord;
    }
    EXPECT_FLOAT_EQ(sum, 7.0f);

    // Test cbegin/cend
    sum = 0.0f;
    for (auto it = const_point.cbegin(); it != const_point.cend(); ++it)
    {
        sum += *it;
    }
    EXPECT_FLOAT_EQ(sum, 7.0f);

    // Test modification through iterators
    point[0] = 5.0f;
    point[1] = 6.0f;
    EXPECT_FLOAT_EQ(point[0], 5.0f);
    EXPECT_FLOAT_EQ(point[1], 6.0f);
}

/**
 * @brief Test 3D point functionality.
 * 
 * Verifies that Point class works correctly in 3D configuration.
 */
TEST_F(PointTest, ThreeDimensionalPoint)
{
    Point3D point{1.0f, 2.0f, 3.0f};
    Vector3D vector{4.0f, 5.0f, 6.0f};

    EXPECT_FLOAT_EQ(point[0], 1.0f);
    EXPECT_FLOAT_EQ(point[1], 2.0f);
    EXPECT_FLOAT_EQ(point[2], 3.0f);
    EXPECT_EQ(point.size(), 3u);

    // Test arithmetic operations in 3D
    Point3D translated = point + vector;
    EXPECT_FLOAT_EQ(translated[0], 5.0f);
    EXPECT_FLOAT_EQ(translated[1], 7.0f);
    EXPECT_FLOAT_EQ(translated[2], 9.0f);

    // Test midpoint in 3D
    Point3D point1{0.0f, 0.0f, 0.0f};
    Point3D point2{2.0f, 4.0f, 6.0f};
    Point3D midpoint = Point3D::Mid(point1, point2);
    EXPECT_FLOAT_EQ(midpoint[0], 1.0f);
    EXPECT_FLOAT_EQ(midpoint[1], 2.0f);
    EXPECT_FLOAT_EQ(midpoint[2], 3.0f);
}

/**
 * @brief Test edge cases and special scenarios.
 * 
 * Verifies behavior with:
 * 1. Very large coordinate values
 * 2. Very small (near-zero) coordinate values
 * 3. Self-assignment
 * 4. Chained operations
 */
TEST_F(PointTest, EdgeCases)
{
    // Large values
    Point2D large{1e10f, -1e10f};
    Vector2D vec{1e10f, 1e10f};
    Point2D result = large + vec;
    EXPECT_FLOAT_EQ(result[0], 2e10f);
    EXPECT_FLOAT_EQ(result[1], 0.0f);

    // Small values
    Point2D small{1e-10f, 2e-10f};
    small += Vector2D{1e-10f, 1e-10f};
    EXPECT_FLOAT_EQ(small[0], 2e-10f);
    EXPECT_FLOAT_EQ(small[1], 3e-10f);

    // Self-assignment
    Point2D point{1.0f, 2.0f};
    point = point;  // Should handle self-assignment correctly
    EXPECT_FLOAT_EQ(point[0], 1.0f);
    EXPECT_FLOAT_EQ(point[1], 2.0f);

    // Chained operations
    point = point + Vector2D{1.0f, 1.0f} + Vector2D{2.0f, 2.0f};
    EXPECT_FLOAT_EQ(point[0], 4.0f);
    EXPECT_FLOAT_EQ(point[1], 5.0f);
}

// NOLINTEND