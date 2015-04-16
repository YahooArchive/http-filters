/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#ifndef CONSOLE_IMPL_H
#define CONSOLE_IMPL_H

#include <iostream>

#include "my-assert.h"

#include "base-impl.h"

namespace http {
namespace filters {

struct ConsoleImplementation : BaseImplementation {
  std::ostream & output_;
  std::ostream & error_;

  ConsoleImplementation(std::ostream & o = std::cout,
      std::ostream & e = std::cerr):
    output_(o), error_(e) { }

  bool PrintError(const char * const c, const char * const l) const {
    ASSERT(c != NULL);
    if (l != NULL) {
      error_ << l << " ";
    }
    error_ << c << std::endl;
    return true;
  }

  bool PrintDebug(const char * const c, const char * const l) const {
    ASSERT(c != NULL);
    if (l != NULL) {
      output_ << l << " ";
    }
    output_ << c << std::endl;
    return true;
  }
};

} //end of filters namespace
} //end of http namespace

#endif //CONSOLE_IMPL_H
