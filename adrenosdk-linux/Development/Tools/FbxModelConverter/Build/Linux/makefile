PROJECT_NAME := FbxModelConverter

# Path to source
SRC_DIR := ../..
FRM_DIR := ../../../../Samples/Framework
FRM_PLATFORM_DIR := ../../../../Samples/Framework/Linux

# BUILD object files
BUILD_DIR := ../../../../../Intermediate/Tools/$(PROJECT_NAME)/Linux

# executable goes to Bin
TARGET_DIR := ../../../../../Bin
TARGET = $(TARGET_DIR)/$(PROJECT_NAME)

CFLAGS := -w -O3 -pipe -g -fPIC

CPPFILES := $(notdir $(wildcard $(SRC_DIR)/*.cpp))
CPPFILES += $(notdir $(wildcard $(FRM_DIR)/*.cpp))
OBJECTS := $(addprefix $(BUILD_DIR)/, $(CPPFILES:.cpp=.o)) 

INCLUDES := -I$(FRM_DIR)

# force rebuild. Comment to take out a force rebuild.
.PHONY: clean

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	g++ $(INCLUDES) -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJECTS)
	mkdir -p $(dir $@)
	g++ $(INCLUDES) $(CFLAGS) -o $@ $(OBJECTS)

clean:
	rm -f $(TARGET) $(OBJECTS)


