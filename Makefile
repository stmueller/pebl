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
#USE_AUDIOIN=1     ##Optional; comment out to turn off  sdl_audioin library
#USE_NETWORK=1      ##Optional; comment out to turn off sdl_net library.
#USE_PORTS=1        ##lpt, serial port, etc.
USE_HTTP=1         ##Optional; turn on/off for http get/set
USE_MIXER=1        ##Optional; uses sdl mixer for better audio+ogg/mp3/etc.


#USE_LIBGAZE=1    ##Optional; turn on/off eyetribe gazeapi.  Probably won't work on linux.
USE_DEBUG = 1     ##Optional; turn on/off debugging stuff.

#USE_RTL = 1       ##optional; use a RTL text layout library--currently harfbuzz (not working)

GCC   = gcc
GCXX = g++ 
#C = ~/src/emscripten-master/emcc
#CXX = ~/src/emscripten-master/em++
EMCC = libs/emsdk/upstream/emscripten/emcc
EMCXX = libs/emsdk/upstream/emscripten/em++
FP =  libs/emsdk/upstream/emscripten/tools/file_packager.py

CL = clang
CLXX = clang++


# Wrapper targets that set OBJ_DIR and call the real targets
# Default target
main:
	$(MAKE) OBJ_DIR=obj-native CC=$(CL) CXX=$(CLXX) main-real

em-opt:
	$(MAKE) OBJ_DIR=obj-em CC=$(EMCC) CXX=$(EMCXX) em-opt-real

em-test:
	$(MAKE) OBJ_DIR=obj-em CC=$(EMCC) CXX=$(EMCXX) em-test-real

# Real build targets
em-opt-real: CC=$(EMCC)
em-opt-real: CXX=$(EMCXX)
em-test-real: CC=$(EMCC)
em-test-real: CXX=$(EMCXX)
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
CXXFLAGS_LINUX =   -DPEBL_UNIX -DPEBL_LINUX -DENABLE_BINRELOC -DPREFIX=$(PREFIX) -DEXECNAME=$(EXECNAME) -DPEBLNAME=$(PEBLNAME) -DPEBLDIRNAME=$(PEBLDIRNAME) -DPEBL_VERSION=\"$(PEBL_VERSION)\" -DHTTP_LIB=2

## Enable HTTP support by default
USE_HTTP = 1

ifdef USE_WAAVE
#	@echo "Using WAAVE movie library";
	CXXFLAGS1 = -DPEBL_MOVIES  
	LINKOPTS1 = -lwaave
endif


# audio in now supported with baseline SDL supposedl.
ifdef USE_AUDIOIN
#	@echo "Using audio in library"
	CXXFLAGS2 = -DPEBL_AUDIOIN
#	LINKOPTS2 = -lsdl_audioin -lsndio
endif


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

ifdef USE_LIBGAZE
	CXXFLAGS6 = -DPEBL_GAZELIB
	LINKOPTS4 = -lGazeApiLib -lboost_system -lboost_thread
endif

ifdef USE_RTL
	CXXFLAGS7 = -DPEBL_RTL
	LINKOPTS7 = -lharfbuzz
endif

CXXFLAGSX = $(CXXFLAGS0) $(CXXFLAGS1) $(CXXFLAGS2) $(CXXFLAGS2B) $(CXXFLAGS3) $(CXXFLAGS4) $(CXXFLAGS5) $(CXXFLAGS6) $(CXXFLAGS7) 
LINKOPTS = $(LINKOPTS1) $(LINKOPTS2) $(LINKOPTS2B) $(LINKOPTS3) $(LINKOPTS4) $(LINKOPTS5) $(LINKOPTS7)


