#//////////////////////////////////////////////////////////////////////////
#//////////////////////////////////////////////////////////////////////////
#//////////////////////////////////////////////////////////////////////////
#//
#//	Copyright (c) 2003-2025
#//	Shane T. Mueller, Ph.D.  smueller at obereed dot net
#//
#//     This file is part of the PEBL project.
#//
#//    PEBL is free software; you can redistribute it and/or modify
#//    it under the terms of the GNU General Public License as published by
#//    the Free Software Foundation; either version 2 of the License, or
#//    (at your option) any later version.
#//
#//    PEBL is distributed in the hope that it will be useful,
#//    but WITHOUT ANY WARRANTY; without even the implied warranty of
#//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#//    GNU General Public License for more details.
#//
#//    You should have received a copy of the GNU General Public License
#//    along with PEBL; if not, write to the Free Software
#//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  
#//    02111-1307 USA
#//
#//////////////////////////////////////////////////////////////////////////
#//////////////////////////////////////////////////////////////////////////
#//////////////////////////////////////////////////////////////////////////

#This only affects install location.  The binary should be 
#locatable anywhere
PREFIX = /usr/local/
##might also be pebl-language
PEBLNAME = pebl2
PEBLDIRNAME = "pebl2"
EXECNAME = $(PEBLNAME)
PEBL_VERSION = 2.2
#USE_WAAVE=1       ##Optional; comment out to turn off waave multimedia library
USE_NETWORK=1      ##Optional; comment out to turn off sdl_net library.
USE_PORTS=1        ##lpt, serial port, etc.
USE_HTTP=1         ##Optional; turn on/off for http get/set
USE_MIXER=1        ##Optional; uses sdl mixer for better audio+ogg/mp3/etc.
USE_AUDIOIN=1      ##Optional; enables audio recording and voice key

PEBL_ARCH   = PEBL_WIN32

USE_DEBUG = 0     ##Optional; turn on/off debugging stuff.


GCC   = gcc
GCXX = g++ 
#C = ~/src/emscripten-master/emcc
#CXX = ~/src/emscripten-master/em++
EMCC = libs/emsdk/upstream/emscripten/emcc
EMCXX = libs/emsdk/upstream/emscripten/em++
FP =  libs/emsdk/upstream/emscripten/tools/file_packager.py

CL = /c/msys64/mingw64/bin/gcc
CLXX = /c/msys64/mingw64/bin/g++
WINDRES = /c/msys64/mingw64/bin/windres


# Wrapper targets that set OBJ_DIR and call the real targets
# Default target
main:
	$(MAKE) -f Makefile-win.mak OBJ_DIR=obj-native CC=$(CL) CXX=$(CLXX) main-real

# Real build targets
main-real: CC=$(CL)
main-real: CXX=$(CLXX)


ifdef USE_DEBUG
DEBUGFLAGS =  -DPEBL_DEBUG -g -DSDL2_DELETE=1
else
DEBUGFLAGS = 
endif
CXXFLAGS0 = -O3

#CXXFLAGS_EMSCRIPTEN = -DPEBL_EMSCRIPTEN -DPEBL_ITERATIVE_EVAL -DPREFIX=$(PREFIX) -DEXECNAME=$(EXECNAME) -DPEBLNAME=$(PEBLNAME) -DPEBLDIRNAME=$(PEBLDIRNAME) -I/usr/include/x86_64-linux-gnu/ -I/usr/local/include/emscripten/

CXXFLAGS_EMSCRIPTEN = -DPEBL_EMSCRIPTEN -DPEBL_HTTP -DHTTP_LIB=3 -DPREFIX=$(PREFIX) -DEXECNAME=$(EXECNAME) -DPEBLNAME=$(PEBLNAME) -DPEBLDIRNAME=$(PEBLDIRNAME) -DPEBL_VERSION=\"$(PEBL_VERSION)\" -sUSE_SDL=2 -sUSE_SDL_NET=2 -sUSE_SDL_TTF=2 -sUSE_SDL_IMAGE=2 -sUSE_SDL_MIXER=2


