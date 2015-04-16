/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#ifndef BITMAP_H
#define BITMAP_H

#include <cstdlib>
#include <cstring>

#include "my-assert.h"

namespace http {
namespace filters {

struct Bit;

struct Bitmap {
  int size_;
  int s_;
  char * bits_;

  ~Bitmap() {
    ASSERT(bits_ != NULL);
    free(bits_);
  }

  Bitmap(const int s, const bool i = false) :
    size_(s), s_((size_ / 0x8) + 1),
    bits_(reinterpret_cast< char * >(malloc(s_))) {
    ASSERT(bits_ != NULL);
    memset(bits_, i ? 0xFF : 0x00, s_);
  }

  Bit operator [] (const int);
  Bit begin(void);
  Bit end(void);
  inline int size(void) const { return size_; }

  inline void reset(const bool i = false) {
    memset(bits_, i ? 0xFF : 0x00, s_);
  }
};

struct Bit {
  operator bool (void) const {
    ASSERT(bitmap_ != NULL);
    ASSERT(bitmap_->s_ >= a_);
    return bitmap_->bits_[a_] & c_;
  }

  Bit & operator = (const bool b) {
    ASSERT(bitmap_ != NULL);
    ASSERT(bitmap_->s_ >= a_);
    if (b) {
      bitmap_->bits_[a_] |= c_;
    } else {
      bitmap_->bits_[a_] &= (-1 ^ c_);
    }
    return *this;
  }

  inline Bit & operator ++ (void) {
    unsigned char c = c_ << 0x1;
    if (c < c_) {
      c |= 0x1;
      ++a_;
    }
    c_ = c;
    return *this;
  }

  inline Bit operator ++ (int) {
    Bit b = *this;
    operator ++ ();
    return b;
  }

  inline bool operator == (const Bit & b) const {
    return bitmap_ == b.bitmap_
      && a_ == b.a_
      && c_ == b.c_;
  }

  inline bool operator != (const Bit & b) const {
    return ! operator == (b);
  }

  inline bool operator > (const Bit & b) const {
    ASSERT(bitmap_ == b.bitmap_);
    return a_ > b.a_ || (a_ == b.a_ && c_ > b.c_);
  }

  inline bool operator <= (const Bit & b) const {
    return ! operator > (b);
  }

  //TODO(dmorilha): implement a better solution
  inline Bit & operator += (int a) {
    for (; a > 0; --a) {
      operator ++ ();
    }
    return *this;
  }

//private:
  Bitmap * bitmap_;
  int a_;
  unsigned char c_;

  Bit(Bitmap * const b, const int a, const unsigned char c) :
    bitmap_(b), a_(a), c_(c) {
    ASSERT(bitmap_ != NULL);
  }

  friend class Bitmap;
};

} //end of filters namespace
} //end of http namespace

#endif //BITMAP_H
