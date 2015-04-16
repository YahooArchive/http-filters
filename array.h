/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#ifndef ARRAY_H
#define ARRAY_H

#include <cstring>
#include <stdint.h>

#define ARRAY_SIZE(t) sizeof(t) / sizeof(t[0])

namespace util {

template < class T >
struct Array {
  typedef T Type;
  typedef Array< T > Self;

  uint32_t size;
  T * t;

  Array(void) :
    size(0), t(NULL) { }

  Array(T * const t, const uint32_t s) :
    size(s), t(t) {
    ASSERT(t != NULL);
    ASSERT(s > 0);
  }

  static inline Self Copy(const Self a) {
    const uint32_t s = a.size;
    T * t = static_cast< T * >(malloc(s * sizeof(T)));
    //C casts are so much more appropriated here.
    memcpy((void *)t, a.t, s * sizeof(T));
    return Self(t, s);
  }

  inline operator T * (void) { return t; }

  template < class U >
  inline T * operator + (const U & u) const {
    return t + u;
  }

  inline T operator [] (const uint32_t i) const {
    ASSERT(i >= 0);
    ASSERT(i < size);
    return t[i];
  }
};

} //end of util namespace
#endif //ARRAY_H