em: CXXFLAGS = $(CXXFLAGSX) $(CXXFLAGS_EMSCRIPTEN) -DPEBL_ITERATIVE_EVAL
em: SDL_FLAGS = $(EM_SDL_FLAGS)
em-opt-real: CXXFLAGS = $(CXXFLAGSX) $(CXXFLAGS_EMSCRIPTEN) -DPEBL_ITERATIVE_EVAL
em-opt-real: SDL_FLAGS = $(EM_SDL_FLAGS)
em-test-real: CXXFLAGS = $(CXXFLAGSX) $(CXXFLAGS_EMSCRIPTEN) -DPEBL_ITERATIVE_EVAL
em-test-real: SDL_FLAGS = $(EM_SDL_FLAGS)
main-real: CXXFLAGS = $(CXXFLAGSX) $(CXXFLAGS_LINUX)
cl: CXXFLAGS = $(CXXFLAGSX) $(CXXFLAGS_LINUX)



BASE_SDL_CONFIG = /usr/bin/sdl-config
BASE_SDL_FLAGS = -I/usr/include/SDL2 -I/usr/local/include -D_REENTRANT

EM_SDL_CONFIG = libs/emsdk/upstream/emscripten/system/bin/sdl-config
EM_SDL_FLAGS = -Ilibs/SDL2_gfx-1.0.4 -D_REENTRANT

SDL_FLAGS = $(BASE_SDL_FLAGS)

##-L/usr/lib -L/usr/local/lib
#TET_FLAGS =  -Ilibs/tet-cpp-client-master/include -Llibs/tet-cpp-client-master 
#ifeq($(USE_WAAVE),1)
#SDL_FLAGS = $(SDLFLAGS) -L/home/smueller/Projects/src/waave-1.0/src 
#endif

#SDL_LIBS = -L/usr/lib -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL -lpthread -lSDLmain


#Comment/uncomment below on OSX
#OSX_FLAGS = -framework AppKit -lSDLmain -DPEBL_OSX
OSX_FLAGS =

#SDLIMG_FLAGS =  -L/usr/lib -L/usr/local/lib -Wl,-rpath,/usr/lib
#SDLIMG_FLAGS =  -L/usr/lib -Wl,-rpath,/usr/lib
#SDLIMG_LIBS =   -lSDL -lSDLmain -lpthread -lSDL_image -lSDL_net 

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
		$(UTIL_DIR)/happyhttp.cpp 




PUTILITIES_OBJ1  = $(patsubst %.cpp, %.o, $(PUTILITIES_SRC))
PUTILITIES_OBJ  = $(patsubst %.c, %.o, $(PUTILITIES_OBJ1))   ##Get the .c file
PUTILITIES_INC1  = $(patsubst %.cpp, %.h, $(PUTILITIES_SRC))
PUTILITIES_INC  = $(patsubst %.c, %.h, $(PUTILITIES_INC1))   ##Get the plain .c file


EMUTILITIES_SRC = $(UTIL_DIR)/PEBLUtility.cpp \
		$(UTIL_DIR)/PError.cpp \
		$(UTIL_DIR)/BinReloc.cpp \
		$(UTIL_DIR)/PEBLPath.cpp \
		$(UTIL_DIR)/PEBLHTTP.cpp \
		$(UTIL_DIR)/md5.cpp 
##		$(UTIL_DIR)/happyhttp.cpp \

EMUTILITIES_OBJ1  = $(patsubst %.cpp, %.o, $(EMUTILITIES_SRC))
EMUTILITIES_OBJ  = $(patsubst %.c, %.o, $(EMUTILITIES_OBJ1))   ##Get the .c file
EMUTILITIES_INC1  = $(patsubst %.cpp, %.h, $(EMUTILITIES_SRC))
EMUTILITIES_INC  = $(patsubst %.c, %.h, $(EMUTILITIES_INC1))   ##Get the plain .c file





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



EMBASE_SRCXX =	$(BASE_DIR)/Evaluator-es.cpp \
			$(BASE_DIR)/FunctionMap.cpp \
			$(BASE_DIR)/grammar.tab.cpp \
			$(BASE_DIR)/PEBLObject.cpp \
			$(BASE_DIR)/Loader.cpp \
			$(BASE_DIR)/PComplexData.cpp \
			$(BASE_DIR)/PList.cpp \
			$(BASE_DIR)/PNode.cpp \
			$(BASE_DIR)/VariableMap.cpp \
			$(BASE_DIR)/Variant.cpp \
			$(DEVICES_DIR)/PEventLoop-es.cpp 

