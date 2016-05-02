QUIET		?= @

INCPATH		= ../../include
LIBPATH		= ../../lib

EXTRAINC	?= -I../../../ulib/include -I../../../libconfig/include
EXTRALIB	?= -lpald -lglpk -lcblas -L../../../ulib/lib -lulib -lconfig++

CXXFLAGS	?= -g3 -O3 -W -Wall
LDFLAGS		?= -lTempo -lgsl $(EXTRALIB)
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
