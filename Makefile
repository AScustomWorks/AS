# If RACK_DIR is not defined when calling the Makefile, default to two levels above
RACK_DIR ?= ../..

# Must follow the format in the Naming section of https://vcvrack.com/manual/PluginDevelopmentTutorial.html
SLUG = AS

# Must follow the format in the Versioning section of https://vcvrack.com/manual/PluginDevelopmentTutorial.html
VERSION = 0.6.0

# FLAGS will be passed to both the C and C++ compiler
FLAGS += -Idep/include

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine.
LDFLAGS +=

# Add .cpp and .c files to the build
SOURCES += $(wildcard src/*.cpp freeverb/*.cpp)

# Add files to the ZIP package when running `make dist`
# The compiled plugin is automatically added.
DISTRIBUTABLES += $(wildcard LICENSE*) res

libsamplerate = dep/lib/libsamplerate.a
OBJECTS += $(libsamplerate)

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk

# Dependencies

$(shell mkdir -p dep)
DEP_FLAGS += -fPIC
DEP_LOCAL := dep

DEPS += $(libsamplerate)
include $(RACK_DIR)/dep.mk

$(libsamplerate):
	cd dep && $(WGET) http://www.mega-nerd.com/SRC/libsamplerate-0.1.9.tar.gz
	cd dep && $(UNTAR) libsamplerate-0.1.9.tar.gz
	cd dep/libsamplerate-0.1.9 && $(CONFIGURE)
	cd dep/libsamplerate-0.1.9 && $(MAKE)
	cd dep/libsamplerate-0.1.9 && $(MAKE) install