EMBASE_OBJXX = $(patsubst %.cpp, %.o, $(EMBASE_SRCXX))


##This just collects plain .c files, 
PEBLBASE_SRC = lex.yy.c \
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


##EMSCRIPTEN-specific devices
EMDEVICES_SRC =   $(DEVICES_DIR)/PDevice.cpp \
	$(DEVICES_DIR)/PEventQueue.cpp \
	$(DEVICES_DIR)/PEvent.cpp\
	$(DEVICES_DIR)/PKeyboard.cpp \
	$(DEVICES_DIR)/PTimer.cpp \
	$(DEVICES_DIR)/DeviceState.cpp \
	$(DEVICES_DIR)/PStream.cpp \
	$(DEVICES_DIR)/PAudioOut.cpp \
	$(DEVICES_DIR)/PNetwork.cpp \
	$(DEVICES_DIR)/PJoystick.cpp 


EMDEVICES_OBJ  = $(patsubst %.cpp, %.o, $(EMDEVICES_SRC))
EMDEVICES_INC  = $(patsubst %.cpp, %.h, $(EMDEVICES_SRC))



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
#			$(SDL_DIR)/PlatformMovie.cpp 
#			$(SDL_DIR)/PlatformAudioIn.cpp \

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



VCG_MAKER_SRC = $(BASE_DIR)/VCG.cpp  $(PEBLBASE_SRCXX)  $(POBJECT_SRC) $(FUNCTIONLIB_SRC) $(PUTILITY_SRC)
VCG_MAKER_OBJ = $(patsubst %.cpp, %.o, $(VCG_MAKER_SRC))
VCG_MAKER_INC = $(patsubst %.cpp, %.h, $(VCG_MAKER_SRC))


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



EMMAIN_SRC =	$(APPS_DIR)/PEBL.cpp \
			$(EMBASE_SRCXX) \
			$(EMDEVICES_SRC) \
			$(FUNCTIONLIB_SRC) \
			$(POBJECT_SRC) \
			$(EMUTILITIES_SRC) \
			$(PLATFORM_SDL_SRC) 
###			$(LIB_SRC)

EMMAIN_OBJ = $(patsubst %.cpp, %.o, $(EMMAIN_SRC))
EMMAIN_INC = $(patsubst %.cpp, %.h, $(EMMAIN_SRC))


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


main-real:  $(DIRS) $(PEBLMAIN_OBJ) $(PEBLMAIN_INC)
	$(CXX) $(CXXFLAGS) -Wall -Wl,-rpath -Wl,LIBDIR $(DEBUGFLAGS) \
	-Wno-write-strings \
	-DPEBL_LINUX \
	$(SDL_FLAGS) -g	\
	-o $(BIN_DIR)/$(PEBLNAME) \
	$(BASE_DIR)/$(PEBLBASE_SRC) \
	$(patsubst %.o, $(OBJ_DIR)/%.o, $(PEBLMAIN_OBJ)) \
	-lSDL2 -lpthread -lSDL2_image -lSDL2_net -lSDL2_ttf -lSDL2_gfx  \
	-lpng  $(LINKOPTS)

##  -Wl,-V #verbose linking 
## -Wl,-rpath,/usr/lib \
 #	-s MAXIMUM_MEMORY=2147483648 \
