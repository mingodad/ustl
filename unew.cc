// This file is part of the uSTL library, an STL implementation.
//
// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "unew.h"

void* tmalloc (size_t n)
{
    void* p = malloc (n);
    if (!p)
	throw ustl::bad_alloc (n);
    return p;
}

void nfree (void* p) noexcept
{
    if (p)
	free (p);
}

#if WITHOUT_LIBSTDCPP
void* operator new (size_t n)	WEAKALIAS("tmalloc");
void* operator new[] (size_t n)	WEAKALIAS("tmalloc");

void  operator delete (void* p) noexcept	WEAKALIAS("nfree");
void  operator delete[] (void* p) noexcept	WEAKALIAS("nfree");
#if HAVE_CPP14
void  operator delete (void* p, size_t n)	WEAKALIAS("nfree");
void  operator delete[] (void* p, size_t n)	WEAKALIAS("nfree");
#endif // HAVE_CPP14
#endif // WITHOUT_LIBSTDCPP
