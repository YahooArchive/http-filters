/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#ifndef COMPILER_H
#define COMPILER_H

#include <vector>

#include "my-assert.h"

#include "assembler.h"
#include "representation.h"

namespace http {
namespace filters {
typedef std::vector< uint32_t > Offsets;

struct Compiler {
  Assembler assembler_;

  Compiler(void);

  void compile(const Forest &, Offsets &);

  inline uint32_t compile(const Tree & t) {
    if (t.root() == NULL) {
      return 0;
    }
    const uint32_t result = compileSimple(t.root());
    assembler_.pushHalt();
    return result;
  }

  uint32_t compileSimple(const Node *);

  void dispatch(const Node * const);

  static inline void PushAnd(Assembler & a,
      const Op::Parameters & o = Op::Parameters()) { a.pushAnd(); }
  static inline void PushFalse(Assembler & a,
      const Op::Parameters & o = Op::Parameters()) { a.pushFalse(); }
  static inline void PushHalt(Assembler & a,
      const Op::Parameters & o = Op::Parameters()) { a.pushHalt(); }
  static inline void PushNone(Assembler & a,
      const Op::Parameters & o = Op::Parameters()) { a.pushNone(); }
  static inline void PushNot(Assembler & a,
      const Op::Parameters & o = Op::Parameters()) { a.pushNot(); }
  static inline void PushOr(Assembler & a,
      const Op::Parameters & o = Op::Parameters()) { a.pushOr(); }
  static inline void PushReturn(Assembler & a,
      const Op::Parameters & o = Op::Parameters()) { a.pushReturn(); }
  static inline void PushTrue(Assembler & a,
      const Op::Parameters & o = Op::Parameters()) { a.pushTrue(); }

  static inline void PushIsMethod(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 1);
    a.pushIsMethod(p[0].c_str(), p[0].size());
  }

  static inline void PushIsScheme(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 1);
    a.pushIsScheme(p[0].c_str(), p[0].size());
  }