## http=2 is curl
CXXFLAGS_LINUX =   -DPEBL_LINUX -DPEBL_UNIX -DENABLE_BINRELOC -DPREFIX=$(PREFIX) -DEXECNAME=$(EXECNAME) -DPEBLNAME=$(PEBLNAME) -DPEBLDIRNAME=$(PEBLDIRNAME) -DPEBL_VERSION=\"$(PEBL_VERSION)\" -DHTTP_LIB=2

## http=2 is curl
CXXFLAGS_WIN32 =   -pipe -DPEBL_WIN32 -DNO_STDIO_REDIRECT -DENABLE_BINRELOC -DPREFIX=$(PREFIX) -DEXECNAME=$(EXECNAME) -DPEBLNAME=$(PEBLNAME) -DPEBLDIRNAME=$(PEBLDIRNAME) -DPEBL_VERSION=\"$(PEBL_VERSION)\" -DHTTP_LIB=2

## Enable HTTP support by default
USE_HTTP = 1

#ifdef USE_WAAVE
##	@echo "Using WAAVE movie library";
#	CXXFLAGS1 = -DPEBL_MOVIES  
#	LINKOPTS1 = -lwaave
#endif



## This requires the following soundfont package:
## timgm6mb-soundfont 
ifdef USE_MIXER
	CXXFLAGS2B = -DPEBL_MIXER
	LINKOPTS2B = -lSDL2_mixer
endif


ifdef USE_NETWORK
      CXXFLAGS3 = -DPEBL_NETWORK
      LINKOPTS3 = -lSDL2_net
endif

ifdef USE_PORTS 
	CXXFLAGS4 = -DPEBL_USEPORTS
##	CFLAGS4 =  -DPEBL_USEPORTS
endif

##hard-coding libcurl here.
ifdef USE_HTTP
	CXXFLAGS5 = -DPEBL_HTTP
	LINKOPTS5 = -lcurl
endif

ifdef USE_AUDIOIN
	CXXFLAGS6 = -DPEBL_AUDIOIN
endif


CXXFLAGSX = $(CXXFLAGS0) $(CXXFLAGS1) $(CXXFLAGS2) $(CXXFLAGS2B) $(CXXFLAGS3) $(CXXFLAGS4) $(CXXFLAGS5) $(CXXFLAGS6) $(CXXFLAGS7) 
LINKOPTS = $(LINKOPTS1) $(LINKOPTS2) $(LINKOPTS2B) $(LINKOPTS3) $(LINKOPTS4) $(LINKOPTS5) $(LINKOPTS7)


main-real: CXXFLAGS = $(CXXFLAGSX) $(CXXFLAGS_WIN32)
cl: CXXFLAGS = $(CXXFLAGSX) $(CXXFLAGS_WIN32)



BASE_SDL_CONFIG = /usr/bin/sdl-config
BASE_SDL_FLAGS = -I/c/msys64/mingw64/include/SDL2 -I/c/msys64/mingw64/include -D_REENTRANT

SDL_FLAGS = $(BASE_SDL_FLAGS)


SHELL = /bin/bash
BISON = /usr/bin/bison
FLEX  = /usr/bin/flex

BIN_DIR  = bin
SBIN_DIR = sbin
# Object directory - can be overridden at invocation time:
#   make OBJ_DIR=obj-native main
#   make OBJ_DIR=obj-em em-opt
# Default is 'obj' for backward compatibility
OBJ_DIR  ?= obj
OUT_DIR  = output
SRC_DIR  = src
BASE_DIR = src/base
APPS_DIR = src/apps
LIBS_DIR = src/libs
OBJECTS_DIR = src/objects
DEVICES_DIR = src/devices
PLATFORMS_DIR = src/platforms
SDL_DIR = src/platforms/sdl
UTIL_DIR = src/utility
TEST_DIR = src/tests


##replaces /default local path with $prefix using sed in the desktop file
SEDLINE= s|/usr/local/|$(PREFIX)|

vpath %.o   $(OBJ_DIR)
vpath %.cpp $(SRC_DIR)
vpath %.c   $(SRC_DIR)
vpath %.h   $(SRC_DIR)
vpath %.hpp $(SRC_DIR)
vpath %.y   $(SRC_DIR)
vpath %.l   $(SRC_DIR)


