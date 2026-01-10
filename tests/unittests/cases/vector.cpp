// Copyright 2024 oldnick85

#include <gtest/gtest.h>
#include <array>
#include <limits>
#include <span>

#include "./vector.h"

using orthtree::Vector;

// NOLINTBEGIN

/**
 * @test VectorDefaultConstructor
 * @brief Tests the default constructor initializes all coordinates to zero.
 * @details Verifies that when a Vector is created without arguments,
 *          all elements are set to TCoord{} (default floating-point zero).
 */
TEST(Vector, DefaultConstructor)
{
    const Vector<float, 3> vec;
    EXPECT_EQ(vec[0], 0.0f);
    EXPECT_EQ(vec[1], 0.0f);
    EXPECT_EQ(vec[2], 0.0f);
}

/**
 * @test VectorInitializerListConstructor
 * @brief Tests construction from initializer list.
 * @details Verifies that coordinates are correctly initialized from
 *          the provided initializer list in the specified order.
 */
TEST(Vector, InitializerListConstructor)
{
    const Vector<double, 4> vec{1.0, 2.0, 3.0, 4.0};
    EXPECT_DOUBLE_EQ(vec[0], 1.0);
    EXPECT_DOUBLE_EQ(vec[1], 2.0);
    EXPECT_DOUBLE_EQ(vec[2], 3.0);
    EXPECT_DOUBLE_EQ(vec[3], 4.0);
}

/**
 * @test VectorCopyConstructor
 * @brief Tests copy constructor creates an independent copy.
 * @details Verifies that modifying the original doesn't affect the copy
 *          and vice versa, ensuring deep copy semantics.
 */
TEST(Vector, CopyConstructor)
{
    Vector<double, 2> original{10, 20};
    Vector<double, 2> copy(original);

    EXPECT_EQ(copy[0], 10);
    EXPECT_EQ(copy[1], 20);

    // Modify original and verify copy remains unchanged
    original[0] = 100;
    EXPECT_EQ(copy[0], 10);  // Copy should not be affected
}

/**
 * @test VectorMoveConstructor
 * @brief Tests move constructor transfers ownership efficiently.
 * @details Verifies that moved-from object can be in a valid but unspecified state,
 *          and the moved-to object contains the original data.
 */
TEST(Vector, MoveConstructor)
{
    Vector<float, 2> original{1.5f, 2.5f};
    Vector<float, 2> moved(std::move(original));

    EXPECT_FLOAT_EQ(moved[0], 1.5f);
    EXPECT_FLOAT_EQ(moved[1], 2.5f);
    // Note: We don't check original as it's moved-from
}

/**
 * @test VectorArrayConstructor
 * @brief Tests construction from std::array.
 * @details Verifies that coordinates are correctly copied from the input array.
 */
TEST(Vector, ArrayConstructor)
{
    const std::array<float, 3> arr{1.0f, 2.0f, 3.0f};
    const Vector<float, 3> vec(arr);

    EXPECT_FLOAT_EQ(vec[0], 1.0f);
    EXPECT_FLOAT_EQ(vec[1], 2.0f);
    EXPECT_FLOAT_EQ(vec[2], 3.0f);
}

/**
 * @test VectorSpanConstructor
 * @brief Tests construction from std::span.
 * @details Verifies that coordinates are correctly copied from the span
 *          and the constructor handles boundary conditions properly.
 */
TEST(Vector, SpanConstructor)
{
    const float data[] = {5.0f, 6.0f, 7.0f};
    const std::span<const float> span(data, 3);
    const Vector<float, 3> vec(span);

    EXPECT_FLOAT_EQ(vec[0], 5.0f);
    EXPECT_FLOAT_EQ(vec[1], 6.0f);
    EXPECT_FLOAT_EQ(vec[2], 7.0f);
}

/**
 * @test VectorSizeMethod
 * @brief Tests the size() method returns correct dimension.
 * @details Verifies that size() returns the template parameter DIM
 *          for vectors of different dimensions.
 */
TEST(Vector, SizeMethod)
{
    const Vector<float, 2> vec2{};
    EXPECT_EQ(vec2.size(), 2);
    const Vector<float, 3> vec3{};
    EXPECT_EQ(vec3.size(), 3);
    const Vector<float, 10> vec10{};
    EXPECT_EQ(vec10.size(), 10);
}

/**
 * @test VectorElementAccess
 * @brief Tests both const and non-const element access operators.
 * @details Verifies that operator[] provides correct read/write access
 *          and maintains proper bounds (tested in debug builds).
 */
TEST(Vector, ElementAccess)
{
    Vector<float, 3> vec{1.0f, 2.0f, 3.0f};

    // Test read access
    EXPECT_FLOAT_EQ(vec[0], 1.0f);
    EXPECT_FLOAT_EQ(vec[1], 2.0f);
    EXPECT_FLOAT_EQ(vec[2], 3.0f);

    // Test write access
    vec[1] = 9.0f;
    EXPECT_FLOAT_EQ(vec[1], 9.0f);

    // Test const access
    const Vector<float, 3>& const_vec = vec;
    EXPECT_FLOAT_EQ(const_vec[0], 1.0f);
}

