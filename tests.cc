/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */


#ifdef CPPUNIT
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <sstream>

static std::stringstream N;
static std::ostream & output = N;

#else

#include <iostream>

static std::ostream & output = std::cout;

namespace CppUnit {
struct TestFixture { };
}

#define CPPUNIT_TEST_SUITE(a) a(void) { int b
#define CPPUNIT_TEST(a) a()
#define CPPUNIT_TEST_SUITE_END() }
#define CPPUNIT_TEST_SUITE_REGISTRATION(a) int main(void) { a b; return 0; }

#endif

#include "my-assert.h"

#include "assembler.h"
#include "bitmap.h"
#include "compiler.h"
#include "console-impl.h"
#include "vm-impl.h"
#include "vm-printer.h"

using namespace http::filters;

struct HttpFiltersUnitTest : public CppUnit::TestFixture {
  void testBitmaps(void) {
    using namespace http::filters;
    const int z = 16;

    {
      Bitmap bitmap(z, false);

      const Bit e = bitmap.end();

      for (int i = 0; i < z; ++i) { ASSERT( ! bitmap[i]); }
      for (Bit b = bitmap.begin(); b != e; ++b) {
        ASSERT( ! static_cast< bool >(b));
      }

      for (int i = 0; i < z; ++i) { bitmap[i] = true; }
      for (int i = 0; i < z; ++i) { ASSERT(bitmap[i]); }
      for (Bit b = bitmap.begin(); b != e; ++b) {
        ASSERT(static_cast< bool >(b));
      }

      for (int i = 0; i < z; ++i) { bitmap[i] = false; }
      for (int i = 0; i < z; ++i) { ASSERT( ! bitmap[i]); }
      for (Bit b = bitmap.begin(); b != e; ++b) {
        ASSERT( ! static_cast< bool >(b));
      }
    }

    {
      http::filters::Bitmap bitmap(z, true);

      const Bit e = bitmap.end();

      for (int i = 0; i < z; ++i) { ASSERT(bitmap[i]); }
      for (Bit b = bitmap.begin(); b != e; ++b) {
        ASSERT(static_cast< bool >(b));
      }

      for (int i = 0; i < z; ++i) { bitmap[i] = false; }
      for (int i = 0; i < z; ++i) { ASSERT( ! bitmap[i]); }
      for (Bit b = bitmap.begin(); b != e; ++b) {
        ASSERT( ! static_cast< bool >(b));
      }

      for (int i = 0; i < z; ++i) { bitmap[i] = true; }
      for (int i = 0; i < z; ++i) { ASSERT(bitmap[i]); }
      for (Bit b = bitmap.begin(); b != e; ++b) {
        ASSERT(static_cast< bool >(b));
      }
    }
  }

  void testAssembler(void) {
    using namespace http::filters;

    Assembler assembler;
    assembler.pushPrintError("Hello World!");
    assembler.pushHalt();

    typedef VMProxy< ConsoleImplementation > MyVM;
    MyVM vm(ConsoleImplementation(output, output),
        assembler.code(), assembler.memory());

    ASSERT(vm.run(0));
    ASSERT(vm.run(0));

    for (int i = 0; i < vm.vm_.bitmap_.size(); ++i) {
      ASSERT( ! vm.vm_.bitmap_[i]);
    }
  }

  void testCompiler(void) {
    using namespace http::filters;
    Tree t;
    t.addOp("true");
    t.addOp("false");
    t.addOp("true");
    t.addOp("false");
    t.addOp("true");
    t.addOp("false");
    t.addOp("true");
    t.addOp("false");
    t.addOp("true");
    const char * const p[] = {"Hello World (says the Compiler)"};
    t.addOp("printError", p, ARRAY_SIZE(p));

    Compiler c;
    c.compile(t);
    t.cleanAll();

    vm::Printer printer;
    printer.print(c.assembler_.code(),
        c.assembler_.memory(), output);

    typedef VMProxy< ConsoleImplementation > MyVM;
    MyVM vm(ConsoleImplementation(output, output),
        c.assembler_.code(), c.assembler_.memory());
    ASSERT(vm.run(0));
    ASSERT(vm.run(0));
  }

  void testCompiler2(void) {
    using namespace http::filters;
    Tree t;
    t.addAnd();
      t.addChildOp("true");
      t.addOr();
        t.addChildAnd();
          t.addChildOp("true");
          t.addOp("false");
        t.parent();
        t.addOp("false");
        t.addOp("false");
        t.addNot();
        t.addOp("false");
      t.addOp("true");
      t.addOp("true");
      t.addNot();
      t.addOp("true");
      t.parent();
    t.parent();
    t.addOp("true");

    OP(t, "printError", "Hello Compiler");

    Compiler c;
    c.compile(t);
    t.cleanAll();

    vm::Printer printer;
    printer.print(c.assembler_.code(),
        c.assembler_.memory(), output);

    typedef VMProxy< ConsoleImplementation > MyVM;
    MyVM vm(ConsoleImplementation(output, output),
        c.assembler_.code(), c.assembler_.memory());
    //TODO(dmorilha) do not ask me how do I know the right number is 16
    ASSERT(vm.run(16));
    ASSERT(vm.run(16));
  }