PUTILITIES_SRC = $(UTIL_DIR)/PEBLUtility.cpp \
		$(UTIL_DIR)/PError.cpp \
		$(UTIL_DIR)/BinReloc.cpp \
		$(UTIL_DIR)/PEBLPath.cpp \
		$(UTIL_DIR)/PEBLHTTP.cpp \
		$(UTIL_DIR)/md5.cpp \
		$(UTIL_DIR)/happyhttp.cpp \
		$(UTIL_DIR)/mman.c 




PUTILITIES_OBJ1  = $(patsubst %.cpp, %.o, $(PUTILITIES_SRC))
PUTILITIES_OBJ  = $(patsubst %.c, %.o, $(PUTILITIES_OBJ1))   ##Get the .c file
PUTILITIES_INC1  = $(patsubst %.cpp, %.h, $(PUTILITIES_SRC))
PUTILITIES_INC  = $(patsubst %.c, %.h, $(PUTILITIES_INC1))   ##Get the plain .c file



MAN_DIR  =   doc/pman
PEBL_DOCSRC = 		$(MAN_DIR)/main.tex \
			$(MAN_DIR)/intro.tex \
			$(MAN_DIR)/chap3.tex \
			$(MAN_DIR)/chap4.tex \
			$(MAN_DIR)/chap5.tex \
			$(MAN_DIR)/launcher.tex \
			$(MAN_DIR)/reference.tex \
			$(MAN_DIR)/colors.tex 

PEBLBASE_SRCXX =	$(BASE_DIR)/Evaluator.cpp \
			$(BASE_DIR)/FunctionMap.cpp \
			$(BASE_DIR)/grammar.tab.cpp \
			$(BASE_DIR)/PEBLObject.cpp \
			$(BASE_DIR)/Loader.cpp \
			$(BASE_DIR)/PComplexData.cpp \
			$(BASE_DIR)/PList.cpp \
			$(BASE_DIR)/PNode.cpp \
			$(BASE_DIR)/VariableMap.cpp \
			$(BASE_DIR)/Variant.cpp \
			$(DEVICES_DIR)/PEventLoop.cpp 

PEBLBASE_OBJXX = $(patsubst %.cpp, %.o, $(PEBLBASE_SRCXX))



##This just collects plain .c files,
PEBLBASE_SRC = $(BASE_DIR)/lex.yy.c \
		$(UTIL_DIR)/rs232.c 


PEBLBASE_OBJ = $(patsubst %.c, %.o, $(PEBLBASE_SRC))


##These are 3rd party libraries we include directly.
#LIB_SRC      = libs/happyhttp/happyhttp.cpp
#LIB_OBJ      =$(patsubst %.cpp, %.o, $(LIB_SRC))



POBJECT_SRC  =  $(OBJECTS_DIR)/PEnvironment.cpp \
		$(OBJECTS_DIR)/PWidget.cpp \
		$(OBJECTS_DIR)/PWindow.cpp  \
		$(OBJECTS_DIR)/PImageBox.cpp \
		$(OBJECTS_DIR)/PCanvas.cpp  \
		$(OBJECTS_DIR)/PColor.cpp  \
		$(OBJECTS_DIR)/PDrawObject.cpp \
		$(OBJECTS_DIR)/PFont.cpp \
		$(OBJECTS_DIR)/PTextObject.cpp \
		$(OBJECTS_DIR)/PLabel.cpp \
		$(OBJECTS_DIR)/PTextBox.cpp \
		$(OBJECTS_DIR)/PMovie.cpp \
		$(OBJECTS_DIR)/PCustomObject.cpp 

##		$(PUTILITIES_SRC)


POBJECT_OBJ  = $(patsubst %.cpp, %.o, $(POBJECT_SRC))
POBJECT_INC  = $(patsubst %.cpp, %.h, $(POBJECT_SRC))


PDEVICES_SRC =  $(DEVICES_DIR)/PDevice.cpp \
	$(DEVICES_DIR)/PEventQueue.cpp \
	$(DEVICES_DIR)/PEvent.cpp\
	$(DEVICES_DIR)/PKeyboard.cpp \
	$(DEVICES_DIR)/PTimer.cpp \
	$(DEVICES_DIR)/DeviceState.cpp \
	$(DEVICES_DIR)/PStream.cpp \
	$(DEVICES_DIR)/PAudioOut.cpp \
	$(DEVICES_DIR)/PNetwork.cpp \
	$(DEVICES_DIR)/PJoystick.cpp \
	$(DEVICES_DIR)/PParallelPort.cpp \
	$(DEVICES_DIR)/PComPort.cpp \
	$(DEVICES_DIR)/PEyeTracker.cpp


