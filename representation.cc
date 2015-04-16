/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#include "representation.h"

namespace http {
namespace filters {

Op::Op(const std::string & n, const char * const * const p, const int s) :
  name(n) {
    std::copy(p, p + s, std::back_inserter(parameters));
  }

Op::Op(const char * const n, const char * const * const p, const int s) :
  name(n) {
    std::copy(p, p + s, std::back_inserter(parameters));
  }

void clean(const Node * i) {
  ASSERT(i != NULL);
  Node * j = i->next;
  while (i != NULL) {
    if (i->hasChild()) {
      const BinaryNode * k = dynamic_cast< const BinaryNode * >(i);
      ASSERT(k != NULL);
      clean(k->child);
    }
    delete i;
    i = j;
    if (i != NULL) { j = i->next; }
  }
}

void Tree::insert(Node * n) {
  ASSERT(n != NULL);
  ASSERT(n->previous == NULL);
  if (root_ == NULL) {
    ASSERT(current_ == NULL);
    current_ = root_ = n;
  } else {
    ASSERT(current_->next == NULL);
    current_->next = n;
    n->previous = current_;
    current_ = n;
    moveCurrent();
  }
}

void Tree::insertChild(Node * const n) {
  ASSERT(n != NULL);
  ASSERT(n->previous == NULL);
  ASSERT(current_ != NULL);
  ASSERT(current_->type() == NodeTypes::kAnd
      || current_->type() == NodeTypes::kOr);
  BinaryNode * o = dynamic_cast< BinaryNode * >(current_);
  ASSERT(o != NULL);
  ASSERT(o->child == NULL);
  o->child = n;
  n->previous = current_;
  stack_.push_back(current_);
  current_ = n;
  moveCurrent();
}

void cleanAll(Forest & f) {
  const Forest::iterator end = f.end();
  Forest::iterator it = f.begin();

  for (; it != end; ++it) {
    it->cleanAll();
  }
}
} //end of filters namespace
} //end of http namespace
