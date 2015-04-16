#ifndef ASSERT_H
#define ASSERT_H

#ifdef CPPUNIT
#ifndef ASSERT
#include <cppunit/extensions/HelperMacros.h>
#define ASSERT CPPUNIT_ASSERT
#endif //ASSERT
#else
#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif //ASSERT
#endif //CPPUNIT

#endif //ASSERT_H
