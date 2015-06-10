/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#ifndef BASE_IMPL_H
#define BASE_IMPL_H

#include <stdint.h>

namespace http {
namespace filters {

struct BaseImplementation {

  bool PrintError(const char * const, const char * const) const { return true; }
  bool PrintDebug(const char * const, const char * const) const { return true; }

  bool IsMethod(const char * const, const uint32_t) { return true; }
  bool IsScheme(const char * const, const uint32_t) { return true; }

  bool ContainsDomain(const char * const, const uint32_t) { return true; }
  bool EqualDomain(const char * const, const uint32_t) { return true; }
  bool NotEqualDomain(const char * const, const uint32_t) { return true; }
  bool StartsWithDomain(const char * const, const uint32_t, const uint32_t) { return true; }

  bool ContainsPath(const char * const, const uint32_t) { return true; }
  bool EqualPath(const char * const, const uint32_t) { return true; }
  bool NotEqualPath(const char * const, const uint32_t) { return true; }
  bool StartsWithPath(const char * const, const uint32_t, const uint32_t) { return true; }

  bool ContainsQueryParameter(const char * const, const char * const) {return true; }
  bool EqualQueryParameter(const char * const, const char * const) { return true; }
  bool ExistsQueryParameter(const char * const) { return true; }
  bool GreaterThanQueryParameter(const char * const, const int64_t) { return true; }
  bool GreaterThanAfterQueryParameter(const char * const, const char * const, const int64_t) { return true; }
  bool LessThanQueryParameter(const char * const, const int64_t) { return true; }
  bool LessThanAfterQueryParameter(const char * const, const char * const, const int64_t) { return true; }
  bool NotEqualQueryParameter(const char * const, const char * const) { return true; }
  bool StartsWithQueryParameter(const char * const, const char * const, const uint32_t) { return true; }

  bool ContainsHeader(const char * const, const char * const) { return true; }
  bool EqualHeader(const char * const, const char * const) { return true; }
  bool ExistsHeader(const char * const) { return true; }
  bool GreaterThanHeader(const char * const, const int64_t) { return true; }
  bool GreaterThanAfterHeader(const char * const, const char * const, const int64_t) { return true; }
  bool LessThanHeader(const char * const, const int64_t) { return true; }
  bool LessThanAfterHeader(const char * const, const char * const, const int64_t) { return true; }
  bool NotEqualHeader(const char * const, const char * const) { return true; }
  bool StartsWithHeader(const char * const, const char * const, const uint32_t) { return true; }

  bool ContainsCookie(const char * const, const char * const) { return true; }
  bool EqualCookie(const char * const, const char * const) { return true; }
  bool ExistsCookie(const char * const) { return true; }
  bool GreaterThanCookie(const char * const, const int64_t) { return true; }
  bool GreaterThanAfterCookie(const char * const, const char * const, const int64_t) { return true; }
  bool LessThanCookie(const char * const, const int64_t) { return true; }
  bool LessThanAfterCookie(const char * const, const char * const, const int64_t) { return true; }
  bool NotEqualCookie(const char * const, const char * const) { return true; }
};

} //end of filters namespace
} //end of ats namespace

#endif //BASE_IMPL_H
