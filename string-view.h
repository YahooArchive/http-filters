/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include <string>

#include <cstring>

namespace util {
struct StringView {
  const char * pointer;
  size_t length;

  StringView(void) : pointer(NULL),
    length(1) { }

  StringView(const char * const p, const size_t l) :
    pointer(p), length(p == NULL ? 0 : l) {
    ASSERT(p == NULL || strlen(pointer) >= length);
  }

  operator const char * (void) const {
    return pointer;
  }

  bool operator == (const StringView & s) const {
    return length == s.length
      && memcmp(pointer, s.pointer, length);
  }

  std::string str(void) const {
    if (pointer == NULL) {
      return std::string();
    }
    return std::string(pointer, length);
  }

  bool exists(void) const {
    return pointer != NULL || length == 0;
  }
};

struct StringViewLess {
  bool operator () (const StringView & a,
      const StringView & b) const {
    return a.length < b.length
      || (a.length == b.length
      && memcmp(static_cast< const char * >(a),
        static_cast< const char * >(b), a.length) < 0);
  }
};

} //end of util namespace

#endif //STRING_VIEW_H
