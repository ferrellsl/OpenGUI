include ../ostest.mak
include ../cflags.mak

STARGET	= libtinycfg.a
DTARGET	= libtinycfg.so

STARGETF = ../lib/$(STARGET)

ifeq ($(OS),linux)
OS=linux
DTARGETF = ../lib/$(DTARGET)
endif

dirs := .

all_files := $(wildcard *.cpp)
all_objs := $(subst .cpp,.o, $(all_files) )
all_lobjs := $(subst .cpp,.lo, $(all_files) )

HDRS = $(wildcard *.h)

SOURCES = $(all_files)
OBJS = $(all_objs)
LOBJS = $(all_lobjs)

CXXFLAGS = $(CFLAGS) -I.

all: $(STARGETF) $(DTARGETF)

$(STARGETF): $(OBJS)
	ar rcs $(STARGETF) $(OBJS)
	@echo
	@echo '########     Static version: $(STARGET) #######'
	@echo

$(DTARGETF): $(LOBJS)
	$(CC) -o $(DTARGETF) -g -fnostdinc -shared -Wl,-soname,$(DTARGET) $(LOBJS) -lstdc++
	@echo
	@echo '########     Dynamic version: $(DTARGET) #######'
	@echo

%.o:  %.cpp $(HDRS)   
	$(CXX) $(CXXFLAGS) -o $@ $< 

%.lo:  %.cpp $(HDRS)  
	$(CXX) $(CXXFLAGS) $(DSO) -o $@  $<
