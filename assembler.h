/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "opcodes.h"

#include "vm.h"

namespace http {
namespace filters {
typedef std::map< std::string, uint32_t > Labels;

struct Assembler {
  typedef std::vector< Instruction > Instructions;
  typedef std::vector< char > RawMemory;

  Instructions instructions_;
  RawMemory memory_;
  Labels memoryUnifier_;
  Labels labels_;

  Assembler(void) {
    memory_.push_back('\0');
  }

  uint32_t operator [](const char * const) const;

  uint32_t label(const char * const);

  inline void pushHalt(void) { push(Opcodes::kHalt, 0, 0, 0); }

  inline void pushSkip(const char * const a = NULL) {
    const uint32_t b = a != NULL ? pushMemory(a) : 0;
    push(Opcodes::kSkip, b, 0, 0);
  }

  void pushExecute(const ExecutionMode::MODES,
      const uint32_t, const uint32_t c = 0);

  inline void pushReturn(void) { push(Opcodes::kReturn, 0, 0, 0); }
  inline void pushNone(void) { push(Opcodes::kNone, 0, 0, 0); }
  inline void pushAnd(void) { push(Opcodes::kAnd, 0, 0, 0); }
  inline void pushOr(void) { push(Opcodes::kOr, 0, 0, 0); }
  inline void pushNot(void) { push(Opcodes::kNot, 0, 0, 0); }
  inline void pushTrue(void) { push(Opcodes::kTrue, 0, 0, 0); }
  inline void pushFalse(void) { push(Opcodes::kFalse, 0, 0, 0); }

  void push(const uint32_t, const uint32_t,
      const uint32_t, const uint32_t);

  uint32_t pushMemory(const char * const);

  void pushPrintDebug(const char * const, const char * const b = NULL,
      const ExecutionMode::MODES c = ExecutionMode::kNone);

  void pushPrintError(const char * const, const char * const b = NULL,
      const ExecutionMode::MODES = ExecutionMode::kNone);

  inline Code code(void) const { return Code(
      reinterpret_cast< Code::Type * >(instructions_.data()),
      instructions_.size() * (sizeof(Instruction) / sizeof(Code::Type))); }

  inline Memory memory(void) const {
    return Memory(memory_.data(), memory_.size()); }

  inline uint32_t codeSize(void) const {
    return instructions_.size(); }

  void pushIsMethod(const char * const, const uint32_t);

  void pushIsScheme(const char * const, const uint32_t);

  /*
   * Domain
   */
  void pushContainsDomain(const char * const, const uint32_t);

  void pushEqualDomain(const char * const, const uint32_t);

  void pushNotEqualDomain(const char * const, const uint32_t);

  void pushStartsWithDomain(const char * const, const uint32_t, const uint32_t);

  /*
   * Path
   */
  void pushContainsPath(const char * const, const uint32_t);

  void pushEqualPath(const char * const, const uint32_t);

  void pushNotEqualPath(const char * const, const uint32_t);

  void pushStartsWithPath(const char * const, const uint32_t, const uint32_t);

  /*
   * Query Parameters
   */
  void pushContainsQueryParameter(const char * const, const char * const);

  void pushEqualQueryParameter(const char * const, const char * const);

  void pushExistsQueryParameter(const char * const);

  void pushGreaterThanQueryParameter(const char * const, const uint32_t);

  void pushGreaterThanAfterQueryParameter(const char * const,
      const char * const, const uint32_t);

  void pushLessThanQueryParameter(const char * const, const uint32_t);

  void pushLessThanAfterQueryParameter(const char * const,
      const char * const, const uint32_t);

  void pushNotEqualQueryParameter(const char * const, const char * const);

  void pushStartsWithQueryParameter(const char * const, const char * const, const uint32_t);

  /*
   * Headers
   */
  void pushContainsHeader(const char * const, const char * const);

  void pushEqualHeader(const char * const, const char * const);

  void pushExistsHeader(const char * const);

  void pushGreaterThanHeader(const char * const, const uint32_t);

  void pushGreaterThanAfterHeader(const char * const, const char * const,
      const uint32_t);

  void pushLessThanHeader(const char * const, const uint32_t);

  void pushLessThanAfterHeader(const char * const, const char * const,
      const uint32_t);

  void pushNotEqualHeader(const char * const, const char * const);

  void pushStartsWithHeader(const char * const, const char * const, const uint32_t);

  /*
   * Cookies
   */
  void pushContainsCookie(const char * const, const char * const);

  void pushEqualCookie(const char * const, const char * const);

  void pushExistsCookie(const char * const);

  void pushGreaterThanCookie(const char * const, const uint32_t);

  void pushGreaterThanAfterCookie(const char * const, const char * const,
      const uint32_t);

  void pushLessThanCookie(const char * const, const uint32_t);

  void pushLessThanAfterCookie(const char * const, const char * const,
      const uint32_t);

  void pushNotEqualCookie(const char * const, const char * const);
};
} //end of filters namespace
} //end of http namespace
#endif //ASSEMBLER_H
