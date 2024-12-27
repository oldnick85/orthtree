// Copyright 2024 oldnick85

#pragma once

#undef ORTHTREE_DEBUG_CHECKS
#define ORTHTREE_DEBUG_CHECKS

#ifdef ORTHTREE_DEBUG_CHECKS
#include <cassert>
#define ORTHTREE_DEBUG_ASSERT(exp, msg) assert(((void)msg, exp));
#else
#define ORTHTREE_DEBUG_ASSERT(exp, msg) \
    {}
#endif
