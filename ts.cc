/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#include <iostream>

#include "ts.h"

namespace http {
namespace filters {

util::StringView getHeader(const TSMBuffer b, const TSMLoc l, const char * const h) {
  ASSERT(b != NULL);
  ASSERT(l != NULL);
  ASSERT(h != NULL);
  util::StringView result;
  const TSMLoc f = TSMimeHdrFieldFind(b, l, h, -1);
  if (f != TS_NULL_MLOC) {
    int length = 0;
    const char * const c = TSMimeHdrFieldValueStringGet(b, l, f, -1, &length);
    result = util::StringView(c, length);
    const TSReturnCode r = TSHandleMLocRelease(b, l, f);
    ASSERT(r == TS_SUCCESS);
  }
  return result;
}

Headers::Headers(const TSMBuffer & b, const TSMLoc & l) {
  TSMLoc location = TSMimeHdrFieldGet(b, l, 0);
  while (location != 0) {
    int length = 0;
    const char * const buffer = TSMimeHdrFieldNameGet(b, l, location, &length);
    ASSERT(buffer != NULL);
    if (buffer != NULL && length > 0) {
      Values & v = map_[util::StringView(buffer, length)];
      int length2 = 0;
      const char * const buffer2 = TSMimeHdrFieldValueStringGet(b, l, location, -1, &length2);
      ASSERT(buffer != NULL);
      if (buffer2 != NULL && length2 > 0) {
        v.push_back(util::StringView(buffer2, length2));
      }
    }
    {
      const TSMLoc next = TSMimeHdrFieldNext(b, l, location);
      const TSReturnCode r = TSHandleMLocRelease(b, l, location);
      ASSERT(r == TS_SUCCESS);
      location = next;
    }
  }
}

Headers::Result Headers::operator [] (
    const char * const a) const {
  const Map::const_iterator it = map_.find(
      util::StringView(a, strlen(a)));
  if (it != map_.end()) {
    return Result(&(it->second), true);
  }
  return Result(NULL, false);
}

void Headers::print(std::ostream & o) const {
  const Map::const_iterator end = map_.end();
  Map::const_iterator it = map_.begin();
  for (; it != end; ++it) {
    o << it->first.str();
    if ( ! it->second.empty()) {
      const Values::const_iterator end2 = it->second.end();
      Values::const_iterator it2 = it->second.begin();
      o << ": " << it2->str();
      for (++it2; it2 != end2; ++it2) {
        o << ", " << it2->str();
      }
    }
    o << "\n";
  }
}


Cookies::Cookies(const util::StringView & s) {
  if (s.pointer != NULL) {
    ASSERT(s.length > 0);
    parse(s);
  }
}

void Cookies::parse(const util::StringView & s) {
  const char * const begin = s,
        * const end = s + s.length,
        * i = begin,
        * j = i,
        * k = i;
  bool skipSpaces = false;
  for (; k != end; ++k) {
    if (skipSpaces) {
      if (*k == ' ') {
        continue;
      } else {
        skipSpaces = false;
        i = k;
      }
    }
    switch (*k) {
    case '=':
      if (j <= i) {
        j = k;
      }
      break;
    case ';':
      map_[util::StringView(i, j - i)]
        .push_back(util::StringView(j + 1, k - j - 1));
      skipSpaces = true;
      break;
    }
  }
  if (i < j) {
    map_[util::StringView(i, j - i)]
      .push_back(util::StringView(j + 1, k - j - 1));
  }
}

Cookies::Result Cookies::operator [] (
    const char * const a) const {
  const Map::const_iterator it = map_.find(
      util::StringView(a, strlen(a)));
  if (it != map_.end()) {
    return Result(&(it->second), true);
  }
  return Result(NULL, false);
}

QueryParameters::QueryParameters(const util::StringView & s) {
  if (s.pointer != NULL) {
    ASSERT(s.length > 0);
    parse(s);
  }
}

void QueryParameters::push(const char * const i,
    const char * & j, const char * const k) {
  ASSERT(i != NULL);
  ASSERT(j != NULL);
  ASSERT(k != NULL);
  if (j <= i) {
    j = k;
  }
  Values & v = map_[util::StringView(i, j - i)];
  if (j + 1 < k) {
    v.push_back(util::StringView(j + 1, k - j - 1));
  }
}

void QueryParameters::parse(const util::StringView & s) {
  const char * const begin = s,
        * const end = s + s.length,
        * i = begin,
        * j = i,
        * k = i;
  for (; k != end; ++k) {
    switch (*k) {
    case '=':
      if (j <= i) {
        j = k;
      }
      break;
    case '&':
      if (i < k) { push(i, j, k); }
      i = k + 1;
      break;
    }
  }
  if (i < k) { push(i, j, k); }
}

QueryParameters::Result QueryParameters::operator [] (
    const char * const a) const {
  const Map::const_iterator it = map_.find(
      util::StringView(a, strlen(a)));
  if (it != map_.end()) {
    return Result(&(it->second), true);
  }
  return Result(NULL, false);
}

} //end of filters namespace
} //end of http namespace
