/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#ifndef TS_IMPL_H
#define TS_IMPL_H

#include <string>
#include <cstring>

#include <ts/ts.h>

#include "base-impl.h"
#include "string-view.h"
#include "ts.h"

namespace http {
namespace filters {

struct TSImplementation : BaseImplementation {
  std::string tag_;
  TSMBuffer buffer_;
  TSMLoc location_;
  TSMLoc url_;
  Headers headers_;
  QueryParameters queryParameters_;
  Cookies cookies_;

  ~TSImplementation() {
    ASSERT(buffer_ != NULL);
    ASSERT(location_ != NULL);
    if (url_ != NULL) {
      TSHandleMLocRelease(buffer_, location_, url_);
    }
  }

  TSImplementation(const char * const t, const TSMBuffer & b, const TSMLoc & l) :
    tag_(t), buffer_(b), location_(l), url_(NULL) { }

  inline Headers & headers(void) {
    if (headers_.empty()) {
      headers_ = Headers(buffer_, location_);
    }
    return headers_;
  }

  inline Cookies & cookies(void) {
    if (cookies_.empty()) {
      Headers::Result r = headers()["Cookie"];
      if (r.second && ! r.first->empty()) {
        cookies_ = Cookies((*r.first)[0]);
      }
    }
    return cookies_;
  }

  inline QueryParameters & queryParameters(void) {
    if (queryParameters_.empty()) {
      queryParameters_ = QueryParameters(queryParameter(buffer_));
    }
    return queryParameters_;
  }

  bool PrintError(const char * const c, const char * const l) const {
    TSError("[%s] %s\n", strlen(l) > 0 ? l : tag_.c_str(), c);
    return true;
  }

  bool PrintDebug(const char * const c, const char * const l) const {
    TSDebug(strlen(l) > 0 ? l : tag_.c_str(), "%s", c);
    return true;
  }

  bool IsMethod(const char * const, const uint32_t) const;

  inline TSMLoc & url(void) {
    if (url_ == NULL) {
      TSHttpHdrUrlGet(buffer_, location_, &url_);
      ASSERT(url_ != NULL);
    }
    return url_;
  }

  bool IsScheme(const char * const, const uint32_t);

  bool ContainsDomain(const char * const, const uint32_t);
  bool EqualDomain(const char * const, const uint32_t);
  bool StartsWithDomain(const char * const, const uint32_t, const uint32_t);

  inline bool NotEqualDomain(const char * const a, const uint32_t b) {
    return ! EqualDomain(a, b);
  }

  bool ContainsPath(const char * const, const uint32_t);
  bool EqualPath(const char * const, const uint32_t);
  bool StartsWithPath(const char * const, const uint32_t, const uint32_t);

  inline bool NotEqualPath(const char * const a, const uint32_t b) {
    return ! EqualPath(a, b);
  }

  util::StringView queryParameter(const TSMBuffer & b) {
    int length = 0;
    const char * const pointer = TSUrlHttpQueryGet(buffer_, url(), &length);
    return util::StringView(pointer, length);
  }

  bool ContainsQueryParameter(const char * const, const char * const);
  bool EqualQueryParameter(const char * const, const char * const);

  inline bool ExistsQueryParameter(const char * const a) {
    return queryParameters()[a].second;
  }

  bool GreaterThanQueryParameter(const char * const, const int64_t);
  bool GreaterThanAfterQueryParameter(const char * const, const char * const, const int64_t);
  bool LessThanQueryParameter(const char * const, const int64_t);
  bool LessThanAfterQueryParameter(const char * const, const char * const, const int64_t);
  bool StartsWithQueryParameter(const char * const, const char * const, const uint32_t);

  inline bool NotEqualQueryParameter(const char * const a, const char * const b) {
    return ExistsQueryParameter(a) && ! EqualQueryParameter(a, b);
  }

  bool ContainsHeader(const char * const, const char * const);
  bool EqualHeader(const char * const, const char * const);

  inline bool ExistsHeader(const char * const a) {
    return headers()[a].second;
  }

  bool GreaterThanHeader(const char * const, const int64_t);
  bool GreaterThanAfterHeader(const char * const, const char * const, const int64_t);
  bool LessThanHeader(const char * const, const int64_t);
  bool LessThanAfterHeader(const char * const, const char * const, const int64_t);
  bool StartsWithHeader(const char * const, const char * const, const uint32_t);

  inline bool NotEqualHeader(const char * const a, const char * const b) {
    return ExistsHeader(a) && ! EqualHeader(a, b);
  }

  bool ContainsCookie(const char * const, const char * const);
  bool EqualCookie(const char * const, const char * const);

  inline bool ExistsCookie(const char * const a) {
    return cookies()[a].second;
  }

  bool GreaterThanCookie(const char * const, const int64_t);
  bool GreaterThanAfterCookie(const char * const, const char * const, const int64_t);
  bool LessThanCookie(const char * const, const int64_t);
  bool LessThanAfterCookie(const char * const, const char * const, const int64_t);
  bool StartsWithCookie(const char * const, const char * const, const uint32_t);

  inline bool NotEqualCookie(const char * const a, const char * const b) {
    return ExistsCookie(a) && ! EqualCookie(a, b);
  }

};
} //end of filters namespace
} //end of http namespace

#endif //TS_IMPL_H
