# Put @ to show the command, put nothing otherwise
SHOW := @


################### functions ###################

# Function to get all files in subdirectories
define GET_FILES
	$(shell find -L $(1) -type f -name '$(2)')
endef

# $(1) : lib name
# $(2) : lib directory (source)
# $(3) : cmake options
# $(4) : lib object pattern (*.a, *.so*, *.o)
define COMPILE_LIB
	$(eval ARG_1 := $(strip $(1)))
	$(eval ARG_2 := $(strip $(2)))
	$(eval ARG_3 := $(strip $(3)))
	$(eval ARG_4 := $(strip $(4)))

	$(SHOW)echo "Building $(ARG_1)"

	$(SHOW)cmake -S $(ARG_2) -B $(ARG_2)/build -Wno-dev $(ARG_3)
	$(SHOW)cmake --build $(ARG_2)/build/

	$(SHOW)mkdir -p $(LIBS_OBJ_DIR)
	$(SHOW)ln -sfr $(ARG_2)/build/$(ARG_4) $(LIBS_OBJ_DIR)
endef


################### directories ###################

# Current working directory
CWD := $(shell pwd)
# directory of .cpp and .hpp source files
SRC_DIR := $(CWD)/sources
# build directory
BUILD_DIR := $(CWD)/build
# project objects directory (contains .o files)
OBJ_DIR := $(BUILD_DIR)/object
# libraries objects directory (contains .so, .a, .o, etc...)
LIBS_OBJ_DIR := $(BUILD_DIR)/libs
# project dependencies directory (contains .d files)
DEPS_DIR := $(OBJ_DIR)
# submodules directory (libraries that the project depends)
SUBMODULES:=$(CWD)/submodules
# libraries include directory
LIBS_INC_DIR := $(SUBMODULES)/include
# libraries sources to build directory
LIBS_SRC_DIR := $(SUBMODULES)/sources
# Exectuable file
EXEC := $(CWD)/app.out


################### project files ###################

# Get all cpp files inside sources directory
SRC := $(call GET_FILES,$(SRC_DIR),*.cpp)
# Erase the path of the files
SRC := $(subst $(SRC_DIR)/,,$(SRC))

# Project object files have same name than source files
OBJ := $(patsubst %.cpp,%.o,$(SRC))
# Add the object directory to the object files
OBJ := $(addprefix $(OBJ_DIR)/,$(subst /,-,$(OBJ)))
# Dependencies files have same name than object files
DEPS := $(patsubst %.o,%.d,$(OBJ))


################### compiler ###################

# Default compiler if not specified
ifeq ($(CC),)
CC := gcc
endif
ifeq ($(CXX),)
CXX := g++
endif

WARNINGS := \
-pedantic -Wpedantic -pedantic-errors \
-Wall -Wextra \
-Wcast-align \
-Wcast-qual \
-Wctor-dtor-privacy \
-Wdisabled-optimization \
-Wformat=2 \
-Winit-self \
-Wmissing-declarations \
-Wmissing-include-dirs \
-Wold-style-cast \
-Woverloaded-virtual \
-Wvirtual-inheritance  \
-Wredundant-decls \
-Wshadow \
-Wsign-conversion \
-Wsign-promo \
-Wundef \
-Wno-unused \
-Wconversion \
-Wformat-nonliteral \
-Wformat=2 \
-Wformat-security  \
-Wformat-y2k \
-Wimport \
-Winline \
-Winvalid-pch \
-Wmissing-field-initializers \
-Wmissing-format-attribute   \
-Wmissing-noreturn \
-Wpacked \
-Wpointer-arith \
-Wstack-protector \
-Wstrict-aliasing=2  \
-Wunreachable-code \
-Wunused \
-Wvariadic-macros \
-Wwrite-strings \
-Weffc++ \
-Werror \
-Wunused-parameter \
-Wlong-long \
-Wsuggest-override \
-fno-common \

ifeq ($(CXX),g++)
WARNINGS:=$(WARNINGS) \
			-Wlogical-op \
			-Wnoexcept \
			-Wstrict-null-sentinel \
			-Wformat-truncation \
			-Wno-long-long \
			-Wno-undef
endif

# Library cpp flags for compilation
COMPILE_FLAGS_LIBS := -std=c++20 -O0 -g -c
# Project cpp flags for compilation
COMPILE_FLAGS := $(WARNINGS) $(COMPILE_FLAGS_LIBS) -MMD -MP
# -g => Generate debug information
# -c => Doesn't create WinMain error if there is no main in the file
# -O0 => No optmization, faster compilation time, better for debugging builds
# -MMD => Create .d files for dependencies of users files only (not system files)
# -MP => Handle renamed or missing files for dependency


