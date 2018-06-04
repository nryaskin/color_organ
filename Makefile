all: images library

BUILD_HOME=../..
INCLUDE_DIRS += -I./inc

BINARY = color_organ_main
LDLIBS += $(BUILD_HOME)/lib/visual.a $(BUILD_HOME)/lib/microphone_driver.a

library: lib_microphone lib_visual 
	cp visual.a $(BUILD_HOME)/lib
	cp microphone_driver.a $(BUILD_HOME)/lib
lib_visual: visual.o
	$(Q)$(AR) rcs -o visual.a -c visual.o
lib_microphone: microphone_driver.o
	$(Q)$(AR) rcs -o microphone_driver.a -c microphone_driver.o
include $(BUILD_HOME)/src/Makefile.include
