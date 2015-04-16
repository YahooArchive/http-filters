/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#ifndef VM_PRINTER_H
#define VM_PRINTER_H

#include <ostream>

#include "opcodes.h"
#include "vm.h"

namespace http {
namespace filters {
namespace vm {
struct Printer {
  void print(const Code &, const Memory &) const;

  void print(const Code &, const Memory &,
      std::ostream &) const;

  static const char * opcode(Opcodes::OPCODES);
};
} //end of vm namespace
} //end of filters namespace
} //end of http namespace
#endif //VM_PRINTER_H