##Make emscripten target (debug):
##Make optimized emscripten target (production):
em-opt-real:  $(DIRS) $(EMMAIN_OBJ) $(EMMAIN_INC)
	$(CXX) $(CXXFLAGS) \
	-Oz \
	-s WASM=1 \
	-s USE_SDL=2 \
	-s USE_SDL_NET=2 \
	-s USE_SDL_TTF=2 \
	-s USE_SDL_IMAGE=2 \
	-s SDL2_IMAGE_FORMATS='["png","jpeg","gif","bmp"]' \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s INITIAL_MEMORY=67108864 \
	-s MAXIMUM_MEMORY=4294967296 \
	-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","FS","callMain"]' \
	-s MODULARIZE=1 \
	-s EXPORT_NAME="createPEBLModule" \
	-s FETCH=1 \
	-s FORCE_FILESYSTEM=1 \
	-s ASSERTIONS=1 \
	-s ASYNCIFY=1 \
	-s ASYNCIFY_STACK_SIZE=524288 \
	-s ASYNCIFY_IMPORTS='["emscripten_sleep"]' \
	--pre-js emscripten/load-idbfs.js \
	-lidbfs.js \
	-DPEBL_EMSCRIPTEN \
	-o $(BIN_DIR)/pebl2.html \
	$(BASE_DIR)/lex.yy.c \
	$(patsubst %.o, $(OBJ_DIR)/%.o, $(EMMAIN_OBJ)) \
	libs/SDL2_gfx-1.0.4/build-em/SDL2_gfxPrimitives.o \
	--shell-file emscripten/shell_PEBL_debug.html \
	--preload-file upload-battery/@/usr/local/share/pebl2/battery \
	--preload-file emscripten/pebl-lib@/usr/local/share/pebl2/pebl-lib \
	--preload-file emscripten/media/@/usr/local/share/pebl2/media

##Make test emscripten target (for development/debugging):
em-test-real:  $(DIRS) $(EMMAIN_OBJ) $(EMMAIN_INC)
	$(CXX) $(CXXFLAGS) \
	-O0 \
	-s WASM=1 \
	-s USE_SDL=2 \
	-s USE_SDL_NET=2 \
	-s USE_SDL_TTF=2 \
	-s USE_SDL_IMAGE=2 \
	-s SDL2_IMAGE_FORMATS='["png","jpeg","gif","bmp"]' \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s INITIAL_MEMORY=67108864 \
	-s MAXIMUM_MEMORY=4294967296 \
	-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","FS","callMain"]' \
	-s MODULARIZE=1 \
	-s EXPORT_NAME="createPEBLModule" \
	-s FETCH=1 \
	-s FORCE_FILESYSTEM=1 \
	-s ASSERTIONS=1 \
	-s ASYNCIFY=1 \
	-s ASYNCIFY_STACK_SIZE=524288 \
	-s ASYNCIFY_IMPORTS='["emscripten_sleep"]' \
	--pre-js emscripten/load-idbfs.js \
	-lidbfs.js \
	-DPEBL_EMSCRIPTEN \
	-o $(BIN_DIR)/pebl2-test.html \
	$(BASE_DIR)/lex.yy.c \
	$(patsubst %.o, $(OBJ_DIR)/%.o, $(EMMAIN_OBJ)) \
	libs/SDL2_gfx-1.0.4/build-em/SDL2_gfxPrimitives.o \
	--shell-file emscripten/shell_PEBL_test.html \
	--preload-file test.pbl@/test.pbl \
	--preload-file emscripten/pebl-lib@/usr/local/share/pebl2/pebl-lib \
	--preload-file emscripten/media/@/usr/local/share/pebl2/media


#
#	 --embed-file demo/getnewsubnum.pbl@test.pbl \
#	 --embed-file pebl-lib@/usr/local/share/pebl2/pebl-lib \
#	 --embed-file media@/usr/local/share/pebl2/media 
#        -s TOTAL_MEMORY=52428800 \
#	 --preload-file pebl.png \
#	 --preload-file pebl.bmp \
#	 --preload-file DejaVuSans.ttf \
#	-Wno-write-strings \
#	-Wall -Wl,-rpath -Wl,LIBDIR $(DEBUGFLAGS) \
#	$(SDL_FLAGS) \
#	-lSDL2 -lpthread -lSDL2_image -lSDL2_net -lSDL2_ttf -lSDL2_gfx  \
#	-lpng \
#	$(LINKOPTS) 

