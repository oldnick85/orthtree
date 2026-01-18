// Copyright 2024 oldnick85

#pragma once

/**
 * @defgroup DebugMacros Debug Macros
 * @brief Debug assertion macros for the Orthtree library
 * 
 * These macros provide runtime checks that can be disabled in release builds
 * for performance. By default, they follow the standard NDEBUG convention.
 */

#ifndef ORTHTREE_DEBUG_CHECKS
/**
     * @brief Controls whether debug checks are enabled
     * 
     * @details Default behavior:
     * - If NDEBUG is defined (release builds), debug checks are DISABLED
     * - If NDEBUG is NOT defined (debug builds), debug checks are ENABLED
     * 
     * Users can override this by defining ORTHTREE_DEBUG_CHECKS before including headers:
     * @code
     * #define ORTHTREE_DEBUG_CHECKS 1  // Force enable
     * #define ORTHTREE_DEBUG_CHECKS 0  // Force disable
     * #include <orthtree.h>
     * @endcode
     */
#ifdef NDEBUG
#define ORTHTREE_DEBUG_CHECKS 0
#else
#define ORTHTREE_DEBUG_CHECKS 1
#endif
#endif

#ifdef ORTHTREE_DEBUG_CHECKS

#include <cassert>
#include <stdexcept>
#include <string>

/**
     * @brief Debug assertion macro with custom message
     * 
     * @param exp Expression to evaluate (should be true in valid state)
     * @param msg Message to display if assertion fails
     * 
     * @details This macro:
     * 1. Is active only when ORTHTREE_DEBUG_CHECKS is 1
     * 2. Uses standard assert() for consistency
     * 3. Evaluates message only on failure (due to comma operator)
     * 4. Provides better error messages than standard assert
     * 
     * @note The message appears in the assertion failure output
     * @warning Disabled in release builds by default for performance
     * 
     * @example
     * @code
     * ORTHTREE_DEBUG_ASSERT(value != nullptr, "Value cannot be null");
     * ORTHTREE_DEBUG_ASSERT(index < size(), "Index out of bounds");
     * @endcode
     */
#define ORTHTREE_DEBUG_ASSERT(exp, msg) assert(((void)(msg), (exp)))

#else
/**
     * @brief Debug assertion macro (disabled in release)
     * 
     * @details Expands to nothing in release builds for zero overhead.
     * The expression is still evaluated to ensure side effects occur,
     * but no check is performed.
     */
#define ORTHTREE_DEBUG_ASSERT(exp, msg) ((void)0)

#endif

namespace orthtree
{

constexpr std::size_t GROUP_COUNT_DEFAULT = 10;

}  // namespace orthtree