PDEVICES_OBJ  = $(patsubst %.cpp, %.o, $(PDEVICES_SRC))
PDEVICES_INC  = $(patsubst %.cpp, %.h, $(PDEVICES_SRC))





PLATFORM_SDL_SRC  =	$(SDL_DIR)/PlatformEnvironment.cpp \
			$(SDL_DIR)/PlatformWidget.cpp \
			$(SDL_DIR)/PlatformWindow.cpp \
			$(SDL_DIR)/PlatformImageBox.cpp \
			$(SDL_DIR)/PlatformKeyboard.cpp \
			$(SDL_DIR)/PlatformFont.cpp \
			$(SDL_DIR)/PlatformLabel.cpp \
			$(SDL_DIR)/PlatformTextBox.cpp \
			$(SDL_DIR)/PlatformTimer.cpp	\
			$(SDL_DIR)/PlatformDrawObject.cpp \
			$(SDL_DIR)/PlatformCanvas.cpp \
			$(SDL_DIR)/SDLUtility.cpp \
		   	$(SDL_DIR)/PlatformEventQueue.cpp \
			$(SDL_DIR)/PlatformAudioOut.cpp \
			$(SDL_DIR)/PlatformNetwork.cpp \
			$(SDL_DIR)/PlatformJoystick.cpp\
			$(SDL_DIR)/PlatformAudioIn.cpp
#			$(SDL_DIR)/PlatformMovie.cpp

PLATFORM_SDL_OBJ  = 	$(patsubst %.cpp, %.o, $(PLATFORM_SDL_SRC))
PLATFORM_SDL_INC  = 	$(patsubst %.cpp, %.h, $(PLATFORM_SDL_SRC))


FUNCTIONLIB_SRC = $(LIBS_DIR)/PEBLMath.cpp \
	  	  $(LIBS_DIR)/PEBLStream.cpp \
		  $(LIBS_DIR)/PEBLObjects.cpp \
                  $(LIBS_DIR)/PEBLEnvironment.cpp \
                  $(LIBS_DIR)/PEBLList.cpp \
                  $(LIBS_DIR)/PEBLString.cpp



FUNCTIONLIB_OBJ =  $(patsubst %.cpp, %.o, $(FUNCTIONLIB_SRC))
FUNCTIONLIB_INC =  $(patsubst %.cpp, %.h, $(FUNCTIONLIB_SRC))




PEBLMAIN_SRC = 		$(APPS_DIR)/PEBL.cpp \
			$(PEBLBASE_SRCXX) \
			$(PDEVICES_SRC) \
			$(FUNCTIONLIB_SRC) \
			$(POBJECT_SRC) \
			$(PUTILITIES_SRC) \
			$(PLATFORM_SDL_SRC) 
###			$(LIB_SRC)




PEBLMAIN_OBJ = $(patsubst %.cpp, %.o, $(PEBLMAIN_SRC))
PEBLMAIN_INC = $(patsubst %.cpp, %.h, $(PEBLMAIN_SRC))

PEBLMAIN_EXTRA = $(LIBS_DIR)/Functions.h \
	           	$(OBJECTS_DIR)/RGBColorNames.h 





DIRS = \
	$(OBJ_DIR) \
	$(OBJ_DIR)/$(SRC_DIR) \
	$(OBJ_DIR)/$(BASE_DIR) \
	$(OBJ_DIR)/$(APPS_DIR) \
	$(OBJ_DIR)/$(OBJECTS_DIR) \
	$(OBJ_DIR)/$(LIBS_DIR) \
	$(OBJ_DIR)/$(DEVICES_DIR) \
	$(OBJ_DIR)/$(PLATFORMS_DIR) \
	$(OBJ_DIR)/$(SDL_DIR) \
	$(OBJ_DIR)/$(UTIL_DIR) \
	$(OBJ_DIR)/$(TEST_DIR) 


##############################
# Dependencies
#
##	   -L$(PREFIX)/lib -lSDL -lpthread -lSDL_image -lSDL_ttf -lSDL_gfx  -lSDL_net -lpng -lSDL_audioin\

