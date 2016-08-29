# This software is part of OpenMono, see http://developer.openmono.com
# Released under the MIT license, see LICENSE.txt
MONO_PATH=/usr/local/openmono
include $(MONO_PATH)/predefines.mk

TARGET=weather

OBJECTS = \
	$(patsubst %.c,%.o,$(wildcard lib/*.c)) \
	$(patsubst %.cpp,%.o,$(wildcard lib/*.cpp)) \
	$(patsubst %.cpp,%.o,$(wildcard *.cpp))

OPTIMIZATION = -Os -Wno-unused-function

include $(MONO_PATH)/mono.mk
include Makefile.test

.PHONY: debug
debug:
	@echo OBJECTS = $(OBJECTS)
	@echo TARGET_OBJECTS = $(TARGET_OBJECTS)

.PHONY: realclean
realclean:
	$(RM) -r build/

