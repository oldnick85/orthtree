# ORTHTREE

## Description

This project is a header-only library for working with orthtrees.

From [here](https://doc.cgal.org/latest/Orthtree/index.html):

> Quadtrees are tree data structures in which each node encloses a rectangular section of space, and each internal node has exactly 4 children. Octrees are a similar data structure in 3D in which each node encloses a rectangular cuboid section of space, and each internal node has exactly 8 children.

> We call the generalization of such data structure "orthtrees", as orthants are generalizations of quadrants and octants. The term "hyperoctree" can also be found in literature to name such data structures in dimensions 4 and higher.

## Documentation

### Sample usage

First of all include header with necessary code:
```cpp
#include <orthtree.h>
```
next, we will make several types aliases for convenience's sake:
```cpp
using Tree_t = Tree<int, float, 2>;
using Box_t  = Tree_t::Box_t;
```
here we make type of tree that will store integers in 2-dimensional space with float coordinates. 
Let's make the tree spanning on rectangle with vertices (-5,-6), (-5, 7), (11, 7) and (11, -6):
```cpp
Tree_t tree{Box_t{{-5.0, -6.0}, {11.0, 7.0}}};
```
Now we can add values like that:
```cpp
tree.Add(1, Box_t{{1.0, 1.0}, {2.0, 2.0}});
tree.Add(2, Box_t{{10.0, 10.0}, {20.0, 20.0}});
```
And then find intersections with box
```cpp
const auto inters_box = tree.FindIntersected(Box_t{{-10.0, -1.0}, {1.0, 10.0}});
```
or find pairwise intersections
```cpp
const auto inters = tree.FindIntersected();
```
Sure we can delete values:
```cpp
tree.Del(1);
```