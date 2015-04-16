/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#ifndef VM_IMPL_H
#define VM_IMPL_H

#include <iostream>

#include "vm.h"

#define P_A m_ + registers_.a
#define P_B m_ + registers_.b
#define P_AB P_A, P_B
#define P_BA P_B, P_A

namespace http {
namespace filters {

template < class I >
bool VM< I >::run(const uint32_t o, const uint32_t j) {
  ASSERT(o < c_.size);
  ASSERT(j < c_.size - o);
  jumpBit(o);
  registers_.pc = o;
  registers_.count = j > 0 ? j : -1;

  while (true) {
    ASSERT(registers_.mode > ExecutionMode::kNull);
    ASSERT(registers_.mode < ExecutionMode::kUpperBound);

    //checks previous operation

    if (registers_.count == 0) {
      forceReturn();

    } else if (registers_.mode == ExecutionMode::kNone
      || (registers_.mode == ExecutionMode::kAnd && registers_.r)
      || (registers_.mode == ExecutionMode::kOr && ! registers_.r)) {

      //checks the cache.
      if (static_cast< bool >(bit_)) {
        incrementBit();
        result(static_cast< bool >(bit_));
        step();
        incrementBit();
        continue;
      } else {
        fetch();
      }
    } else {
      forceReturn();
    }

    dispatch();

    if (registers_.op == Opcodes::kReturn) {
      if ( ! stack_.empty()) {
        const bool r = registers_.r;
        stackPop();
        result(r);

        ASSERT(registers_.pc > 0);
        ASSERT(registers_.pc < c_.size);

        const uint32_t previous = registers_.pc - 1;
        jumpBit(previous);
        const uint32_t * b = c_ + previous * kSize;
        if (b[0] == Opcodes::kExecute
            && (b[1] == ExecutionMode::kOr
            || ExecutionMode::kAnd)) {
          bit_ = true;
          incrementBit();
          bit_ = r;
        } else {
          incrementBit();
        }
        incrementBit();
      } else {
        //TODO(dmorilha): investigate if this is a semantic fault.
        break;
      }
    } else if (registers_.op == Opcodes::kHalt) {
      break;
    }
  }

  return result();
}

template < class I >
void VM< I >::dispatch(void) {
  bool cache = false,
       r;
  //TODO(dmorilha): investigate the use of computed gotos.
  //http://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables
  switch (registers_.op) {
  case Opcodes::kNull: ASSERT(false); break;//unrecheable
  case Opcodes::kSkip: break;

  case Opcodes::kExecute:
    {
      const Registers & p = stackPush();

      switch (p.a) {
      case ExecutionMode::kNull: ASSERT(false); break; //unrecheable

      case ExecutionMode::kNone:
        registers_.mode = ExecutionMode::kNone;
        break;

      case ExecutionMode::kAnd:
        registers_.mode = ExecutionMode::kAnd;
        registers_.r = true;
        break;

      case ExecutionMode::kOr:
        registers_.mode = ExecutionMode::kOr;
        registers_.r = false;
        break;

      case ExecutionMode::kUpperBound: ASSERT(false); break; //unrecheable
      default: ASSERT(false); break; //unrecheable
      }

      registers_.pc = p.b;
      ASSERT(registers_.pc < c_.size);
      registers_.count = p.c > 0 ? p.c : -1;
      jumpBit(registers_.pc);

      /*
      std::cerr << "EXECUTE " << (int)registers_.mode << " " << registers_.pc
        << " " << registers_.count << "\n";
      */
      return;
    }

    break;

  case Opcodes::kReturn: break; //go back one level in the stack.
  case Opcodes::kHalt: break; //stop execution.

  case Opcodes::kNone:
    //std::cerr << "NONE" "\n";
    registers_.mode = ExecutionMode::kNone;
    break;

  case Opcodes::kAnd:
    //std::cerr << "AND" "\n";
    registers_.mode = ExecutionMode::kAnd;
    registers_.r = r = true;
    break;

  case Opcodes::kOr:
    //std::cerr << "OR" "\n";
    registers_.mode = ExecutionMode::kOr;
    registers_.r = false;
    break;

  case Opcodes::kNot:
    //std::cerr << "NOT" "\n";
    registers_.n = true;
    break;

  case Opcodes::kFlip:
    //std::cerr << "FLIP" "\n";
    result( r = ( ! result()));
    cache = true;
    break;

  case Opcodes::kTrue:
    //std::cerr << "TRUE" "\n";
    result(r = true);
    cache = true;
    break;

  case Opcodes::kFalse:
    //std::cerr << "FALSE" "\n";
    result(r = false);
    cache = true;
    break;

  case Opcodes::kPrintError:
    ASSERT(registers_.c < ExecutionMode::kUpperBound);
    if ( ! ((registers_.c == ExecutionMode::kAnd && ! registers_.r)
      || (registers_.c == ExecutionMode::kOr && registers_.r))) {
      i_.PrintError(P_AB);
    }
    break;

  case Opcodes::kPrintDebug:
    ASSERT(registers_.c < ExecutionMode::kUpperBound);
    if ( ! ((registers_.c == ExecutionMode::kAnd && ! registers_.r)
      || (registers_.c == ExecutionMode::kOr && registers_.r))) {
      i_.PrintDebug(P_AB);
    }
    break;

  case Opcodes::kIsMethod:
    result(r = i_.IsMethod(P_A, registers_.b));
    cache = true;
    break;

  case Opcodes::kIsScheme:
    result(r = i_.IsScheme(P_A, registers_.b));
    cache = true;
    break;

  case Opcodes::kContainsDomain:
    result(r = i_.ContainsDomain(P_A, registers_.b));
    cache = true;
    break;

  case Opcodes::kEqualDomain:
    result(r = i_.EqualDomain(P_A, registers_.b));
    cache = true;
    break;

  case Opcodes::kNotEqualDomain:
    result(r = i_.NotEqualDomain(P_A, registers_.b));
    cache = true;
    break;

  case Opcodes::kContainsPath:
    result(r = i_.ContainsPath(P_A, registers_.b));
    cache = true;
    break;

  case Opcodes::kEqualPath:
    result(r = i_.EqualPath(P_A, registers_.b));
    cache = true;
    break;

  case Opcodes::kNotEqualPath:
    result(r = i_.NotEqualPath(P_A, registers_.b));
    cache = true;
    break;

  case Opcodes::kContainsQueryParameter:
    result(r = i_.ContainsQueryParameter(P_AB));
    cache = true;
    break;

  case Opcodes::kEqualQueryParameter:
    result(r = i_.EqualQueryParameter(P_AB));
    cache = true;
    break;

  case Opcodes::kExistsQueryParameter:
    result(r = i_.ExistsQueryParameter(P_A));
    cache = true;
    break;

  case Opcodes::kGreaterThanQueryParameter:
    result(r = i_.GreaterThanQueryParameter(P_A, registers_.b));
    cache = true;
    break;

  case Opcodes::kGreaterThanAfterQueryParameter:
    result(r = i_.GreaterThanAfterQueryParameter(P_AB, registers_.c));
    cache = true;
    break;

  case Opcodes::kLessThanQueryParameter:
    result(r = i_.LessThanQueryParameter(P_A, registers_.b));
    cache = true;
    break;

  case Opcodes::kLessThanAfterQueryParameter:
    result(r = i_.LessThanAfterQueryParameter(P_AB, registers_.c));
    cache = true;
    break;

  case Opcodes::kNotEqualQueryParameter:
    result(r = i_.NotEqualQueryParameter(P_AB));
    cache = true;
    break;

  case Opcodes::kContainsHeader:
    result(r = i_.ContainsHeader(P_AB));
    cache = true;
    break;

  case Opcodes::kEqualHeader:
    result(r = i_.EqualHeader(P_AB));
    cache = true;
    break;

  case Opcodes::kExistsHeader:
    result(r = i_.ExistsHeader(P_A));
    cache = true;
    break;

  case Opcodes::kGreaterThanHeader:
    result(r = i_.GreaterThanHeader(P_A, registers_.b));
    cache = true;
    break;

  case Opcodes::kGreaterThanAfterHeader:
    result(r = i_.GreaterThanAfterHeader(P_AB, registers_.c));
    cache = true;
    break;

  case Opcodes::kLessThanHeader:
    result(r = i_.LessThanHeader(P_A, registers_.b));
    cache = true;
    break;

  case Opcodes::kLessThanAfterHeader:
    result(r = i_.LessThanAfterHeader(P_AB, registers_.c));
    cache = true;
    break;

  case Opcodes::kNotEqualHeader:
    result(r = i_.NotEqualHeader(P_AB));
    cache = true;
    break;

  case Opcodes::kContainsCookie:
    result(r = i_.ContainsCookie(P_AB));
    cache = true;
    break;

  case Opcodes::kEqualCookie:
    result(r = i_.EqualCookie(P_AB));
    cache = true;
    break;

  case Opcodes::kExistsCookie:
    result(r = i_.ExistsCookie(P_A));
    cache = true;
    break;

  case Opcodes::kGreaterThanCookie:
    result(r = i_.GreaterThanCookie(P_A, registers_.b));
    cache = true;
    break;

  case Opcodes::kGreaterThanAfterCookie:
    result(r = i_.GreaterThanAfterCookie(P_AB, registers_.c));
    cache = true;
    break;

  case Opcodes::kLessThanCookie:
    result(r = i_.LessThanCookie(P_A, registers_.b));
    cache = true;
    break;

  case Opcodes::kLessThanAfterCookie:
    result(r = i_.LessThanAfterCookie(P_AB, registers_.c));
    cache = true;
    break;

  case Opcodes::kNotEqualCookie:
    result(r = i_.NotEqualCookie(P_AB));
    cache = true;
    break;

  case Opcodes::kUpperBound: ASSERT(false); break; //unrecheable
  default: ASSERT(false); break; //unrecheable
  }

  if (cache) {
    bit_ = true;
    incrementBit();
    bit_ = r;
  } else {
    incrementBit();
  }

  incrementBit();

  //TODO(dmorilha) if kExecuteSingle make sure to persist into the called operation.
}

template < class I >
void VM< I >::print(void) const {
  std::cout << std::hex << registers_.op << " "
    << registers_.a << " " << registers_.b << " "
    << registers_.c << " " "\n";
}

template < class I >
const int VM< I >::kStackSize = 16;


} //end of filters namespace
} //end of http namespace

#undef P_A
#undef P_B
#undef P_AB
#undef P_BA

#endif //VM_IMPL_H
