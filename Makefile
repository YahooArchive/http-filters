CXX ?= g++
CXXFLAGS += -g -fPIC
ENABLE_ASAN ?= false
GDB ?= gdb

ifeq ($(shell uname), Darwin)
	GDB := lldb
endif

ifeq ($(ENABLE_ASAN), true)
	CXXFLAGS += -fsanitize=address
endif

gdb: tests
	$(GDB) ./$< -ex run -ex quit;

run: tests
	./$<;

cppunit: assembler.cc bitmap.cc compiler.cc vm-printer.cc \
	representation.cc tests.cc vm-impl.h cppunit.cc
	$(CXX) -DCPPUNIT $(CXXFLAGS) $(LDFLAGS) -lcppunit -o $@ $(filter-out %.h, $^);
	./cppunit;

tests: assembler.o bitmap.o compiler.o vm-printer.o representation.o \
	vm-impl.h tests.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBCPPUNIT) -o $@ $(filter-out %.h, $^);

ats-filters.so: LDFLAGS += -Wl,--allow-shlib-undefined -shared
ats-filters.so: CXXFLAGS += -DPLUGIN_TAG=\"ats-filters\"
ats-filters.so: ats-filters.o assembler.o bitmap.o compiler.o representation.o \
	ts.o ts-impl.o vm-impl.h vm-printer.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(filter-out %.h, $^);

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^;

clean:
	rm -fv *.o tests

lines:
	wc -l *.cc *.h
