// Copyright 2024 oldnick85

#include <gtest/gtest.h>

#include "./box.h"
#include "./point.h"

using orthtree::Box;

// NOLINTBEGIN

/**
 * @test Base functionality tests for Box class
 * @brief Tests basic construction and intersection detection
 */
TEST(Box, Base)
{
    using Box_t   = Box<float, 2>;
    using Point_t = Box_t::Point_t;

    // Test construction from two points with automatic ordering
    Point_t pnt1{1.0, 3.0};
    Point_t pnt2{2.0, 2.0};
    Box_t box{pnt1, pnt2};

    // Verify that coordinates are properly ordered (min <= max)
    ASSERT_EQ(box.PntMin()[0], 1.0);
    ASSERT_EQ(box.PntMin()[1], 2.0);
    ASSERT_EQ(box.PntMax()[0], 2.0);
    ASSERT_EQ(box.PntMax()[1], 3.0);

    // Define test boxes for intersection checking
    Box_t box0{{0.0, 0.0}, {4.0, 2.0}};     // Base box for intersection tests
    Box_t box1{{0.5, 0.5}, {1.0, 1.0}};     // Fully inside box0
    Box_t box2{{2.0, -1.0}, {3.0, 3.0}};    // Partially overlapping
    Box_t box3{{3.5, -1.0}, {5.0, 1.0}};    // Partially overlapping
    Box_t box4{{5.5, 0.5}, {6.5, 1.5}};     // Non-overlapping (disjoint)
    Box_t box5{{-5.5, -0.5}, {6.5, 10.5}};  // Encloses box0 completely

    // Test various intersection scenarios
    ASSERT_TRUE(box0.Intersect(box1));   // Small box inside large box
    ASSERT_TRUE(box0.Intersect(box2));   // Partial overlap
    ASSERT_TRUE(box0.Intersect(box3));   // Partial overlap (edge case)
    ASSERT_FALSE(box0.Intersect(box4));  // No overlap (disjoint)
    ASSERT_TRUE(box0.Intersect(box5));   // box5 encloses box0
}

/**
 * @test Constructor tests
 * @brief Tests various constructors of Box class
 */
TEST(Box, Constructors)
{
    using Box_t   = Box<float, 2>;
    using Point_t = Box_t::Point_t;

    // Test construction from single point (zero-volume box)
    Point_t point{2.0, 3.0};
    Box_t singlePointBox{point};
    ASSERT_EQ(singlePointBox.PntMin()[0], 2.0);
    ASSERT_EQ(singlePointBox.PntMin()[1], 3.0);
    ASSERT_EQ(singlePointBox.PntMax()[0], 2.0);
    ASSERT_EQ(singlePointBox.PntMax()[1], 3.0);

    // Test move constructor from point
    Point_t movablePoint{4.0, 5.0};
    Box_t movedBox{std::move(movablePoint)};
    ASSERT_EQ(movedBox.PntMin()[0], 4.0);
    ASSERT_EQ(movedBox.PntMin()[1], 5.0);

    // Test construction with unordered coordinates
    Box_t unorderedBox{{3.0, 5.0}, {1.0, 2.0}};
    ASSERT_EQ(unorderedBox.PntMin()[0], 1.0);
    ASSERT_EQ(unorderedBox.PntMin()[1], 2.0);
    ASSERT_EQ(unorderedBox.PntMax()[0], 3.0);
    ASSERT_EQ(unorderedBox.PntMax()[1], 5.0);
}

/**
 * @test Midpoint calculation tests
 * @brief Tests PntMid() method for center point calculation
 */