  void test1(void) {
    const uint32_t p[] = {
      Opcodes::kTrue, 0x0, 0x0, 0x0,
      Opcodes::kHalt, 0x0, 0x0, 0x0,
    };

    typedef VMProxy< ConsoleImplementation > MyVM;
    MyVM vm(ConsoleImplementation(output, output),
        Code(p, ARRAY_SIZE(p)), Memory());

    ASSERT(vm.run(0));
    ASSERT(vm.run(0));

    for (int i = 0; i < vm.vm_.bitmap_.size(); ++i) {
      if (i == 0 || i == 1) {
        ASSERT(vm.vm_.bitmap_[i]);
        continue;
      }
      ASSERT( ! vm.vm_.bitmap_[i]);
    }
  }

  void test2(void) {
    const uint32_t p[] = {
      Opcodes::kFalse, 0x0, 0x0, 0x0,
      Opcodes::kHalt,  0x0, 0x0, 0x0,
    };

    typedef VMProxy< ConsoleImplementation > MyVM;
    MyVM vm(ConsoleImplementation(output, output),
        Code(p, ARRAY_SIZE(p)), Memory());

    ASSERT( ! vm.run(0));
    ASSERT( ! vm.run(0));

    for (int i = 0; i < vm.vm_.bitmap_.size(); ++i) {
      if (i == 0) {
        ASSERT(vm.vm_.bitmap_[i]);
        continue;
      }
      ASSERT( ! vm.vm_.bitmap_[i]);
    }
  }

  void test3(void) {
    const uint32_t p[] = {
      Opcodes::kNot,  0x0, 0x0, 0x0,
      Opcodes::kTrue, 0x0, 0x0, 0x0,
      Opcodes::kHalt, 0x0, 0x0, 0x0,
    };

    typedef VMProxy< ConsoleImplementation > MyVM;
    MyVM vm(ConsoleImplementation(output, output),
        Code(p, ARRAY_SIZE(p)), Memory());

    ASSERT( ! vm.run(0));
    ASSERT( ! vm.run(0));

    for (int i = 0; i < vm.vm_.bitmap_.size(); ++i) {
      if (i == 2 || i == 3) {
        ASSERT(vm.vm_.bitmap_[i]);
        continue;
      }
      ASSERT( ! vm.vm_.bitmap_[i]);
    }
  }

  void test4(void) {
    const uint32_t p[] = {
      Opcodes::kAnd,  0x0, 0x0, 0x0,
      Opcodes::kNot,  0x0, 0x0, 0x0,
      Opcodes::kFalse, 0x0, 0x0, 0x0,
      Opcodes::kHalt, 0x0, 0x0, 0x0,
    };

    typedef VMProxy< ConsoleImplementation > MyVM;
    MyVM vm(ConsoleImplementation(output, output),
        Code(p, ARRAY_SIZE(p)), Memory());

    ASSERT(vm.run(0));
    ASSERT(vm.run(0));

    for (int i = 0; i < vm.vm_.bitmap_.size(); ++i) {
      if (i == 4) {
        ASSERT(vm.vm_.bitmap_[i]);
        continue;
      }
      ASSERT( ! vm.vm_.bitmap_[i]);
    }
  }

  void test5(void) {
    const uint32_t p[] = {
      Opcodes::kOr,    0x0, 0x0, 0x0,
      Opcodes::kFalse, 0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kHalt,  0x0, 0x0, 0x0,
    };

    typedef VMProxy< ConsoleImplementation > MyVM;
    MyVM vm(ConsoleImplementation(output, output),
        Code(p, ARRAY_SIZE(p)), Memory());

    ASSERT(vm.run(0));
    ASSERT(vm.vm_.registers_.pc == 3);
    ASSERT(vm.run(0));

    for (int i = 0; i < vm.vm_.bitmap_.size(); ++i) {
      if (i == 2 || i == 4 || i == 5) {
        ASSERT(vm.vm_.bitmap_[i]);
        continue;
      }
      ASSERT( ! vm.vm_.bitmap_[i]);
    }
  }

  void test6(void) {
    const uint32_t p[] = {
      Opcodes::kAnd,   0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kFalse, 0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kTrue,  0x0, 0x0, 0x0,
      Opcodes::kHalt,  0x0, 0x0, 0x0,
    };

    typedef VMProxy< ConsoleImplementation > MyVM;
    MyVM vm(ConsoleImplementation(output, output),
        Code(p, ARRAY_SIZE(p)), Memory());

    ASSERT( ! vm.run(0));
    ASSERT(vm.vm_.registers_.pc == 3);
    ASSERT( ! vm.run(0));

    for (int i = 0; i < vm.vm_.bitmap_.size(); ++i) {
      if (i >= 2 && i <= 4) {
        ASSERT(vm.vm_.bitmap_[i]);
        continue;
      }
      ASSERT( ! vm.vm_.bitmap_[i]);
    }
  }

