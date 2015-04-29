/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#ifndef TS_H
#define TS_H

#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "my-assert.h"

#include <ts/ts.h>

#include "string-view.h"

namespace http {
namespace filters {

util::StringView getHeader(const TSMBuffer, const TSMLoc, const char * const);

struct Headers {
  typedef std::vector< util::StringView > Values;
  typedef std::map< util::StringView, Values, util::StringViewLess > Map;
  typedef std::pair< const Values *, bool > Result;

  Map map_;

  Headers(void) { }
  Headers(const TSMBuffer &, const TSMLoc &);
  inline bool empty(void) const { return map_.empty(); }
  Result operator [] (const char * const) const;
  void print(std::ostream &) const;
};

struct Cookies {
  typedef std::vector< util::StringView > Values;
  typedef std::map< util::StringView, Values, util::StringViewLess > Map;
  typedef std::pair< const Values *, bool > Result;

  Map map_;

  Cookies(void) { }
  Cookies(const util::StringView &);
  void parse(const util::StringView &);
  inline bool empty(void) const { return map_.empty(); }
  Result operator [] (const char * const) const;
  void push(const char * const, const char * &, const char * const);
};

struct QueryParameters {
  typedef std::vector< util::StringView > Values;
  typedef std::map< util::StringView, Values, util::StringViewLess > Map;
  typedef std::pair< const Values * , bool > Result;

  Map map_;

  QueryParameters(void) { }
  QueryParameters(const util::StringView &);
  void parse(const util::StringView &);
  inline bool empty(void) const { return map_.empty(); }
  Result operator [] (const char * const) const;
  void push(const char * const, const char * &, const char * const);
};

} //end of filters namespace
} //end of http namespace

#endif //TS_H