TEST(Box, Midpoint)
{
    using Box_t = Box<float, 2>;

    // Test with symmetric box
    Box_t box1{{0.0, 0.0}, {2.0, 4.0}};
    auto mid1 = box1.PntMid();
    ASSERT_EQ(mid1[0], 1.0);  // (0+2)/2 = 1.0
    ASSERT_EQ(mid1[1], 2.0);  // (0+4)/2 = 2.0

    // Test per-dimension midpoint calculation
    ASSERT_EQ(box1.PntMid(0), 1.0);  // X dimension midpoint
    ASSERT_EQ(box1.PntMid(1), 2.0);  // Y dimension midpoint

    // Test with asymmetric box
    Box_t box2{{1.0, 2.0}, {5.0, 6.0}};
    auto mid2 = box2.PntMid();
    ASSERT_EQ(mid2[0], 3.0);  // (1+5)/2 = 3.0
    ASSERT_EQ(mid2[1], 4.0);  // (2+6)/2 = 4.0
}

/**
 * @test String representation tests
 * @brief Tests Str() method for string conversion
 */
TEST(Box, StringRepresentation)
{
    using Box_t = Box<float, 2>;

    Box_t box{{1.0, 2.0}, {3.0, 4.0}};
    std::string str = box.Str();

    // Expected format: "min_point-max_point"
    // Assuming Point::Str() returns format like "(1,2)"
    EXPECT_NE(str.find('1'), std::string::npos);  // Contains min x
    EXPECT_NE(str.find('2'), std::string::npos);  // Contains min y
    EXPECT_NE(str.find('3'), std::string::npos);  // Contains max x
    EXPECT_NE(str.find('4'), std::string::npos);  // Contains max y
    EXPECT_NE(str.find('-'), std::string::npos);  // Contains separator
}

/**
 * @test Intersection computation tests
 * @brief Tests Intersection() method returning optional box
 */
TEST(Box, IntersectionComputation)
{
    using Box_t = Box<float, 2>;

    // Test overlapping boxes
    Box_t box1{{0.0, 0.0}, {2.0, 2.0}};
    Box_t box2{{1.0, 1.0}, {3.0, 3.0}};

    auto intersection = box1.Intersection(box2);
    ASSERT_TRUE(intersection.has_value());  // Should have intersection
    EXPECT_EQ(intersection->PntMin()[0], 1.0);
    EXPECT_EQ(intersection->PntMin()[1], 1.0);
    EXPECT_EQ(intersection->PntMax()[0], 2.0);
    EXPECT_EQ(intersection->PntMax()[1], 2.0);

    // Test non-overlapping boxes
    Box_t box3{{0.0, 0.0}, {1.0, 1.0}};
    Box_t box4{{2.0, 2.0}, {3.0, 3.0}};

    auto noIntersection = box3.Intersection(box4);
    ASSERT_FALSE(noIntersection.has_value());  // Should not intersect

    // Test edge-touching boxes (considered intersecting)
    Box_t box5{{0.0, 0.0}, {1.0, 1.0}};
    Box_t box6{{1.0, 0.0}, {2.0, 1.0}};

    auto edgeIntersection = box5.Intersection(box6);
    ASSERT_TRUE(edgeIntersection.has_value());  // Touching at edge
    EXPECT_EQ(edgeIntersection->PntMin()[0], 1.0);
    EXPECT_EQ(edgeIntersection->PntMin()[1], 0.0);
    EXPECT_EQ(edgeIntersection->PntMax()[0], 1.0);
    EXPECT_EQ(edgeIntersection->PntMax()[1], 1.0);
}

/**
 * @test Containment tests
 * @brief Tests Contain() and ContainStrict() methods
 */
TEST(Box, Containment)
{
    using Box_t = Box<float, 2>;

    Box_t outer{{0.0, 0.0}, {4.0, 4.0}};
    Box_t inner{{1.0, 1.0}, {3.0, 3.0}};
    Box_t edge{{0.0, 0.0}, {4.0, 4.0}};     // Same as outer
    Box_t partial{{1.0, 1.0}, {5.0, 5.0}};  // Partially outside

    // Test regular containment (boundaries allowed)
    ASSERT_TRUE(outer.Contain(inner));     // Fully contained
    ASSERT_TRUE(outer.Contain(edge));      // Same box (boundaries match)
    ASSERT_FALSE(outer.Contain(partial));  // Partially outside

    // Test strict containment (no boundary contact)
    ASSERT_TRUE(outer.ContainStrict(inner));  // Fully inside, no boundary contact
    ASSERT_FALSE(outer.ContainStrict(edge));  // Same box (boundaries touch)

    Box_t boundaryTouch{{0.0, 1.0}, {3.0, 4.0}};       // Touches left boundary
    ASSERT_FALSE(outer.ContainStrict(boundaryTouch));  // Touches boundary
}

