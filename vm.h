/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#ifndef VM_H
#define VM_H

#include <algorithm>
#include <vector>
#include <string>

#include <stdint.h>

#include "my-assert.h"

#include "array.h"
#include "bitmap.h"
#include "opcodes.h"

namespace http {
namespace filters {

struct Instruction {
  uint32_t op;
  uint32_t a;
  uint32_t b;
  uint32_t c;

  Instruction(const uint32_t op, const uint32_t a, const uint32_t b,
    const uint32_t c) : op(op), a(a), b(b), c(c) {
    ASSERT(op < Opcodes::kUpperBound);
  }
};

struct Instructions {
  Instruction * instructions_;
  uint32_t size_;
};

struct Registers : Instruction {
  uint32_t pc;
  uint32_t count;

  bool r;
  bool n;
  uint8_t mode;

  Registers(void) : Instruction(0, 0, 0, 0),
    pc(0), count(0), r(true), n(false), mode(0) { }
};

typedef util::Array< const uint32_t > Code;
typedef util::Array< const char > Memory;

static const int kSize = 4;

template < class I >
struct VM {
  static const int kBits = 2;
  static const int kInitialStackSize = 16;
  static const int kStackSize;

  typedef std::vector< Registers > Stack;

  Registers registers_;
  Bitmap bitmap_;
  Bit bit_;
  const Code c_;
  const Memory m_;

  Stack stack_;
  I i_;

  VM(const I & i, const Code & c, const Memory & m) :
    bitmap_(c.size * kBits, false), bit_(bitmap_.begin()),
    c_(c), m_(m), i_(i) {
    registers_.mode = ExecutionMode::kNone;
    stack_.reserve(kInitialStackSize);
  }

  bool run(const uint32_t, const uint32_t j = 0);

  inline void dispatch(void);

  inline void forceReturn(void) {
    registers_.op = Opcodes::kReturn;
    registers_.a = registers_.b = registers_.c = 0x0;
  }

  inline const Registers & stackPush(void) {
    stack_.push_back(registers_);
    registers_ = Registers();
    return stack_.back();
  }

  inline void stackPop(void) {
    ASSERT( ! stack_.empty());
    registers_ = stack_.back();
    stack_.pop_back();
  }

  inline void fetch(void) {
    //load the instruction at once.
    uint32_t * p = reinterpret_cast< uint32_t * >(&registers_);

    const uint32_t * begin = c_ + (registers_.pc * kSize),
          * end = begin + kSize;

    //simple 1 instruction GOTO
    while (begin[0] == Opcodes::kExecuteSingle) {
      ASSERT(begin[1] != registers_.pc); //prevents infinite loop.
      ASSERT(begin[1] < c_.size);
      //TODO(dmorilha): fetch bit.
      begin = c_ + (begin[1] * kSize);
      end = begin + kSize;
    }

    std::copy(begin, end, p);
    step();

    //std::cerr << "NEXT" "\n";
  }

  inline void step(void) {
    ++registers_.pc;
    --registers_.count;
  }

  inline void jumpBit(const uint32_t i) {
    ASSERT(i < c_.size);
    bit_ = bitmap_[i * kBits];
    ASSERT(bit_ <= bitmap_.end());
  }

  inline void incrementBit(void) {
    ++bit_;
    ASSERT(bit_ <= bitmap_.end());
  }

  inline void print(void) const;

  inline bool result(void) const { return registers_.r; }

  inline void result(bool r) {
    if (registers_.n) {
      r = ! r;
      registers_.n = false;
    }
    registers_.r = r;
  }
};

template < class I >
struct VMProxy {
  typedef std::pair< std::string, uint32_t > Entry;
  typedef std::vector< Entry > Entries;

  VM< I > vm_;
  Entries entries_;

  //TODO(dmorilha): need to make sure entries are sorted.
  VMProxy(const I & i, const Code & c, const Memory & m,
      const Entries & e = Entries()) : vm_(i, c, m), entries_(e) { }

  bool run(const uint32_t i, const uint32_t j = 0) {
    return vm_.run(i, j);
  }

  bool run(const std::string & k, const uint32_t j = 0) {
    const Entries::const_iterator i = std::lower_bound(entries_.begin(),
        entries_.end(), std::make_pair(k, 0));
    if (i->first != k) {
      //Entry does not exists
      return false;
    }
    return vm_.run(i->second, j);
  }
};

} //end of filters namespace
} //end of http namespace
#endif //VM_H
