/*--------------------------------------------------------------------
 *
 * (C) Copyright Koninklijke Philips Electronics NV 2006. 
 * All rights reserved. This software is licensed under the terms of
 * version 2.1 of the GNU Lesser General Public License as published 
 * by the Free Software Foundation. For licensing and warranty
 * information, see the file COPYING in the main directory.
 *
 *------------------------------------------------------------------*/

#ifndef MINMAX_H
#define MINMAX_H

#define NO_ALGO
#ifdef NO_ALGO
#include <string.h>

template<class T>
T min(const T& a, const T& b)
{
  return a < b ? a : b;
}

template<class T>
T max(const T& a, const T& b)
{
  return a > b ? a : b;
}

template<class T>
void copy(const T* begin, const T* end, T* result)
{
  while (begin != end)
    *result++ = *begin++;

  // memcpy(result, begin, (end-begin)*sizeof(T));
}

#else
#include <algorithm>
using std::min;
using std::max;
using std::copy;
#endif

#endif
