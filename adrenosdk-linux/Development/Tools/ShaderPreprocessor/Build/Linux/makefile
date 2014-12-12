PROJECT_NAME := ShaderPreprocessor

# Path to source
SRC_DIR := ../..

# BUILD object files
BUILD_DIR := ../../../../../Intermediate/Tools/ShaderPreprocessor/ubuntu
INCLUDES := -I../../../../Inc

# executable goes to Bin
TARGET_DIR := ../../../../../Bin
TARGET = $(TARGET_DIR)/$(PROJECT_NAME)

CFLAGS := -w -O3 -pipe -g -fPIC

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
