/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#ifndef REPRESENTATION_H
#define REPRESENTATION_H

#include <vector>
#include <string>

#include "my-assert.h"

#define OP(I, O, ...) { \
  const char * const p[] = {__VA_ARGS__}; \
  I.addOp(O, p, ARRAY_SIZE(p)); \
}

#define CHILD_OP(I, O, ...) { \
  const char * const p[] = {__VA_ARGS__}; \
  I.addChildOp(O, p, ARRAY_SIZE(p)); \
}

namespace http {
namespace filters {

struct NodeTypes {
  enum TYPES {
    kNull,
    kUndefined,
    kAnd,
    kOr,
    kNot,
    kOp,
    kUpperBound,
  };
};

struct Node;

struct Node {
  Node * next,
    * previous;

  //TODO(dmorilha): what we can annotate in order to produce better code

  virtual ~Node() { }

  virtual bool hasChild(void) const { return false; }
  virtual NodeTypes::TYPES type(void) const { return NodeTypes::kUndefined; }

protected:
  Node(void) : next(NULL), previous(NULL) { }
};

struct BinaryNode : Node {
  Node * child;

  virtual ~BinaryNode() { }

  bool hasChild(void) const { return child != NULL; }
  virtual NodeTypes::TYPES type(void) const { return NodeTypes::kUndefined; }

protected:
  BinaryNode(void) : child(NULL) { }
};

struct And : BinaryNode {
  NodeTypes::TYPES type(void) const { return NodeTypes::kAnd; }
};

struct Or : BinaryNode {
  NodeTypes::TYPES type(void) const { return NodeTypes::kOr; }
};

struct Not : Node {
  NodeTypes::TYPES type(void) const { return NodeTypes::kNot; }
};

struct Op : Node {
  typedef std::vector< std::string > Parameters;

  std::string name;
  Parameters parameters;

  ~Op() { }

  Op(void) { }

  Op(const char * const n) : name(n) { }

  Op(const std::string & n) : name(n) { }

  Op(const char * const n, const Parameters & p) :
    name(n), parameters(p) { }

  Op(const std::string & n, const Parameters & p) :
    name(n), parameters(p) { }

  Op(const std::string &, const char * const * const, const int);
  Op(const char * const, const char * const * const, const int);

  NodeTypes::TYPES type(void) const { return NodeTypes::kOp; }
};

void clean(const Node *);

struct Tree {
  typedef std::vector< Node * > Stack;

  Node * root_;
  Node * current_;
  Stack stack_;

  Tree(void) : root_(NULL), current_(NULL) { }

  inline void addAnd(void) { insert(new And()); }
  inline void addChildAnd(void) { insertChild(new And()); }

  inline void addOr(void) { insert(new Or()); }
  inline void addChildOr(void) { insertChild(new Or()); }

  inline void addNot(void) { insert(new Not()); }
  inline void addChildNot(void) { insertChild(new Not()); }

  template < class T1 >
  inline void addOp(const T1 & a) {
    insert(new Op(a));
  }
  template < class T1 >
  inline void addChildOp(const T1 & a) {
    insertChild(new Op(a));
  }

  template < class T1, class T2 >
  inline void addOp(const T1 & a, const T2 & b) {
    insert(new Op(a, b));
  }
  template < class T1, class T2 >
  inline void addChildOp(const T1 & a, const T2 & b) {
    insertChild(new Op(a, b));
  }

  template < class T1, class T2, class T3 >
  inline void addOp(const T1 & a, const T2 & b, const T3 & c) {
    insert(new Op(a, b, c));
  }
  template < class T1, class T2, class T3 >
  inline void addChildOp(const T1 & a, const T2 & b, const T3 & c) {
    insertChild(new Op(a, b, c));
  }

  inline void cleanAll(void) {
    ASSERT(root_ != NULL);
    clean(root_);
    root_ = current_ = NULL;
  }

  void insert(Node * const);
  void insertChild(Node * const);

  void moveCurrent(void) {
    ASSERT(current_ != NULL);
    while (current_->next != NULL) {
      current_ = current_->next;
    }
  }

  inline void parent(void) {
    ASSERT( ! stack_.empty());
    current_ = stack_.back();
    stack_.pop_back();
  }

  inline const Node * root(void) const { return root_; }
};

typedef std::vector< Tree > Forest;

void cleanAll(Forest &);

} //end of filters namespace
} //end of ats namespace
#endif //REPRESENTATION_H
