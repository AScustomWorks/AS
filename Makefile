RACK_DIR ?= ../..
SLUG = AS
VERSION = 0.6.0.2

FLAGS += -Idep/include
SOURCES += $(wildcard src/*.cpp freeverb/*.cpp)
DISTRIBUTABLES += $(wildcard LICENSE*) res

# Static libs
libsamplerate := dep/lib/libsamplerate.a
OBJECTS += $(libsamplerate)

# Dependencies
$(shell mkdir -p dep)
DEP_LOCAL := dep
DEPS += $(libsamplerate)

$(libsamplerate):

	cd dep && $(WGET) http://www.mega-nerd.com/SRC/libsamplerate-0.1.9.tar.gz
	cd dep && $(UNTAR) libsamplerate-0.1.9.tar.gz	
	cd dep/libsamplerate-0.1.9 && $(CONFIGURE)
	cd dep/libsamplerate-0.1.9/src && $(MAKE)
	cd dep/libsamplerate-0.1.9/src && $(MAKE) install

include $(RACK_DIR)/plugin.mk