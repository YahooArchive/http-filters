/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#include "bitmap.h"

namespace http {
namespace filters {
Bit Bitmap::operator [] (const int i) {
  ASSERT(size_ * 0x8 >= i);
  const int a = i / 0x8,
            b = (i % 0x8);
  return Bit(this, a, 0x1 << b);
}

Bit Bitmap::begin(void) {
  return Bit(this, 0x0, 0x1);
}

Bit Bitmap::end(void) {
  return Bit(this, size_ / 0x8, 0x1 << (size_ % 0x8));
}
}
}
