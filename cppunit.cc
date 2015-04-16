/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TextOutputter.h>

#include <exception>

int main(void) {
  bool success = false;
  try {
    CppUnit::TestFactoryRegistry & registry
      = CppUnit::TestFactoryRegistry::getRegistry();

    CppUnit::TextUi::TestRunner runner;
    runner.addTest(registry.makeTest());

    success = runner.run();
  } catch (std::exception & e) {
    std::cout << "Failure due to: " << e.what() << std::endl;
  }
  return ((success) ? 0 : 1);
}
