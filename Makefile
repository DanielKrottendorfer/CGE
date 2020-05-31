CXX      := g++
CXXFLAGS := 
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
INCLUDE  := -I include
LDFLAGS  := -L lib -lglew32 -lfreeglut -lopengl32
DEMO      := $(wildcard ./src/*.cpp)
TARGET   := program

all: clean create_dir build_demo run

build_demo: $(TARGET)
	
$(TARGET): $(DEMO) 
	$(CXX) $(CXXFLAGS) -o $@ $(wildcard ./build/objects/*.o) $(LDFLAGS)

$(DEMO): $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $@ -o $</$(notdir $(basename $@)).o  $(LDFLAGS)

run:
	$(TARGET).exe

clean:
	if exist ".\build" rd /s /q ".\build"

create_dir: 
	if not exist ".\build" mkdir .\build
	if not exist ".\build\apps" mkdir .\build\apps
	if not exist ".\build\objects" mkdir .\build\objects