##This packages the emscripten data/file/js. (NON-FUNCTIONAL - commented out)
#fp:
#	python $(FP) bin/pebl2.data --js-output=bin/pebl2-files.js \
#	--preload test.pbl \
#	--preload emscripten/pebl-lib@/usr/local/share/pebl2/pebl-lib \
#	--preload emscripten/media@/usr/local/share/pebl2/media 

doc: $(PEBL_DOCSRC)
	cd doc/pman; pdflatex main.tex
	cp doc/pman/main.pdf doc/pman/PEBLManual$(PEBL_VERSION).pdf

deb:    main doc
	epm -f deb $(PEBLNAME)

.PHONY: appimage
appimage: main
	@echo "========================================="
	@echo "Building PEBL AppImage"
	@echo "========================================="
	./build-appimage.sh $(PEBL_VERSION)

.PHONY: appimage-clean
appimage-clean:
	@echo "Cleaning AppImage build artifacts..."
	rm -rf AppDir
	rm -f *.AppImage
	rm -f bin/*.AppImage
	rm -f linuxdeploy-*.AppImage
	rm -rf squashfs-root
	rm -f bin/pebl2-appimage
	@echo "✓ AppImage artifacts cleaned"

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

#	-s USE_SDL=2 \   #this is for emscriten, which doesn't work.
#	-s USE_SDL_NET=2 \
#	-s USE_SDL_TTF=2 \
#	-s USE_SDL_IMAGE=2 \
#        -s SDL2_IMAGE_FORMATS='["png","jpeg","gif","bmp"]' \

#$(EM_SDL_FLAGS)

$(DIRS):
	@mkdir -p $@


dox: $(PEBLBASE_SRCXX)
	doxygen pebl.dox


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
	@echo "✓ Build artifacts cleaned"


.PHONY: install

uninstall:
	rm -Rf $(PREFIX)/bin/$(PEBLNAME)
	rm -Rf $(PREFIX)/share/$(PEBLNAME)

install:
	@if [ -z "$(DESTDIR)" ]; then $(MAKE) uninstall; fi

	install -d $(DESTDIR)$(PREFIX)/bin/

	cp bin/$(PEBLNAME) $(DESTDIR)$(PREFIX)/bin/$(PEBLNAME)
	rm -Rf $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)
	install -d $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)
	install -d $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/media
	install -d $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/pebl-lib
	install -d $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/doc
	install -d $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/battery
	install -d $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/demo
	install -d $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/tutorials

	cp -R tutorials/ $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/tutorials/
	cp -R media/* $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/media/
	cp -R demo/*  $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/demo/
	cp -R experiments/*  $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/demo/
	rm -rf `find $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/media -type d -name .svn`
	cp  pebl-lib/*.pbl $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/pebl-lib/
	cp doc/pman/PEBLManual$(PEBL_VERSION).pdf $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/doc
	cp bin/launcher.pbl $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/pebl-lib/
	cp pebl-lib/translatetest.pbl $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/pebl-lib/
	chmod -R uga+r $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/

	mkdir -p $(DESTDIR)$(PREFIX)/share/applications/
	sed -e '$(SEDLINE)' bin/PEBL2.desktop > PEBL2.desktop
	cp PEBL2.desktop $(DESTDIR)$(PREFIX)/share/applications/

	cp -R battery/* $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/battery
	cp battery/\.\.png $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/battery
	cp battery/\.\.about.txt $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/battery


	rm -rf `find $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/battery -type d -name .svn`
	rm -f `find $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/battery | grep \~`
	rm -Rf `find $(DESTDIR)$(PREFIX)/share/pebl/battery | grep 'data'`
	rm -f $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/battery/launch.bat
	rm -f $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/battery/PEBLLaunch-log.txt
	rm -f $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/battery/*.config
	rm -f $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/battery/makelinks-mac.sh
##Now, convert all the battery files to unix format.
	find $(DESTDIR)$(PREFIX)/share/$(PEBLNAME)/battery -name '*pbl' -exec dos2unix {} \;

ifeq (.depend,$(wildcard .depend))
include .depend
endif
