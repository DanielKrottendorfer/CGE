CXX      := g++
CXXFLAGS := 
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
INCLUDE  := -I include
LDFLAGS  := -L lib -lglew32 -lfreeglut -lopengl32
DEMO      := $(wildcard ./demo/*.cpp)
TARGET   := program

all: create_dir build_demo run

build_demo: $(APP_DIR)/$(TARGET)
	
$(APP_DIR)/$(TARGET): $(DEMO) 
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $(wildcard ./build/objects/*.o) $(LDFLAGS)

$(DEMO): $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $@ -o $</$(notdir $(basename $@)).o  $(LDFLAGS)

run:
	$(APP_DIR)/$(TARGET).exe

clean:
	rd /s /q ".\build"

create_dir: 
	if not exist ".\build" mkdir .\build
	if not exist ".\build\apps" mkdir .\build\apps
	if not exist ".\build\objects" mkdir .\build\objects