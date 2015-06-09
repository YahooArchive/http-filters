/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#include <iomanip>
#include <iostream>
#include <sstream>

#include "vm-printer.h"

namespace http {
namespace filters {
namespace vm {
void Printer::print(const Code & co, const Memory & m) const {
  print(co, m, std::cout);
}

void Printer::print(const Code & co, const Memory & m,
    std::ostream & o) const {

  typedef std::pair< uint32_t, int > Pair;
  typedef std::vector< Pair > Entries;
  int z = 1;

  Entries entries1,
          entries2;

  for (uint32_t i = 0; i < co.size; i += kSize) {
    const uint32_t op = co[i];
    if (op == Opcodes::kExecute
        || op == Opcodes::kExecuteSingle) {
      const uint32_t b = co[i + 2];
      entries1.push_back(std::make_pair(b * kSize, z));
      entries2.push_back(std::make_pair(i, z));
      ++z;
    }
  }

  const Entries::const_iterator END1 = entries1.end(),
        END2 = entries2.end();

  Entries::const_iterator it1 = entries1.begin(),
    it2 = entries2.begin();

  ASSERT(entries1.size() == entries2.size());

  for (uint32_t i = 0; i < co.size; i += kSize) {
    const uint32_t op = co[i];
    const uint32_t a = co[i + 1];
    const uint32_t b = co[i + 2];
    const uint32_t c = co[i + 3];

    if (it1 != END1 && it1->first == i) {
      o << "\n" " -- Entry " << it1->second << " --" "\n";
      ++it1;
    }

    o << std::resetiosflags(std::ios::left) << std::setiosflags(std::ios::right)
      << std::setw(6) << std::dec << (i / kSize) << " "
      << std::resetiosflags(std::ios::right) << std::setiosflags(std::ios::left)
      << "0x" << std::setw(6) << std::hex << std::setfill('0') <<  (i * sizeof(uint32_t)) << std::setfill(' ') << " "
      << std::setw(30) << opcode(static_cast< Opcodes::OPCODES >(op))
      << "0x" << std::setw(8) << a
      << "0x" << std::setw(8) << b
      << "0x" << std::setw(8) << c;

    std::stringstream comments;

    if (op == Opcodes::kExecute || op == Opcodes::kExecuteSingle) {
      switch(a) {
      case ExecutionMode::kNone:
        comments << ( ! comments.str().empty() ? ", " : "") << " kNone";
        break;
      case ExecutionMode::kAnd:
        comments << ( ! comments.str().empty() ? ", " : "") << " kAnd";
        break;
      case ExecutionMode::kOr:
        comments << ( ! comments.str().empty() ? ", " : "") << " kOr";
        break;
      default: ASSERT(false); break; //unreacheable
      }
    }

    if (it2 != END2 && it2->first == i) {
      ASSERT(op == Opcodes::kExecute
          || op == Opcodes::kExecuteSingle);
      comments << ( ! comments.str().empty() ? ", " : "") << "Entry " << it2->second;
      ++it2;
    }

    if ( ! comments.str().empty()) {
      o << " //" << comments.str();
      comments.str().clear();
    }

    o << "\n";

    switch (op) {
      case Opcodes::kContainsCookie:
      case Opcodes::kContainsDomain:
      case Opcodes::kContainsHeader:
      case Opcodes::kContainsPath:
      case Opcodes::kContainsQueryParameter:
      case Opcodes::kEqualCookie:
      case Opcodes::kEqualDomain:
      case Opcodes::kEqualHeader:
      case Opcodes::kEqualPath:
      case Opcodes::kEqualQueryParameter:
      case Opcodes::kExistsCookie:
      case Opcodes::kExistsHeader:
      case Opcodes::kExistsQueryParameter:
      case Opcodes::kGreaterThanAfterCookie:
      case Opcodes::kGreaterThanAfterHeader:
      case Opcodes::kGreaterThanAfterQueryParameter:
      case Opcodes::kGreaterThanCookie:
      case Opcodes::kGreaterThanHeader:
      case Opcodes::kGreaterThanQueryParameter:
      case Opcodes::kIsMethod:
      case Opcodes::kIsScheme:
      case Opcodes::kLessThanAfterCookie:
      case Opcodes::kLessThanAfterHeader:
      case Opcodes::kLessThanAfterQueryParameter:
      case Opcodes::kLessThanCookie:
      case Opcodes::kLessThanHeader:
      case Opcodes::kLessThanQueryParameter:
      case Opcodes::kNotEqualDomain:
      case Opcodes::kNotEqualPath:
      case Opcodes::kNotEqualCookie:
      case Opcodes::kNotEqualHeader:
      case Opcodes::kNotEqualQueryParameter:
      case Opcodes::kPrintDebug:
      case Opcodes::kPrintError:
        if (a != 0) {
          ASSERT(a < m.size);
          o << " -> \"" << m.t + a << "\"\n";
        }
        break;

      default:
        break;
    }

    switch (op) {
      case Opcodes::kContainsCookie:
      case Opcodes::kContainsHeader:
      case Opcodes::kContainsQueryParameter:
      case Opcodes::kEqualCookie:
      case Opcodes::kEqualHeader:
      case Opcodes::kEqualQueryParameter:
      case Opcodes::kGreaterThanAfterCookie:
      case Opcodes::kGreaterThanAfterHeader:
      case Opcodes::kGreaterThanAfterQueryParameter:
      case Opcodes::kLessThanAfterCookie:
      case Opcodes::kLessThanAfterHeader:
      case Opcodes::kLessThanAfterQueryParameter:
      case Opcodes::kLessThanQueryParameter:
      case Opcodes::kNotEqualCookie:
      case Opcodes::kNotEqualHeader:
      case Opcodes::kNotEqualQueryParameter:
      case Opcodes::kPrintDebug:
      case Opcodes::kPrintError:
        if (b != 0) {
          ASSERT(b < m.size);
          o << " -> \"" << m.t + b << "\"\n";
        }
        break;

      default:
        break;
    }
  }

  o << std::flush;
}

const char * Printer::opcode(Opcodes::OPCODES op) {
  switch (op) {
  case Opcodes::kNull:
    return "kNull"; break;

  case Opcodes::kSkip:
    return "kSkip"; break;
  case Opcodes::kExecute:
    return "kExecute"; break;
  case Opcodes::kExecuteSingle:
    return "kExecuteSingle"; break;
  case Opcodes::kReturn:
    return "kReturn"; break;
  case Opcodes::kHalt:
    return "kHalt"; break;

  case Opcodes::kNone:
    return "kNone"; break;
  case Opcodes::kAnd:
    return "kAnd"; break;
  case Opcodes::kOr:
    return "kOr"; break;
  case Opcodes::kNot:
    return "kNot"; break;

  case Opcodes::kFalse:
    return "kFalse"; break;
  case Opcodes::kTrue:
    return "kTrue"; break;

  case Opcodes::kPrintError:
    return "kPrintError"; break;
  case Opcodes::kPrintDebug:
    return "kPrintDebug"; break;

  case Opcodes::kIsMethod:
    return "kIsMethod"; break;
  case Opcodes::kIsScheme:
    return "kIsScheme"; break;

  case Opcodes::kContainsDomain:
    return "kContainsDomain"; break;
  case Opcodes::kEqualDomain:
    return "kEqualDomain"; break;
  case Opcodes::kNotEqualDomain:
    return "kNotEqualDomain"; break;
  case Opcodes::kStartsWithDomain:
    return "kStartsWithDomain"; break;

  case Opcodes::kContainsPath:
    return "kContainsPath"; break;
  case Opcodes::kEqualPath:
    return "kEqualPath"; break;
  case Opcodes::kNotEqualPath:
    return "kNotEqualPath"; break;
  case Opcodes::kStartsWithPath:
    return "kStartsWithPath"; break;

  case Opcodes::kContainsQueryParameter:
    return "kContainsQueryParameter"; break;
  case Opcodes::kEqualQueryParameter:
    return "kEqualQueryParameter"; break;
  case Opcodes::kExistsQueryParameter:
    return "kExistsQueryParameter"; break;
  case Opcodes::kGreaterThanQueryParameter:
    return "kGreaterThanQueryParameter"; break;
  case Opcodes::kGreaterThanAfterQueryParameter:
    return "kGreaterThanAfterQueryParameter"; break;
  case Opcodes::kLessThanQueryParameter:
    return "kLessThanQueryParameter"; break;
  case Opcodes::kLessThanAfterQueryParameter:
    return "kLessThanAfterQueryParameter"; break;
  case Opcodes::kNotEqualQueryParameter:
    return "kNotEqualQueryParameter"; break;
  case Opcodes::kStartsWithQueryParameter:
    return "kStartsWithQueryParameter"; break;

  case Opcodes::kContainsHeader:
    return "kContainsHeader"; break;
  case Opcodes::kEqualHeader:
    return "kEqualHeader"; break;
  case Opcodes::kExistsHeader:
    return "kExistsHeader"; break;
  case Opcodes::kGreaterThanHeader:
    return "kGreaterThanHeader"; break;
  case Opcodes::kGreaterThanAfterHeader:
    return "kGreaterThanAfterHeader"; break;
  case Opcodes::kLessThanHeader:
    return "kLessThanHeader"; break;
  case Opcodes::kLessThanAfterHeader:
    return "kLessThanAfterHeader"; break;
  case Opcodes::kNotEqualHeader:
    return "kNotEqualHeader"; break;
  case Opcodes::kStartsWithHeader:
    return "kStartsWithHeader"; break;

  case Opcodes::kContainsCookie:
    return "kContainsCookie"; break;
  case Opcodes::kEqualCookie:
    return "kEqualCookie"; break;
  case Opcodes::kExistsCookie:
    return "kExistsCookie"; break;
  case Opcodes::kGreaterThanCookie:
    return "kGreaterThanCookie"; break;
  case Opcodes::kGreaterThanAfterCookie:
    return "kGreaterThanAfterCookie"; break;
  case Opcodes::kLessThanCookie:
    return "kLessThanCookie"; break;
  case Opcodes::kLessThanAfterCookie:
    return "kLessThanAfterCookie"; break;
  case Opcodes::kNotEqualCookie:
    return "kNotEqualCookie"; break;
  case Opcodes::kStartsWithCookie:
    return "kStartsWithCookie"; break;

  case Opcodes::kUpperBound:
    return "kUpperBound"; break;

  default: return "(Unknown)"; break;
  }
}
} //end of vm namespace
} //end of filters namespace
} //end of http namespace