  void test7(void) {
    const uint32_t p[] = {
      Opcodes::kExecute, 0x1, 0x3, 0x1, //0
      Opcodes::kHalt,    0x0, 0x0, 0x0, //1
      Opcodes::kTrue,    0x0, 0x0, 0x0, //2
      Opcodes::kFalse,   0x0, 0x0, 0x0, //3
      Opcodes::kTrue,    0x0, 0x0, 0x0, //4
      Opcodes::kHalt,    0x0, 0x0, 0x0, //5
    };

    typedef VMProxy< ConsoleImplementation > MyVM;
    MyVM vm(ConsoleImplementation(output, output),
        Code(p, ARRAY_SIZE(p)), Memory());

    ASSERT( ! vm.run(0));
    ASSERT(vm.vm_.registers_.pc == 2);
    ASSERT( ! vm.run(0));

    for (int i = 0; i < vm.vm_.bitmap_.size(); ++i) {
      if (i == 0 || i == 6) {
        ASSERT(vm.vm_.bitmap_[i]);
        continue;
      }
      ASSERT( ! vm.vm_.bitmap_[i]);
    }
  }

  void test8(void) {
    const uint32_t p[] = {
      Opcodes::kExecute, 0x1, 0x3, 0x1, //0
      Opcodes::kHalt,    0x0, 0x0, 0x0, //1
      Opcodes::kFalse,   0x0, 0x0, 0x0, //2
      Opcodes::kTrue,    0x0, 0x0, 0x0, //3
      Opcodes::kFalse,   0x0, 0x0, 0x0, //4
      Opcodes::kHalt,    0x0, 0x0, 0x0, //5
    };

    typedef VMProxy< ConsoleImplementation > MyVM;
    MyVM vm(ConsoleImplementation(output, output),
        Code(p, ARRAY_SIZE(p)), Memory());

    ASSERT(vm.run(0));
    ASSERT(vm.vm_.registers_.pc == 2);
    ASSERT(vm.run(0));

    for (int i = 0; i < vm.vm_.bitmap_.size(); ++i) {
      if (i == 0 || i == 1 || i == 6 || i == 7) {
        ASSERT(vm.vm_.bitmap_[i]);
      } else {
        ASSERT( ! vm.vm_.bitmap_[i]);
      }
    }
  }

  void test9(void) {
    const char * const m = "\0Hello World!";

    const uint32_t p[] = {
      Opcodes::kExecuteSingle, 0x2, 0x0, 0x0, //0
      Opcodes::kHalt,          0x0, 0x0, 0x0, //1
      Opcodes::kPrintError,    0x1, 0x0, 0x0, //2
    };

    typedef VMProxy< ConsoleImplementation > MyVM;
    MyVM vm(ConsoleImplementation(output, output),
        Code(p, ARRAY_SIZE(p)), Memory(m, ARRAY_SIZE(m)));

    ASSERT(vm.run(0));
    ASSERT(vm.vm_.registers_.pc == 2);
    ASSERT(vm.run(0));
  }

  void test10(void) {
    const char * const m =
      "\0"
      "foo\0"
      "bar\0"
      "baz";

    const uint32_t p[] = {
      Opcodes::kFalse,      0x0, 0x0, 0x0, //0
      Opcodes::kPrintError, 0x1, 0x0, 0x2, //1
      Opcodes::kPrintError, 0x1, 0x0, 0x3, //2
      Opcodes::kFlip,       0x0, 0x0, 0x0, //3
      Opcodes::kPrintError, 0x5, 0x0, 0x2, //4
      Opcodes::kPrintError, 0x5, 0x0, 0x3, //5
      Opcodes::kHalt,       0x0, 0x0, 0x0, //6
    };

    typedef VMProxy< ConsoleImplementation > MyVM;
    MyVM vm(ConsoleImplementation(output, output),
        Code(p, ARRAY_SIZE(p)), Memory(m, ARRAY_SIZE(m)));

    ASSERT(vm.run(0));
    ASSERT(vm.run(0));
    ASSERT(vm.vm_.registers_.pc == 7);

    for (int i = 0; i < vm.vm_.bitmap_.size(); ++i) {
      if (i == 0 || i == 6 || i == 7) {
        ASSERT(vm.vm_.bitmap_[i]);
        continue;
      }
      ASSERT( ! vm.vm_.bitmap_[i]);
    }
  }

  CPPUNIT_TEST_SUITE(HttpFiltersUnitTest);
  CPPUNIT_TEST(testBitmaps);
  CPPUNIT_TEST(testAssembler);
  CPPUNIT_TEST(testCompiler);
  CPPUNIT_TEST(testCompiler2);
  CPPUNIT_TEST(test1);
  CPPUNIT_TEST(test2);
  CPPUNIT_TEST(test3);
  CPPUNIT_TEST(test4);
  CPPUNIT_TEST(test5);
  CPPUNIT_TEST(test6);
  CPPUNIT_TEST(test7);
  CPPUNIT_TEST(test8);
  CPPUNIT_TEST(test9);
  CPPUNIT_TEST(test10);
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(HttpFiltersUnitTest);