/**
 * @test VectorCopyAssignment
 * @brief Tests copy assignment operator.
 * @details Verifies that assignment creates an independent copy
 *          and self-assignment is handled correctly.
 */
TEST(Vector, CopyAssignment)
{
    Vector<float, 2> vec1{1.0f, 2.0f};
    Vector<float, 2> vec2{3.0f, 4.0f};

    vec2 = vec1;  // Copy assignment

    EXPECT_FLOAT_EQ(vec2[0], 1.0f);
    EXPECT_FLOAT_EQ(vec2[1], 2.0f);

    // Test self-assignment
    vec1 = vec1;  // Should be safe
    EXPECT_FLOAT_EQ(vec1[0], 1.0f);
    EXPECT_FLOAT_EQ(vec1[1], 2.0f);
}

/**
 * @test VectorScalarMultiplication
 * @brief Tests scalar multiplication operations.
 * @details Verifies both operator*= (in-place) and operator* (copying)
 *          work correctly with various scalar values.
 */
TEST(Vector, ScalarMultiplication)
{
    Vector<float, 2> vec{2.0f, 3.0f};

    // Test operator*=
    vec *= 2.0f;
    EXPECT_FLOAT_EQ(vec[0], 4.0f);
    EXPECT_FLOAT_EQ(vec[1], 6.0f);

    // Test operator* (returns new vector)
    const Vector<float, 2> vec2 = vec * 0.5f;
    EXPECT_FLOAT_EQ(vec2[0], 2.0f);
    EXPECT_FLOAT_EQ(vec2[1], 3.0f);

    // Original vector should remain unchanged
    EXPECT_FLOAT_EQ(vec[0], 4.0f);
    EXPECT_FLOAT_EQ(vec[1], 6.0f);
}

/**
 * @test VectorAddition
 * @brief Tests vector addition operations.
 * @details Verifies both operator+= (in-place) and operator+ (copying)
 *          produce correct results.
 */
TEST(Vector, Addition)
{
    Vector<float, 3> vec1{1.0f, 2.0f, 3.0f};
    const Vector<float, 3> vec2{4.0f, 5.0f, 6.0f};

    // Test operator+=
    vec1 += vec2;
    EXPECT_FLOAT_EQ(vec1[0], 5.0f);
    EXPECT_FLOAT_EQ(vec1[1], 7.0f);
    EXPECT_FLOAT_EQ(vec1[2], 9.0f);

    // Test operator+
    const Vector<float, 3> vec3 = vec1 + vec2;
    EXPECT_FLOAT_EQ(vec3[0], 9.0f);   // 5 + 4
    EXPECT_FLOAT_EQ(vec3[1], 12.0f);  // 7 + 5
    EXPECT_FLOAT_EQ(vec3[2], 15.0f);  // 9 + 6
}

/**
 * @test VectorSubtraction
 * @brief Tests vector subtraction operations.
 * @details Verifies both operator-= (in-place) and operator- (copying)
 *          produce correct results.
 */
TEST(Vector, Subtraction)
{
    Vector<float, 3> vec1{10.0f, 20.0f, 30.0f};
    const Vector<float, 3> vec2{1.0f, 2.0f, 3.0f};

    // Test operator-=
    vec1 -= vec2;
    EXPECT_FLOAT_EQ(vec1[0], 9.0f);
    EXPECT_FLOAT_EQ(vec1[1], 18.0f);
    EXPECT_FLOAT_EQ(vec1[2], 27.0f);

    // Test operator-
    const Vector<float, 3> vec3 = vec1 - vec2;
    EXPECT_FLOAT_EQ(vec3[0], 8.0f);   // 9 - 1
    EXPECT_FLOAT_EQ(vec3[1], 16.0f);  // 18 - 2
    EXPECT_FLOAT_EQ(vec3[2], 24.0f);  // 27 - 3
}

/**
 * @test VectorEquality
 * @brief Tests equality and inequality operators.
 * @details Verifies that vectors are equal only when all components match
 *          and inequality detects any difference.
 */
TEST(Vector, Equality)
{
    const Vector<float, 2> vec1{1.0f, 2.0f};
    const Vector<float, 2> vec2{1.0f, 2.0f};
    const Vector<float, 2> vec3{1.0f, 3.0f};

    EXPECT_TRUE(vec1 == vec2);
    EXPECT_FALSE(vec1 == vec3);
    EXPECT_TRUE(vec1 != vec3);
    EXPECT_FALSE(vec1 != vec2);
}

/**
 * @test VectorDotProduct
 * @brief Tests dot product calculation.
 * @details Verifies dot product formula: sum(x_i * y_i) for all dimensions.
 */
TEST(Vector, DotProduct)
{
    const Vector<float, 3> vec1{1.0f, 2.0f, 3.0f};
    const Vector<float, 3> vec2{4.0f, 5.0f, 6.0f};

    const float result = vec1.Dot(vec2);
    // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
    EXPECT_FLOAT_EQ(result, 32.0f);

    // Dot product with self should equal squared length
    EXPECT_FLOAT_EQ(vec1.Dot(vec1), vec1.Length2());
}

