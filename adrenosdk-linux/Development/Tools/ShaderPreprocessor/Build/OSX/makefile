


ifeq ($(ARCH),x86)
   $(info == x86 == )
   ARCH_FLAGS := -m32 -arch i386
else
   ARCH:=x64
   $(info == x64 == )
   ARCH_FLAGS := -m64 -arch x86_64
   EXTRA_ARCH_PATHING:=/x64
endif

ifeq ($(MODE),debug)
   $(info == DEBUG == )
   CFLAGS := -pipe -g -fPIC -m32 -arch i386
   TARGET_DIR := ../../../../../Bin_d$(EXTRA_ARCH_PATHING)
   PROJECT_NAME := ShaderPreprocessor_d
else
   MODE:=release
   $(info == RELEASE == )
   CFLAGS := -w -O3 -pipe -g -fPIC -m32 -arch i386
   TARGET_DIR := ../../../../../Bin$(EXTRA_ARCH_PATHING)
   PROJECT_NAME := ShaderPreprocessor
endif




# Path to source
SRC_DIR := ../..

# BUILD object files
BUILD_DIR := ../../../../../Intermediate/Tools/ShaderPreprocessor/OSX/$(ARCH)
CROSSPLATFORM_DIR := ../../../../Inc
TEXTURECONVERTERHEADER_DIR:= ../../../../Tools/TextureConverter/inc
INCLUDES := -I$(CROSSPLATFORM_DIR) -I$(TEXTURECONVERTERHEADER_DIR)

# executable goes to Bin
TARGET = $(TARGET_DIR)/$(PROJECT_NAME)


CPPFILES := ShaderPreprocessor.cpp
OBJECTS := $(addprefix $(BUILD_DIR)/, $(CPPFILES:.cpp=.o)) 

# force rebuild. Comment to take out a force rebuild.
.PHONY: clean

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	g++ -c $(CFLAGS) $(INCLUDES) $< -o $@

$(TARGET): $(OBJECTS)
	mkdir -p $(dir $@)
	g++ $(CFLAGS) $(INCLUDES) -o $@ $(OBJECTS)

clean:
	rm -f $(TARGET) $(OBJECTS)


rebuild_all: clean $(TARGET)
