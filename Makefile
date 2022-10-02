CXX = g++
CXXFLAG = -Wall -std=c++11
LFLAG = -lopencv_core460 -lopencv_imgproc460 -lopencv_highgui460 -lopencv_videoio460 -lzlibstatic -lfreetype -lzlibstatic

LINCLUDE = include
LLIB = lib
LOBJ = obj
LSRC = src
LAPP = bin
LDEP = dep

APPNAME = main

ifeq ($(MAKECMDGOALS), release)
CXXFLAG += -O3
else
CXXFLAG += -g
endif

SRC = $(wildcard $(LSRC)/*.cpp)
SRC += $(wildcard $(LSRC)/*.c)
OBJ  = $(SRC:$(LSRC)/%=$(LOBJ)/%.o)
INCLUDE = $(LINCLUDE:%=-I%)
LIB = $(LLIB:%=-L%)

.PHONY: all, clean, run, release

DEP = $(OBJ:$(LOBJ)/%.o=$(LDEP)/%.d)

$(LDEP)/%.d: $(LSRC)/%
	@$(CXX) $(INCLUDE) $< -MM -MT $(@:$(LDEP)/%.d=$(LOBJ)/%.o) >$@

RM = rm -f
ifeq ($(OS), Windows_NT)
APPNAME := $(LAPP)/$(APPNAME).exe
endif
APP := ./$(APPNAME)

all: $(APP)

build: $(APP)

clean:
	$(RM) $(OBJ) $(APP) $(DEP)

$(APP):	$(OBJ)
	$(CXX) $(CXXFLAG) -o $@ $^ $(INCLUDE) $(LIB) $(LFLAG)

ifneq ($(MAKECMDGOALS), clean)
-include $(DEP)
endif

$(LOBJ)/%.o: $(LSRC)/%
	$(CXX) $(CXXFLAG) -o $@ -c $< $(INCLUDE)

run: $(APP)
	$(APP)

release: $(APP)
