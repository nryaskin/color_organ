all: images library

BUILD_HOME=../..
INCLUDE_DIRS += -I./inc

BINARY = color_organ_main
LDLIBS += $(BUILD_HOME)/lib/visual.a

library: lib
	cp visual.a $(BUILD_HOME)/lib
lib: visual.o
	$(Q)$(AR) rcs -o visual.a -c visual.o

include $(BUILD_HOME)/src/Makefile.include
