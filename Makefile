RACK_DIR ?= ../..
SLUG = AS
VERSION = 0.6.4

FLAGS +=
SOURCES += $(wildcard src/*.cpp freeverb/*.cpp)
DISTRIBUTABLES += $(wildcard LICENSE*) res

include $(RACK_DIR)/plugin.mk