/**
 * @test Orthant containment tests
 * @brief Tests ContainOrthant() and ContainInOrthant() methods
 */
TEST(Box, OrthantContainment)
{
    using Box_t = Box<float, 2>;

    // Test ContainOrthant - checks if box overlaps with at least one orthant
    Box_t base_box{{0.0, 0.0}, {4.0, 4.0}};

    // Box covering one quarter (orthant) of base box
    Box_t orthantBox{{0.0, 0.0}, {2.0, 2.0}};  // Bottom-left orthant

    // Box should contain at least one orthant of itself
    EXPECT_TRUE(base_box.ContainOrthant(base_box));

    // Test with various boxes
    Box_t box1{{0.0, 0.0}, {2.0, 2.0}};  // Bottom-left orthant
    Box_t box2{{2.0, 2.0}, {4.0, 4.0}};  // Top-right orthant
    Box_t box3{{0.0, 2.0}, {2.0, 4.0}};  // Top-left orthant
    Box_t box4{{2.0, 0.0}, {4.0, 2.0}};  // Bottom-right orthant

    EXPECT_TRUE(base_box.ContainOrthant(box1));
    EXPECT_TRUE(base_box.ContainOrthant(box2));
    EXPECT_TRUE(base_box.ContainOrthant(box3));
    EXPECT_TRUE(base_box.ContainOrthant(box4));

    // Box that's too small to contain any full orthant
    Box_t small_box{{1.0, 1.0}, {1.5, 1.5}};
    ASSERT_FALSE(small_box.ContainOrthant(base_box));

    // Test ContainInOrthant - checks if box is within a single orthant
    Box_t parent{{0.0, 0.0}, {8.0, 8.0}};
    Box_t child1{{0.1, 0.1}, {3.9, 3.9}};    // Bottom-left orthant
    Box_t child2{{4.1, 4.1}, {7.9, 7.9}};    // Top-right orthant
    Box_t straddle{{2.0, 2.0}, {6.0, 6.0}};  // Straddles midpoint

    EXPECT_TRUE(parent.ContainInOrthant(child1));     // Within one orthant
    EXPECT_TRUE(parent.ContainInOrthant(child2));     // Within another orthant
    EXPECT_FALSE(parent.ContainInOrthant(straddle));  // Crosses midpoint
}

/**
 * @test 3D box tests
 * @brief Tests Box class functionality in 3D space
 */
TEST(Box, ThreeDimensional)
{
    using Box3D   = Box<float, 3>;
    using Point3D = Box3D::Point_t;

    // Test 3D construction and basic properties
    Point3D p1{0.0, 0.0, 0.0};
    Point3D p2{2.0, 3.0, 4.0};
    Box3D box3d{p1, p2};

    ASSERT_EQ(box3d.PntMin()[0], 0.0);
    ASSERT_EQ(box3d.PntMin()[1], 0.0);
    ASSERT_EQ(box3d.PntMin()[2], 0.0);
    ASSERT_EQ(box3d.PntMax()[0], 2.0);
    ASSERT_EQ(box3d.PntMax()[1], 3.0);
    ASSERT_EQ(box3d.PntMax()[2], 4.0);

    // Test 3D midpoint
    auto mid3d = box3d.PntMid();
    ASSERT_EQ(mid3d[0], 1.0);  // (0+2)/2
    ASSERT_EQ(mid3d[1], 1.5);  // (0+3)/2
    ASSERT_EQ(mid3d[2], 2.0);  // (0+4)/2

    // Test 3D intersection
    Box3D boxA{{0.0, 0.0, 0.0}, {2.0, 2.0, 2.0}};
    Box3D boxB{{1.0, 1.0, 1.0}, {3.0, 3.0, 3.0}};

    ASSERT_TRUE(boxA.Intersect(boxB));

    auto intersect3D = boxA.Intersection(boxB);
    ASSERT_TRUE(intersect3D.has_value());
    EXPECT_EQ(intersect3D->PntMin()[0], 1.0);
    EXPECT_EQ(intersect3D->PntMin()[1], 1.0);
    EXPECT_EQ(intersect3D->PntMin()[2], 1.0);
}

