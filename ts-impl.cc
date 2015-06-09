/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#include <algorithm>
#include <cstring>
#include <sstream>

#include "ts.h"
#include "ts-impl.h"

namespace http {
namespace filters {

struct Contains {
  const char * const p;
  const size_t s;
  Contains(const char * const p, const size_t s) :
    p(p), s(s) {
    ASSERT(p != NULL);
    ASSERT(s <= strlen(p));
  }
  template < class I >
  bool operator () (const I & i) const {
    const char * const begin = i->pointer,
          * const end = begin + i->length;
    return std::search(begin, end, p, p + s) != end;
  }
};

struct Equal {
  const char * const p;
  const size_t s;
  Equal(const char * const p, const size_t s) :
    p(p), s(s) {
    ASSERT(p != NULL);
    ASSERT(s <= strlen(p));
  }
  template < class I >
  bool operator () (const I & i) const {
    return s == i->length && memcmp(p, i->pointer, s) == 0;
  }
};

template < class T >
struct GreaterThan {
  const T t;
  GreaterThan(const T & t) : t(t) { }
  template < class I >
  bool operator () (const I & i) const {
    std::istringstream ss(i->str());
    int64_t c;
    ss >> c;
    return ! ss.fail() && c > t;
  }
};

template < class T >
struct GreaterThanAfter {
  const char * const p;
  const size_t s;
  const T t;
  GreaterThanAfter(const char * const p, const size_t s,
      const T & t) : p(p), s(s), t(t) {
    ASSERT(s <= strlen(p));
  }
  template < class I >
  bool operator () (const I & i) const {
    const char * const begin = i->pointer,
          * const end = begin + i->length,
          * iterator = std::search(begin, end, p, p + s);
    while (iterator != end) {
      iterator += s;
      ASSERT(iterator <= end);
      std::istringstream ss(std::string(iterator, end - iterator));
      int64_t c;
      ss >> c;
      if ( ! ss.fail() && c > t) {
        return true;
      }
      iterator = std::search(iterator, end, p, p + s);
    }
    return false;
  }
};

template < class T >
struct LessThan {
  const T t;
  LessThan(const T & t) : t(t) { }
  template < class I >
  bool operator () (const I & i) const {
    std::istringstream ss(i->str());
    int64_t c;
    ss >> c;
    return ! ss.fail() && c < t;
  }
};

template < class T >
struct LessThanAfter{
  const char * const p;
  const size_t s;
  const T t;
  LessThanAfter(const char * const p, const size_t s,
      const T & t) : p(p), s(s), t(t) {
    ASSERT(s <= strlen(p));
  }
  template < class I >
  bool operator () (const I & i) const {
    const char * const begin = i->pointer,
          * const end = begin + i->length,
          * iterator = std::search(begin, end, p, p + s);
    while (iterator != end) {
      iterator += s;
      ASSERT(iterator <= end);
      std::istringstream ss(std::string(iterator, end - iterator));
      int64_t c;
      ss >> c;
      if ( ! ss.fail() && c < t) {
        return true;
      }
      iterator = std::search(iterator, end, p, p + s);
    }
    return false;
  }
};

struct StartsWith {
  const char * const p;
  const size_t s;
  const size_t o;
  StartsWith(const char * const p, const size_t s, const size_t o) :
    p(p), s(s), o(o) {
    ASSERT(p != NULL);
    ASSERT(s <= strlen(p));
  }
  template < class I >
  bool operator () (const I & i) const {
    const char * const begin = i->pointer,
          * const end = begin + i->length;
    return std::search(begin, end, p, p + s) != begin + o;
  }
};

template < class T, class U >
bool Loop(const typename T::Result & r, const U & u) {
  typedef typename T::Values::const_iterator Iterator;
  if ( ! r.second) {
    return false;
  }
  ASSERT(r.first != NULL);
  const Iterator end = r.first->end();
  Iterator it = r.first->begin();
  for (; it != end; ++it) {
    ASSERT(it->pointer != NULL);
    ASSERT(it->length > 0);
    if (u(it)) {
      return true;
    }
  }
  return false;
}

bool TSImplementation::ContainsHeader(
    const char * const a, const char * const b) {
  return Loop< Headers >(headers()[a], Contains(b, strlen(b)));
}

bool TSImplementation::EqualHeader(
    const char * const a, const char * const b) {
  return Loop< Headers >(headers()[a], Equal(b, strlen(b)));
}

bool TSImplementation::GreaterThanHeader(
    const char * const a, const int64_t b) {
  return Loop< Headers >(headers()[a], GreaterThan< int64_t >(b));
}

bool TSImplementation::GreaterThanAfterHeader(const char * const a,
    const char * const b, const int64_t c) {
  return Loop< Headers >(headers()[a],
      GreaterThanAfter< int64_t >(b, strlen(b), c));
}

bool TSImplementation::LessThanHeader(
    const char * const a, const int64_t b) {
  return Loop< Headers >(headers()[a], LessThan< int64_t >(b));
}

bool TSImplementation::LessThanAfterHeader(const char * const a,
    const char * const b, const int64_t c) {
  return Loop< Headers >(headers()[a],
      LessThanAfter< int64_t >(b, strlen(b), c));
}

bool TSImplementation::StartsWithHeader(
    const char * const a, const char * const b, const uint32_t c) {
  return Loop< Headers >(headers()[a], StartsWith(b, strlen(b), c));
}

bool TSImplementation::IsScheme(
    const char * const a, const uint32_t b) {
  ASSERT(a != NULL);
  ASSERT(strlen(a) == b);
  ASSERT(buffer_ != NULL);
  ASSERT(location_ != NULL);
  int l = 0;
  const char * const m = TSUrlSchemeGet(buffer_, url(), &l);
  return b == static_cast< uint32_t >(l) && memcmp(m, a, b) == 0;
}

bool TSImplementation::IsMethod(
    const char * const a, const uint32_t b) const {
  ASSERT(a != NULL);
  ASSERT(strlen(a) == b);
  ASSERT(buffer_ != NULL);
  ASSERT(location_ != NULL);
  int l = 0;
  const char * const m = TSHttpHdrMethodGet(buffer_, location_, &l);
  return b == static_cast< uint32_t >(l) && memcmp(m, a, b) == 0;
}

bool TSImplementation::ContainsDomain(
    const char * const a, const uint32_t b) {
  ASSERT(a != NULL);
  ASSERT(strlen(a) >= b);
  ASSERT(buffer_ != NULL);
  ASSERT(location_ != NULL);
  int l = 0;
  const char * const begin = TSUrlHostGet(buffer_, url(), &l),
        * const end = begin + l;
  return std::search(begin, end, a, a + b) != end;
}

bool TSImplementation::EqualDomain(
    const char * const a, const uint32_t b) {
  ASSERT(a != NULL);
  ASSERT(strlen(a) >= b);
  ASSERT(buffer_ != NULL);
  ASSERT(location_ != NULL);
  int l = 0;
  const char * const m = TSUrlHostGet(buffer_, url(), &l);
  return b == static_cast< uint32_t >(l) && memcmp(m, a, b) == 0;
}

bool TSImplementation::StartsWithDomain(
    const char * const a, const uint32_t b, const uint32_t c) {
  ASSERT(a != NULL);
  ASSERT(strlen(a) >= b);
  ASSERT(buffer_ != NULL);
  ASSERT(location_ != NULL);
  int l = 0;
  const char * const begin = TSUrlHostGet(buffer_, url(), &l),
        * const end = begin + l;
  return std::search(begin, end, a, a + b) != begin + c;
}


bool TSImplementation::ContainsPath(
    const char * const a, const uint32_t b) {
  ASSERT(a != NULL);
  ASSERT(strlen(a) >= b);
  ASSERT(buffer_ != NULL);
  ASSERT(location_ != NULL);
  int l = 0;
  const char * const begin = TSUrlPathGet(buffer_, url(), &l),
        * const end = begin + l;
  return std::search(begin, end, a, a + b) != end;
}

bool TSImplementation::EqualPath(
    const char * const a, const uint32_t b) {
  ASSERT(a != NULL);
  ASSERT(strlen(a) >= b);
  ASSERT(buffer_ != NULL);
  ASSERT(location_ != NULL);
  int l = 0;
  const char * const m = TSUrlPathGet(buffer_, url(), &l);
  return b == static_cast< uint32_t >(l) && memcmp(m, a, b) == 0;
}

bool TSImplementation::StartsWithPath(
    const char * const a, const uint32_t b, const uint32_t c) {
  ASSERT(a != NULL);
  ASSERT(strlen(a) >= b);
  ASSERT(buffer_ != NULL);
  ASSERT(location_ != NULL);
  int l = 0;
  const char * const begin = TSUrlPathGet(buffer_, url(), &l),
        * const end = begin + l;
  return std::search(begin, end, a, a + b) == begin + c;
}


bool TSImplementation::ContainsQueryParameter(
    const char * const a, const char * const b) {
  return Loop< QueryParameters >(queryParameters()[a], Contains(b, strlen(b)));
}

bool TSImplementation::EqualQueryParameter(
    const char * const a, const char * const b) {
  return Loop< QueryParameters >(queryParameters()[a], Equal(b, strlen(b)));
}

bool TSImplementation::GreaterThanQueryParameter(
    const char * const a, const int64_t b) {
  return Loop< QueryParameters >(queryParameters()[a], GreaterThan< int64_t >(b));
}

bool TSImplementation::GreaterThanAfterQueryParameter(const char * const a,
    const char * const b, const int64_t c) {
  return Loop< QueryParameters >(queryParameters()[a],
      GreaterThanAfter< int64_t >(b, strlen(b), c));
}

bool TSImplementation::LessThanQueryParameter(
    const char * const a, const int64_t b) {
  return Loop< QueryParameters >(queryParameters()[a], LessThan< int64_t >(b));
}

bool TSImplementation::LessThanAfterQueryParameter(const char * const a,
    const char * const b, const int64_t c) {
  return Loop< QueryParameters >(queryParameters()[a],
      LessThanAfter< int64_t >(b, strlen(b), c));
}

bool TSImplementation::StartsWithQueryParameter(
    const char * const a, const char * const b, const uint32_t c) {
  return Loop< QueryParameters >(queryParameters()[a], StartsWith(b, strlen(b), c));
}


bool TSImplementation::ContainsCookie(
    const char * const a, const char * const b) {
  return Loop< Cookies >(cookies()[a], Contains(b, strlen(b)));
}

bool TSImplementation::EqualCookie(
    const char * const a, const char * const b) {
  return Loop< Cookies >(cookies()[a], Equal(b, strlen(b)));
}

bool TSImplementation::GreaterThanCookie(
    const char * const a, const int64_t b) {
  return Loop< Cookies >(cookies()[a], GreaterThan< int64_t >(b));
}

bool TSImplementation::GreaterThanAfterCookie(const char * const a,
    const char * const b, const int64_t c) {
  return Loop< Cookies >(cookies()[a],
      GreaterThanAfter< int64_t >(b, strlen(b), c));
}

bool TSImplementation::LessThanCookie(
    const char * const a, const int64_t b) {
  return Loop< Cookies >(cookies()[a], LessThan< int64_t >(b));
}

bool TSImplementation::LessThanAfterCookie(const char * const a,
    const char * const b, const int64_t c) {
  return Loop< Cookies >(cookies()[a],
      LessThanAfter< int64_t >(b, strlen(b), c));
}

bool TSImplementation::StartsWithCookie(
    const char * const a, const char * const b, const uint32_t c) {
  return Loop< Cookies >(cookies()[a], StartsWith(b, strlen(b), c));
}
} //end of filters namespace
} //end of http namespace
