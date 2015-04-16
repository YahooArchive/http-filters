/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#include "my-assert.h"
#include <iostream>

#include "compiler.h"

namespace http {
namespace filters {

Compiler::Compiler(void) {
  assembler_.pushSkip();
}
void Compiler::compile(const Forest & f, Offsets & r) {
  typedef Forest::const_iterator Iterator;
  const Iterator END = f.end();
  for (Iterator it = f.begin(); it != END; ++it) {
    r.push_back(compile(*it));
  }
}

uint32_t Compiler::compileSimple(const Node * const n) {
  typedef std::pair< const Node *, uint32_t > Pair;
  typedef std::vector< Pair > Offsets;
  Offsets offsets;
  ASSERT(n != NULL);

  const Node * i = n,
        * j = i->next;

  while (i != NULL) {
    if (i->hasChild()) {
      const int nodeType = i->type();
      const BinaryNode * k = dynamic_cast< const BinaryNode * >(i);
      ASSERT(k != NULL);
      ASSERT(nodeType == NodeTypes::kAnd
          || nodeType == NodeTypes::kOr);
      ASSERT(k->child != NULL);
      offsets.push_back(std::make_pair(i,
            compileSimple(k->child)));
    }
    i = j;
    if (i != NULL) { j = i->next; }
  }

  i = n;
  j = i->next;

  const uint32_t e = assembler_.codeSize();
  Offsets::const_iterator it = offsets.begin();

  while (i != NULL) {
    const int nodeType = i->type();
    if (i->hasChild()) {
      ASSERT(it != offsets.end());
      ASSERT(it->first == i);
      ExecutionMode::MODES m = ExecutionMode::kNone;
      switch (nodeType) {
      case NodeTypes::kAnd: m = ExecutionMode::kAnd; break;
      case NodeTypes::kOr: m = ExecutionMode::kOr; break;
      default: ASSERT(false); break; //unrecheable
      }

      PushExecute(assembler_, m, it->second);
      ++it;
    } else {
      switch (nodeType) {
      case NodeTypes::kNot: PushNot(assembler_); break;
      case NodeTypes::kOp: dispatch(i); break;
      default: ASSERT(false); break; //unrecheable
      }
    }
    i = j;
    if (i != NULL) { j = i->next; }
  }
  PushReturn(assembler_);
  return e;
}

struct X {
  const char * const a;
  void (*b) (Assembler &, const Op::Parameters &);
  inline bool operator < (const std::string & n) const {
    return strcmp(a, n.c_str()) < 0;
  }
  inline bool operator == (const std::string & n) const {
    return strcmp(a, n.c_str()) == 0;
  }
};

void Compiler::dispatch(const Node * const n) {
  ASSERT(n != NULL);
  ASSERT(n->type() > NodeTypes::kUndefined);
  ASSERT(n->type() < NodeTypes::kUpperBound);
  const Op * const o = dynamic_cast< const Op * const >(n);
  ASSERT(o != NULL);

  static const X x [] = {
    { "containsCookie", &Compiler::PushContainsCookie },
    { "containsDomain", &Compiler::PushContainsDomain },
    { "containsHeader", &Compiler::PushContainsHeader },
    { "containsPath", &Compiler::PushContainsPath },
    { "containsQueryParameter", &Compiler::PushContainsQueryParameter },
    { "equalCookie", &Compiler::PushEqualCookie },
    { "equalDomain", &Compiler::PushEqualDomain },
    { "equalHeader", &Compiler::PushEqualHeader },
    { "equalPath", &Compiler::PushEqualPath },
    { "equalQueryParameter", &Compiler::PushEqualQueryParameter },
    { "existsCookie", &Compiler::PushExistsCookie },
    { "existsHeader", &Compiler::PushExistsHeader },
    { "existsQueryParameter", &Compiler::PushExistsQueryParameter },
    { "false", &Compiler::PushFalse },
    { "greaterThanAfterCookie", &Compiler::PushGreaterThanAfterCookie },
    { "greaterThanAfterHeader", &Compiler::PushGreaterThanAfterHeader },
    { "greaterThanAfterQueryParameter", &Compiler::PushGreaterThanAfterQueryParameter },
    { "greaterThanCookie", &Compiler::PushGreaterThanCookie },
    { "greaterThanHeader", &Compiler::PushGreaterThanHeader },
    { "greaterThanQueryParameter", &Compiler::PushGreaterThanQueryParameter },
    { "isMethod", &Compiler::PushIsMethod },
    { "isScheme", &Compiler::PushIsScheme },
    { "lessThanAfterCookie", &Compiler::PushLessThanAfterCookie },
    { "lessThanAfterHeader", &Compiler::PushLessThanAfterHeader },
    { "lessThanAfterQueryParameter", &Compiler::PushLessThanAfterQueryParameter },
    { "lessThanCookie", &Compiler::PushLessThanCookie },
    { "lessThanHeader", &Compiler::PushLessThanHeader },
    { "lessThanQueryParameter", &Compiler::PushLessThanQueryParameter },
    { "notEqualCookie", &Compiler::PushNotEqualCookie },
    { "notEqualDomain", &Compiler::PushNotEqualDomain },
    { "notEqualHeader", &Compiler::PushNotEqualHeader },
    { "notEqualPath", &Compiler::PushNotEqualPath },
    { "notEqualQueryParameter", &Compiler::PushNotEqualQueryParameter },
    { "printDebug", &Compiler::PushPrintDebug },
    { "printError", &Compiler::PushPrintError },
    { "true", &Compiler::PushTrue },
  };

  static const X * const begin = &x[0],
               * const end = &x[ARRAY_SIZE(x)];

  const X * const i = std::lower_bound(begin, end, o->name);

  ASSERT(i >= begin);

  if (i < end && *i == o->name) {
    (*(i->b))(assembler_, o->parameters);
  } else {
    std::cerr << o->name << "\n";
    ASSERT(false); //unsupported.
  }
}

void Compiler::PushPrintError(Assembler & a,
    const Op::Parameters & p) {
  ASSERT( ! p.empty());

  switch (p.size()) {
  case 1:
    a.pushPrintError(p[0].c_str());
    break;
  case 2:
    a.pushPrintError(p[0].c_str(), p[1].c_str());
    break;
  case 3:
    {
      const std::string & s = p[2];
      if (s == "true" || s == "True" || s == "TRUE") {
        a.pushPrintError(p[0].c_str(), p[1].c_str(), ExecutionMode::kAnd);
      } else if (s == "false" || s == "False" || s == "FALSE") {
        a.pushPrintError(p[0].c_str(), p[1].c_str(), ExecutionMode::kOr);
      } else {
        ASSERT(false); //unsupported
      }
    }
    break;
  default:
    ASSERT(false); break; //unsupported
  }
}

void Compiler::PushPrintDebug(Assembler & a,
    const Op::Parameters & p) {
  ASSERT( ! p.empty());

  switch (p.size()) {
  case 1:
    a.pushPrintDebug(p[0].c_str());
    break;
  case 2:
    a.pushPrintDebug(p[0].c_str(), p[1].c_str());
    break;
  case 3:
    {
      const std::string & s = p[2];
      if (s == "true" || s == "True" || s == "TRUE") {
        a.pushPrintDebug(p[0].c_str(), p[1].c_str(), ExecutionMode::kAnd);
      } else if (s == "false" || s == "False" || s == "FALSE") {
        a.pushPrintDebug(p[0].c_str(), p[1].c_str(), ExecutionMode::kOr);
      } else {
        ASSERT(false); //unsupported
      }
    }
    break;
  default:
    ASSERT(false); break; //unsupported
  }
}

} //end of filters namespace
} //end of http namespace
