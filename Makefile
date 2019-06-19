RACK_DIR ?= ../..

FLAGS +=
#FLAGS += -w
SOURCES += $(wildcard src/*.cpp freeverb/*.cpp)
DISTRIBUTABLES += $(wildcard LICENSE*) res

include $(RACK_DIR)/plugin.mk