/**
 * @test Edge case tests
 * @brief Tests boundary conditions and special cases
 */
TEST(Box, EdgeCases)
{
    using Box_t = Box<float, 2>;

    // Test with zero-size box (point)
    Box_t point_box{{2.0, 3.0}, {2.0, 3.0}};
    ASSERT_EQ(point_box.PntMin()[0], point_box.PntMax()[0]);
    ASSERT_EQ(point_box.PntMin()[1], point_box.PntMax()[1]);

    // Test containment of zero-size box
    Box_t large_box{{2.0, 0.0}, {10.0, 10.0}};
    ASSERT_TRUE(large_box.Contain(point_box));
    ASSERT_FALSE(large_box.ContainStrict(point_box));  // On boundary

    // Test intersection with zero-size box
    Box_t another_point{{2.0, 3.0}, {2.0, 3.0}};
    ASSERT_TRUE(point_box.Intersect(another_point));  // Same point

    // Test with negative coordinates
    Box_t negative_box{{-5.0, -5.0}, {-1.0, -1.0}};
    Box_t mixed_box{{-3.0, -3.0}, {3.0, 3.0}};

    auto negative_intersection = negative_box.Intersection(mixed_box);
    ASSERT_TRUE(negative_intersection.has_value());
    EXPECT_EQ(negative_intersection->PntMin()[0], -3.0);
    EXPECT_EQ(negative_intersection->PntMin()[1], -3.0);
}

/**
 * @test Float precision tests
 * @brief Tests behavior with floating-point precision considerations
 */
TEST(Box, FloatingPointPrecision)
{
    using Box_t = Box<float, 2>;

    // Test with very small boxes (near zero size)
    Box_t tiny_box{{1.0f, 1.0f},
                   {1.0f + std::numeric_limits<float>::epsilon(), 1.0f + std::numeric_limits<float>::epsilon()}};

    // Should still intersect with itself
    ASSERT_TRUE(tiny_box.Intersect(tiny_box));

    // Test with values near boundary
    const float eps = std::numeric_limits<float>::epsilon();
    Box_t box1{{0.0f, 0.0f}, {1.0f, 1.0f}};
    Box_t box2{{1.0f - eps, 1.0f - eps}, {2.0f, 2.0f}};

    // These should intersect (within floating-point precision)
    ASSERT_TRUE(box1.Intersect(box2));
}

/**
 * @test Dimension-specific tests
 * @brief Tests behavior in different dimensions (1D, 2D, 3D)
 */
TEST(Box, DifferentDimensions)
{
    // Test 1D box (interval)
    using Box1D = Box<float, 1>;
    Box1D interval1{{1.0}, {3.0}};
    Box1D interval2{{2.0}, {4.0}};

    ASSERT_TRUE(interval1.Intersect(interval2));
    auto inter1D = interval1.Intersection(interval2);
    ASSERT_TRUE(inter1D.has_value());
    EXPECT_EQ(inter1D->PntMin()[0], 2.0);
    EXPECT_EQ(inter1D->PntMax()[0], 3.0);

    // Test 4D box
    using Box4D = Box<float, 4>;
    Box4D::Point_t pnt1{0.0, 0.0, 0.0, 0.0};
    Box4D::Point_t pnt2{1.0, 1.0, 1.0, 1.0};
    Box4D box4d{pnt1, pnt2};

    // Verify all dimensions are properly handled
    for (size_t i = 0; i < 4; ++i)
    {
        EXPECT_EQ(box4d.PntMin()[i], 0.0);
        EXPECT_EQ(box4d.PntMax()[i], 1.0);
        EXPECT_EQ(box4d.PntMid(i), 0.5);
    }
}

// NOLINTEND