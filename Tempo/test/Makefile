QUIET		?= @

INCPATH		= ../include
LIBPATH		= ../lib

EXTRAINC	?= -I../../ulib/include
EXTRALIB	?= -lgsl -lcblas -L../../ulib/lib -lulib

CXXFLAGS	?= -O3 -flto -W -Wall
LDFLAGS		?= -lTempo $(EXTRALIB)
DEBUG		?=

TARGET		= $(patsubst %.cpp, %.test, $(wildcard *.cpp))

%.test: %.cpp $(LIBPATH)/libTempo.a
	$(QUIET)echo "GEN "$@;
	$(QUIET)$(CXX) -I $(INCPATH) $(EXTRAINC) $(CXXFLAGS) $(DEBUG) $< -o $@ -L $(LIBPATH) $(LDFLAGS);

all: $(TARGET)

clean:
	$(QUIET)rm -rf $(TARGET) *~

test:
	$(QUIET)./run_all_test.sh

.PHONY: all clean test
