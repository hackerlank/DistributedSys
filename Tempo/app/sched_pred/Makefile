QUIET		?= @

INCPATH		= ../../include
LIBPATH		= ../../lib

EXTRAINC	?= -I../../../ulib/include
EXTRALIB	?= -L../../../ulib/lib -lulib -lconfig++

CXXFLAGS	?= -O3 -flto -W -Wall
LDFLAGS		?= -lTempo $(EXTRALIB)
DEBUG		?=

TARGET		= $(patsubst %.cpp, %.app, $(wildcard *.cpp))

%.app: %.cpp $(LIBPATH)/libTempo.a
	$(QUIET)echo "GEN "$@;
	$(QUIET)$(CXX) -I $(INCPATH) $(EXTRAINC) $(CXXFLAGS) $(DEBUG) $< -o $@ -L $(LIBPATH) $(LDFLAGS);

all: $(TARGET)

clean:
	$(QUIET)rm -rf $(TARGET)
	$(QUIET)find . -name "*~" | xargs rm -rf

.PHONY: all clean test