  static inline void PushContainsDomain(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 1);
    a.pushContainsDomain(p[0].c_str(), p[0].size());
  }

  static inline void PushEqualDomain(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 1);
    a.pushEqualDomain(p[0].c_str(), p[0].size());
  }

  static inline void PushNotEqualDomain(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 1);
    a.pushNotEqualDomain(p[0].c_str(), p[0].size());
  }

  static inline void PushStartsWithDomain(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 1);
    a.pushStartsWithDomain(p[0].c_str(), p[0].size(), 0);
  }

  static inline void PushStartsWithPath(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 1);
    a.pushStartsWithPath(p[0].c_str(), p[0].size(), 0);
  }

  static inline void PushContainsPath(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 1);
    a.pushContainsPath(p[0].c_str(), p[0].size());
  }

  static inline void PushEqualPath(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 1);
    a.pushEqualPath(p[0].c_str(), p[0].size());
  }

  static inline void PushNotEqualPath(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 1);
    a.pushNotEqualPath(p[0].c_str(), p[0].size());
  }

  static inline void PushContainsHeader(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushContainsHeader(p[0].c_str(), p[1].c_str());
  }

  static inline void PushEqualHeader(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushEqualHeader(p[0].c_str(), p[1].c_str());
  }

  static inline void PushExistsHeader(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 1);
    a.pushExistsHeader(p[0].c_str());
  }

  static inline void PushGreaterThanHeader(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushGreaterThanHeader(p[0].c_str(), atoi(p[1].c_str()));
  }

  static inline void PushGreaterThanAfterHeader(
      Assembler & a, const Op::Parameters & p) {
    ASSERT(p.size() == 3);
    a.pushGreaterThanAfterHeader(p[0].c_str(), p[1].c_str(), atoi(p[2].c_str()));
  }

  static inline void PushLessThanHeader(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushLessThanHeader(p[0].c_str(), atoi(p[1].c_str()));
  }

  static inline void PushLessThanAfterHeader(
      Assembler & a, const Op::Parameters & p) {
    ASSERT(p.size() == 3);
    a.pushLessThanAfterHeader(p[0].c_str(), p[1].c_str(), atoi(p[2].c_str()));
  }

  static inline void PushGreaterThanCookie(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushGreaterThanCookie(p[0].c_str(), atoi(p[1].c_str()));
  }

  static inline void PushGreaterThanAfterCookie(
      Assembler & a, const Op::Parameters & p) {
    ASSERT(p.size() == 3);
    a.pushGreaterThanAfterCookie(p[0].c_str(), p[1].c_str(), atoi(p[2].c_str()));
  }

  static inline void PushLessThanCookie(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushLessThanCookie(p[0].c_str(), atoi(p[1].c_str()));
  }

  static inline void PushLessThanAfterCookie(
      Assembler & a, const Op::Parameters & p) {
    ASSERT(p.size() == 3);
    a.pushLessThanAfterCookie(p[0].c_str(), p[1].c_str(), atoi(p[2].c_str()));
  }

  static inline void PushGreaterThanQueryParameter(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushGreaterThanQueryParameter(p[0].c_str(), atoi(p[1].c_str()));
  }

  static inline void PushGreaterThanAfterQueryParameter(
      Assembler & a, const Op::Parameters & p) {
    ASSERT(p.size() == 3);
    a.pushGreaterThanAfterQueryParameter(p[0].c_str(), p[1].c_str(), atoi(p[2].c_str()));
  }

  static inline void PushLessThanQueryParameter(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushLessThanQueryParameter(p[0].c_str(), atoi(p[1].c_str()));
  }

  static inline void PushLessThanAfterQueryParameter(
      Assembler & a, const Op::Parameters & p) {
    ASSERT(p.size() == 3);
    a.pushLessThanAfterQueryParameter(p[0].c_str(), p[1].c_str(), atoi(p[2].c_str()));
  }

  static inline void PushNotEqualHeader(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushNotEqualHeader(p[0].c_str(), p[1].c_str());
  }

  static inline void PushNotEqualQueryParameter(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushNotEqualQueryParameter(p[0].c_str(), p[1].c_str());
  }

  static inline void PushContainsCookie(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushContainsCookie(p[0].c_str(), p[1].c_str());
  }

  static inline void PushContainsQueryParameter(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushContainsQueryParameter(p[0].c_str(), p[1].c_str());
  }

  static inline void PushExistsQueryParameter(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 1);
    a.pushExistsQueryParameter(p[0].c_str());
  }

  static inline void PushEqualQueryParameter(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushEqualQueryParameter(p[0].c_str(), p[1].c_str());
  }

  static inline void PushNotEqualCookie(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushNotEqualCookie(p[0].c_str(), p[1].c_str());
  }

  static inline void PushEqualCookie(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushEqualCookie(p[0].c_str(), p[1].c_str());
  }

  static inline void PushExistsCookie(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 1);
    a.pushExistsCookie(p[0].c_str());
  }

  static inline void PushStartsWithHeader(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushStartsWithHeader(p[0].c_str(), p[1].c_str(), 0);
  }

  static inline void PushStartsWithQueryParameter(Assembler & a,
      const Op::Parameters & p) {
    ASSERT(p.size() == 2);
    a.pushStartsWithQueryParameter(p[0].c_str(), p[1].c_str(),
        0);
  }

  static inline void PushExecute(Assembler & a, const ExecutionMode::MODES m,
      const uint32_t o, const uint32_t c = 0) {
    ASSERT(m == ExecutionMode::kNone
        || m == ExecutionMode::kAnd
        || m == ExecutionMode::kOr);
    ASSERT(o < a.codeSize());
    a.pushExecute(m, o, c);
  }

  static void PushPrintError(Assembler &, const Op::Parameters &);

  static void PushPrintDebug(Assembler &, const Op::Parameters &);
};
} //end of filters namespace
} //end of http namespace

#endif //COMPILER_H