/**
 * @test VectorLength
 * @brief Tests length and squared length calculations.
 * @details Verifies Length2() returns dot product with itself,
 *          and Length() returns sqrt(Length2()).
 */
TEST(Vector, Length)
{
    const Vector<float, 2> vec{3.0f, 4.0f};

    // Test Length2 (squared length)
    EXPECT_FLOAT_EQ(vec.Length2(), 25.0f);  // 3² + 4² = 9 + 16 = 25

    // Test Length (Euclidean norm)
    EXPECT_FLOAT_EQ(vec.Length(), 5.0f);  // sqrt(25) = 5

    // Verify relationship between Length and Length2
    EXPECT_FLOAT_EQ(vec.Length(), std::sqrt(vec.Length2()));
}

/**
 * @test VectorNormalization
 * @brief Tests vector normalization.
 * @details Verifies that Normalize() produces a unit vector
 *          (length = 1) while preserving direction.
 */
TEST(Vector, Normalization)
{
    Vector<float, 3> vec{2.0f, 0.0f, 0.0f};
    vec.Normalize();

    EXPECT_NEAR(vec.Length(), 1.0f, std::numeric_limits<float>::epsilon());
    EXPECT_FLOAT_EQ(vec[0], 1.0f);
    EXPECT_FLOAT_EQ(vec[1], 0.0f);
    EXPECT_FLOAT_EQ(vec[2], 0.0f);

    // Test with arbitrary vector
    Vector<float, 2> vec2{3.0f, 4.0f};
    vec2.Normalize();

    EXPECT_NEAR(vec2.Length(), 1.0f, 1e-6f);
    EXPECT_FLOAT_EQ(vec2[0], 0.6f);  // 3/5
    EXPECT_FLOAT_EQ(vec2[1], 0.8f);  // 4/5
}

/**
 * @test VectorNormalizeZeroVector
 * @brief Tests normalization of zero/very small vectors.
 * @details Verifies that Normalize() handles edge cases gracefully
 *          without division by zero.
 */
TEST(Vector, NormalizeZeroVector)
{
    Vector<float, 2> zero_vec{0.0f, 0.0f};
    zero_vec.Normalize();

    // Should remain zero (or very small) without crashing
    EXPECT_FLOAT_EQ(zero_vec[0], 0.0f);
    EXPECT_FLOAT_EQ(zero_vec[1], 0.0f);

    Vector<float, 2> tiny_vec{std::numeric_limits<float>::epsilon() * 0.5f,
                              std::numeric_limits<float>::epsilon() * 0.5f};
    tiny_vec.Normalize();  // Should not divide by zero
}

/**
 * @test VectorIterators
 * @brief Tests iterator functionality for range-based loops and algorithms.
 * @details Verifies that begin(), end(), cbegin(), cend() work correctly
 *          for both const and non-const vectors.
 */
TEST(Vector, Iterators)
{
    Vector<float, 3> vec{1.0f, 2.0f, 3.0f};

    // Test non-const iterators
    float sum = 0.0f;
    for (auto& val : vec)
    {
        sum += val;
    }
    EXPECT_FLOAT_EQ(sum, 6.0f);

    // Test const iterators
    const Vector<float, 3>& const_vec = vec;
    sum                               = 0.0f;
    for (const auto& val : const_vec)
    {
        sum += val;
    }
    EXPECT_FLOAT_EQ(sum, 6.0f);

    // Test cbegin/cend
    sum = 0.0f;
    for (auto it = const_vec.cbegin(); it != const_vec.cend(); ++it)
    {
        sum += *it;
    }
    EXPECT_FLOAT_EQ(sum, 6.0f);
}

/**
 * @test VectorArithmeticCombinations
 * @brief Tests complex arithmetic expression combinations.
 * @details Verifies that multiple operations can be chained correctly.
 */
TEST(Vector, ArithmeticCombinations)
{
    const Vector<float, 2> a{1.0f, 2.0f};
    const Vector<float, 2> b{3.0f, 4.0f};
    const Vector<float, 2> c{5.0f, 6.0f};

    // Test complex expression
    const Vector<float, 2> result = (a + b) * 2.0f - c;
    // a + b = {4, 6}
    // *2 = {8, 12}
    // -c = {3, 6}

    EXPECT_FLOAT_EQ(result[0], 3.0f);
    EXPECT_FLOAT_EQ(result[1], 6.0f);
}

/**
 * @test VectorPrecision
 * @brief Tests calculations with high precision requirements.
 * @details Uses double precision to verify accuracy of operations.
 */
TEST(Vector, Precision)
{
    const Vector<double, 2> vec1{1.0e-10, 2.0e-10};
    const Vector<double, 2> vec2{3.0e-10, 4.0e-10};

    const double dot = vec1.Dot(vec2);
    // (1e-10 * 3e-10) + (2e-10 * 4e-10) = 3e-20 + 8e-20 = 11e-20
    EXPECT_DOUBLE_EQ(dot, 11.0e-20);
}

// NOLINTEND