################### rules ###################

all : build

buildrun : build run

remake: clean buildrun

build : init_build compile_libs compile link
compile : $(OBJ)
link : $(EXEC)

run :
	$(SHOW)echo "Running $(EXEC)"
	$(SHOW)$(EXEC)

debug :
	$(SHOW)echo "Debugging $(EXEC)"
	$(SHOW)gdb -quiet $(EXEC)

init_build:
	$(SHOW)echo "Create Build Directories"
	$(SHOW)mkdir -p $(BUILD_DIR)
	$(SHOW)mkdir -p $(OBJ_DIR)
	$(SHOW)mkdir -p $(LIBS_OBJ_DIR)
	$(SHOW)mkdir -p $(DEPS_DIR)

cleanall : clean clean_libs

clean : clean_exec
	$(SHOW)rm -rf $(BUILD_DIR)
	$(SHOW)echo "Clean Project"
	$(SHOW)rm -rf $(OBJ_DIR)
	$(SHOW)rm -rf $(DEPS_DIR)

clean_exec:
	$(SHOW)echo "Clean Executable"
	$(SHOW)rm -rf $(EXEC)

clean_libs:
	$(SHOW)echo "Clean Submodules"
	$(SHOW)rm -rf $(SUBMODULES)/**/build
	$(SHOW)rm -rf $(LIBS_OBJ_DIR)

format:
	$(SHOW)clang-format -i --verbose -style=file:.clang-format \
	$(call GET_FILES,$(SRC_DIR),*.cpp) \
	$(call GET_FILES,$(SRC_DIR),*.hpp) \
	$(call GET_FILES,$(SRC_DIR),*.tpp)


################### libraries rules ###################

IMGUI_DIR:=$(SUBMODULES)/imgui
IMGUI_SRC:=$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_demo.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/imgui_impl_sdl2.cpp \
	$(IMGUI_DIR)/imgui_impl_opengl3.cpp
IMGUI_OBJ:=$(patsubst %.cpp,%.o,$(IMGUI_SRC))
IMGUI_OBJ:=$(subst $(IMGUI_DIR),$(LIBS_OBJ_DIR),$(IMGUI_OBJ))

SDL_REQ:=$(LIBS_OBJ_DIR)/libSDL2.a $(LIBS_OBJ_DIR)/libSDL2main.a

compile_libs : $(SDL_REQ) $(IMGUI_OBJ)

$(IMGUI_OBJ) : $(LIBS_OBJ_DIR)/%.o : $(IMGUI_DIR)/%.cpp
	$(SHOW)echo "Compile Library $@"
	$(SHOW)$(CXX) $(COMPILE_FLAGS_LIBS) $< -o $@ -I"$(LIBS_INC_DIR)/SDL"

$(SDL_REQ) :
	$(call COMPILE_LIB, SDL \
		,$(SUBMODULES)/SDL\
		, -D CMAKE_C_COMPILER=$(CC) \
		-D CMAKE_CXX_COMPILER=$(CXX) \
		-D CMAKE_BUILD_TYPE=Release \
		-D SDL_TEST=0 \
		-D SDL_TESTS=0 \
		-D SDL_STATIC=1 \
		-D SDL_STATIC_PIC=1 \
		, *.a \
	)


################### compile rules ###################

INCLUDES:=-I"$(SRC_DIR)" -isystem"$(LIBS_INC_DIR)"
LIBRARIES:=$(IMGUI_OBJ) -L"$(LIBS_OBJ_DIR)" -lSDL2 -lSDL2main -lOpenGL

# set up the dependencies
-include $(DEPS)

# Creating the object files of the project
.SECONDEXPANSION:
$(OBJ) : $(OBJ_DIR)/%.o : $(SRC_DIR)/$$(subst -,/,%).cpp
	$(SHOW)echo "Compile $(subst $(SRC_DIR)/,,$<)"
	$(SHOW)$(CXX) $(COMPILE_FLAGS) $< -o $@ $(INCLUDES)

# Create the executable by Linking all the object files and the libraries together
$(EXEC) : $(OBJ)
	$(SHOW)echo "Building and Linking $@"
	$(SHOW)$(CXX) $^ -o $@ $(LIBRARIES)

nothing: