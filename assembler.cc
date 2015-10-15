/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#include <stdexcept>

#include <cstring>

#include "my-assert.h"

#include "assembler.h"
#include "opcodes.h"

namespace http {
namespace filters {

struct MapFinder {
  const char * const key_;
  MapFinder(const char * const k) : key_(k) { }
  bool operator ()(const Labels::value_type & v) const {
    return strcmp(v.first.c_str(), key_) == 0;
  }
};

uint32_t Assembler::operator [](const char * const l) const {
  ASSERT(l != NULL);
  const Labels::const_iterator E = labels_.end(),
        it = std::find_if(labels_.begin(), E, MapFinder(l));
  if (it == E) {
    return -1;
  }
  ASSERT(it->second <= instructions_.size());
  return it->second;
}

uint32_t Assembler::label(const char * const l) {
  typedef std::pair< Labels::const_iterator, bool > Result;
  ASSERT(l != NULL);
  Result result = labels_.insert(
      std::make_pair(std::string(l), instructions_.size()));
  return result.first->second;
}

void Assembler::push(const uint32_t op, const uint32_t a,
    const uint32_t b, const uint32_t c) {
  ASSERT(op > Opcodes::kNull);
  ASSERT(op < Opcodes::kUpperBound);
  instructions_.push_back(Instruction(op, a, b, c));
}

uint32_t Assembler::pushMemory(const char * const a) {
  typedef std::pair< Labels::iterator, bool > Result;
  ASSERT(a != NULL);
  
  if ('\0' == *a) {
    return 0;
  }
  
  Result result = memoryUnifier_.insert(
      std::make_pair(std::string(a), instructions_.size()));

  if (result.second) {
    const size_t l = strlen(a) + 1;
    result.first->second = std::distance(memory_.begin(), memory_.end());
    std::copy(a, a + l, std::back_inserter(memory_));
  }

  ASSERT(result.first->second <= memory_.size());
  return result.first->second;
}

void Assembler::pushPrintDebug(const char * const a,
    const char * const b, const ExecutionMode::MODES c) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a);
  const uint32_t p = b != NULL ? pushMemory(b) : 0;
  push(Opcodes::kPrintDebug, o, 0, 0);
}

void Assembler::pushPrintError(const char * const a,
    const char * const b, const ExecutionMode::MODES c) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a);
  const uint32_t p = b != NULL ? pushMemory(b) : 0;
  push(Opcodes::kPrintError, o, p, c);
}

void Assembler::pushExecute(const ExecutionMode::MODES m,
    const uint32_t o, const uint32_t c) {
  ASSERT(m == ExecutionMode::kNone
      || m == ExecutionMode::kAnd
      || m == ExecutionMode::kOr);
  ASSERT(o < codeSize());
  push(Opcodes::kExecute, m, o, c);
}

void Assembler::pushExistsHeader(const char * const a) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kExistsHeader, o, 0, 0);
}

void Assembler::pushEqualHeader(const char * const a, const char * const b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kEqualHeader, o, p, 0);
}

void Assembler::pushEqualCookie(const char * const a, const char * const b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kEqualCookie, o, p, 0);
}

void Assembler::pushGreaterThanHeader(const char * const a, const uint32_t b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kGreaterThanHeader, o, b, 0);
}

void Assembler::pushLessThanHeader(const char * const a, const uint32_t b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kLessThanHeader, o, b, 0);
}

void Assembler::pushIsMethod(const char * const a, const uint32_t b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (strlen(a) < b) {
    throw std::invalid_argument("Invalid 2st argument: greater than string");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kIsMethod, o, b, 0);
}

void Assembler::pushIsScheme(const char * const a, const uint32_t b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (strlen(a) < b) {
    throw std::invalid_argument("Invalid 2st argument: greater than string");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kIsScheme, o, b, 0);
}

void Assembler::pushContainsDomain(const char * const a, const uint32_t b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (strlen(a) < b) {
    throw std::invalid_argument("Invalid 2st argument: greater than string");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kContainsDomain, o, b, 0);
}

void Assembler::pushEqualDomain(const char * const a, const uint32_t b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (strlen(a) < b) {
    throw std::invalid_argument("Invalid 2st argument: greater than string");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kEqualDomain, o, b, 0);
}

void Assembler::pushNotEqualDomain(const char * const a, const uint32_t b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (strlen(a) < b) {
    throw std::invalid_argument("Invalid 2st argument: greater than string");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kNotEqualDomain, o, b, 0);
}

void Assembler::pushStartsWithDomain(const char * const a, const uint32_t b,
    const uint32_t c) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (strlen(a) < b) {
    throw std::invalid_argument("Invalid 2st argument: greater than string");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kStartsWithDomain, o, b, c);
}

void Assembler::pushContainsPath(const char * const a, const uint32_t b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (strlen(a) < b) {
    throw std::invalid_argument("Invalid 2st argument: greater than string");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kContainsPath, o, b, 0);
}

void Assembler::pushEqualPath(const char * const a, const uint32_t b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (strlen(a) < b) {
    throw std::invalid_argument("Invalid 2st argument: greater than string");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kEqualPath, o, b, 0);
}

void Assembler::pushStartsWithPath(const char * const a, const uint32_t b,
    const uint32_t c) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (strlen(a) < b) {
    throw std::invalid_argument("Invalid 2st argument: greater than string");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kStartsWithPath, o, b, c);
}

void Assembler::pushNotEqualPath(const char * const a, const uint32_t b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (strlen(a) < b) {
    throw std::invalid_argument("Invalid 2st argument: greater than string");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kNotEqualPath, o, b, 0);
}

void Assembler::pushContainsQueryParameter(
    const char * const a, const char * const b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (b == NULL) {
    throw std::invalid_argument("Invalid 2st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kContainsQueryParameter, o, p, 0);
}

void Assembler::pushContainsCookie(const char * const a, const char * const b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (b == NULL) {
    throw std::invalid_argument("Invalid 2st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kContainsCookie, o, p, 0);
}


void Assembler::pushContainsHeader(const char * const a, const char * const b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (b == NULL) {
    throw std::invalid_argument("Invalid 2st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kContainsHeader, o, p, 0);
}

void Assembler::pushGreaterThanAfterHeader(const char * const a,
    const char * const b, const uint32_t c) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (b == NULL) {
    throw std::invalid_argument("Invalid 2st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kGreaterThanAfterHeader, o, p, c);
}

void Assembler::pushLessThanAfterHeader(const char * const a,
    const char * const b, const uint32_t c) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (b == NULL) {
    throw std::invalid_argument("Invalid 2st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kLessThanAfterHeader, o, p, c);
}

void Assembler::pushGreaterThanAfterQueryParameter(const char * const a,
    const char * const b, const uint32_t c) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (b == NULL) {
    throw std::invalid_argument("Invalid 2st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kGreaterThanAfterQueryParameter, o, p, c);
}

void Assembler::pushLessThanAfterQueryParameter(const char * const a,
    const char * const b, const uint32_t c) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (b == NULL) {
    throw std::invalid_argument("Invalid 2st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kLessThanAfterQueryParameter, o, p, c);
}

void Assembler::pushGreaterThanAfterCookie(const char * const a,
    const char * const b, const uint32_t c) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (b == NULL) {
    throw std::invalid_argument("Invalid 2st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kGreaterThanAfterCookie, o, p, c);
}

void Assembler::pushLessThanAfterCookie(const char * const a,
    const char * const b, const uint32_t c) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (b == NULL) {
    throw std::invalid_argument("Invalid 2st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kLessThanAfterCookie, o, p, c);
}

void Assembler::pushGreaterThanQueryParameter(const char * const a,
    const uint32_t b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kGreaterThanQueryParameter, o, b, 0);
}

void Assembler::pushLessThanQueryParameter(const char * const a,
    const uint32_t b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kLessThanQueryParameter, o, b, 0);
}

void Assembler::pushGreaterThanCookie(const char * const a,
    const uint32_t b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kGreaterThanAfterCookie, o, b, 0);
}

void Assembler::pushLessThanCookie(const char * const a,
    const uint32_t b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kLessThanAfterCookie, o, b, 0);
}

void Assembler::pushEqualQueryParameter(const char * const a,
    const char * const b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (b == NULL) {
    throw std::invalid_argument("Invalid 2st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kEqualQueryParameter, o, p, 0);
}

void Assembler::pushStartsWithQueryParameter(const char * const a,
    const char * const b, const uint32_t c) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (b == NULL) {
    throw std::invalid_argument("Invalid 2st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kStartsWithQueryParameter, o, p, c);
}

void Assembler::pushNotEqualQueryParameter(const char * const a,
    const char * const b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (b == NULL) {
    throw std::invalid_argument("Invalid 2st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kNotEqualHeader, o, p, 0);
}

void Assembler::pushNotEqualHeader(const char * const a,
    const char * const b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (b == NULL) {
    throw std::invalid_argument("Invalid 2st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kNotEqualHeader, o, p, 0);
}

void Assembler::pushStartsWithHeader(const char * const a,
    const char * const b, const uint32_t c) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (b == NULL) {
    throw std::invalid_argument("Invalid 2st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kStartsWithHeader, o, p, c);
}

void Assembler::pushNotEqualCookie(const char * const a,
    const char * const b) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  } else if (b == NULL) {
    throw std::invalid_argument("Invalid 2st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a),
        p = pushMemory(b);
  push(Opcodes::kNotEqualHeader, o, p, 0);
}

void Assembler::pushExistsQueryParameter(const char * const a) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kExistsQueryParameter, o, 0, 0);
}

void Assembler::pushExistsCookie(const char * const a) {
  if (a == NULL) {
    throw std::invalid_argument("Invalid 1st argument: NULL pointer");
  }
  const uint32_t o = pushMemory(a);
  push(Opcodes::kExistsCookie, o, 0, 0);
}

} //end of filters namespace
} //end of http namespace