#SDL_FLAGS = -I/usr/include/SDL -I/usr/local/include -D_REENTRANT -L/usr/lib -L/usr/local/lib \
#	-L/home/smueller/Projects/src/waave-1.0/src -L/usr/lib  -L/usr/local/lib \
#	-I/usr/include/SDL -I/usr/include -I/usr/local/include -D_REENTRANT 

#d: 
#	@echo $(PEBLMAIN_OBJ)


resource.o: resource.rc installer/pebl2.ico
	$(WINDRES) resource.rc -O coff -o resource.o

main-real:  $(DIRS) $(PEBLMAIN_OBJ) $(PEBLBASE_OBJ) $(PEBLMAIN_INC) resource.o
	$(CXX) $(CXXFLAGS) -Wall -mwindows -Wl,-rpath -Wl,LIBDIR $(DEBUGFLAGS) \
	-Wno-write-strings \
	-DPEBL_ARCH \
	$(SDL_FLAGS) -g	\
	-o $(BIN_DIR)/$(PEBLNAME) \
	$(patsubst %.o, $(OBJ_DIR)/%.o, $(PEBLBASE_OBJ)) \
	$(patsubst %.o, $(OBJ_DIR)/%.o, $(PEBLMAIN_OBJ)) \
	resource.o \
	-L/c/msys64/mingw64/lib \
	-lmingw32 -lSDL2main -lSDL2 -lpthread -lSDL2_image -lSDL2_net -lSDL2_ttf -lSDL2_gfx  \
	-lpng  $(LINKOPTS)


doc: $(PEBL_DOCSRC)
	cd doc/pman; pdflatex main.tex
	cp doc/pman/main.pdf doc/pman/PEBLManual$(PEBL_VERSION).pdf


.PHONY: appimage

parse:
	bison -d $(BASE_DIR)/grammar.y -o $(BASE_DIR)/grammar.tab.cpp
	flex -o$(BASE_DIR)/lex.yy.c  $(BASE_DIR)/Pebl.l 

parse-debug:
	bison -d $(BASE_DIR)/grammar.y -t --verbose --graph=bison.vcg -o $(BASE_DIR)/grammar.tab.cpp
	flex  -o$(BASE_DIR)/lex.yy.c -d $(BASE_DIR)/Pebl.l

.PHONY: tests


%.h:
	@echo Updating %.h;

# Loader.cpp depends on Functions.h because it loads the function tables
$(BASE_DIR)/Loader.o: $(BASE_DIR)/Loader.cpp $(LIBS_DIR)/Functions.h | $(DIRS)
	$(CXX)   $(CXXFLAGS) -g -c $<  -o $(OBJ_DIR)/$@  $(SDL_FLAGS)

%.o: %.cpp | $(DIRS)
	$(CXX)   $(CXXFLAGS) -g -c $^  -o $(OBJ_DIR)/$@  $(SDL_FLAGS)

# Compile C files - lex.yy.c needs C++ compiler due to C++ headers
# Other .c files (like rs232.c) use pure C
src/base/lex.yy.o: src/base/lex.yy.c | $(DIRS)
	$(CXX) $(CXXFLAGS) -Wno-register -g -c $<  -o $(OBJ_DIR)/$@  $(SDL_FLAGS)

%.o: %.c | $(DIRS)
	$(CC) -std=gnu99 -pipe -O3 -g -c $^  -o $(OBJ_DIR)/$@  $(SDL_FLAGS) 

#	-s USE_SDL=2 \   #this is for emscriten, which doesn't work.
#	-s USE_SDL_NET=2 \
#	-s USE_SDL_TTF=2 \
#	-s USE_SDL_IMAGE=2 \
#        -s SDL2_IMAGE_FORMATS='["png","jpeg","gif","bmp"]' \

#$(EM_SDL_FLAGS)

$(DIRS):
	@mkdir -p $@



remake: ready clean $(PROGS)
ready:
	-rm -f $(OUT_DIR)/*

.PHONY: dep
dep:
	$(CXX) -MM $(CXXFLAGS)    $(PEBLMAIN_SRC)  > .depend

.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	rm -rf obj-native obj-em obj
	rm -f resource.o
	@echo "✓ Build artifacts cleaned"


.PHONY: install


ifeq (.depend,$(wildcard .depend))
include .depend
endif
