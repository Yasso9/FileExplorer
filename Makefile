############################## USER CUSTOM ##############################

# Put @ if we should not show the command, put nothing otherwise
SHOW := @

# Function to get all files in subdirectories
define GET_FILES
	$(shell find $(1) -type f -name '$(2)')
endef

# $(1) : lib name
# $(2) : lib directory (source)
# $(3) : lib requirement (.a, .so, .o, etc...)
# $(4) : cmake options
# $(5) : lib object pattern (*.a, *.so*, *.o)
define COMPILE_LIB
$(3) :
	$(SHOW)echo "Building $(1)"

	$(SHOW)cmake -S $(2) -B $(2)/build -Wno-dev $(4)
	$(SHOW)cmake --build $(2)/build/

	$(SHOW)mkdir -p $(OBJ_LIBS_DIR)
	$(SHOW)ln -sfr $(2)/build/$(5) $(OBJ_LIBS_DIR)
endef



############################## OS detection ##############################

# Know in what operating system we are
ifeq ($(OS),Windows_NT)
DETECTED_OS := Windows
else
DETECTED_OS := $(shell uname)
endif

ifneq ($(DETECTED_OS),Windows)
ifneq ($(DETECTED_OS),Linux)
$(error ERROR : OS UNDETECTED)
endif
endif



############################## Global Informations ##############################

# Current working directory
CWD := $(shell pwd)
# directory of .cpp and .hpp files
SOURCES_DIR := $(CWD)/sources
# directory of files needed to build executable and the executable itself
BUILD_DIR := $(CWD)/build
# directory of .o (object) files of project
OBJ_DIR := $(BUILD_DIR)/object
# .so files directory
OBJ_LIBS_DIR := $(BUILD_DIR)/libs
# directory of .d (dependency) files of project
DEPS_DIR := $(OBJ_DIR)
# Submodules directory
SUBMODULES:=$(CWD)/submodules
# directory of libraries that the project depends
LIBS_INC := $(SUBMODULES)/includes
# Exectuable path and name
EXEC := $(CWD)/application.out



############################## files and object location ##############################

# Get all cpp files inside sources => "./sources/sub_directory/filename.cpp"
SOURCES_FILES := $(call GET_FILES,$(SOURCES_DIR),*.cpp)
# Erase files directory => "sub_directory/filename.cpp"
SOURCES_FILES := $(subst $(SOURCES_DIR)/,,$(SOURCES_FILES))

# Object files of the all cpp files of the project that we need to generate (or are already generated)
# sub_directory/filename.cpp => sub_directory/filename.o
OBJ_FILES := $(patsubst %.cpp,%.o,$(SOURCES_FILES))
# => ./object/sub_directory-filename.o
OBJ_FILES := $(addprefix $(OBJ_DIR)/,$(subst /,-,$(OBJ_FILES)))

# Dependencies (.d files) have same name than object files
DEPS_FILES := $(patsubst %.o,%.d,$(OBJ_FILES))



############################## libraries ##############################

# IMGUI_SRC:=$(SUBMODULES)/imgui-sfml
# IMGUI_REQUIREMENTS:=$(OBJ_LIBS_DIR)/libImGui-SFML.a

LIBS_REQUIRE:=$(IMGUI_REQUIREMENTS)

############################## COMPILER SETUP ##############################

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

COMPILE_FLAGS_LIBS := -std=c++20 -O0 -g -c
COMPILE_FLAGS := $(WARNINGS) $(COMPILE_FLAGS_LIBS) -MMD -MP
LINK_FLAGS :=
# -g => Generate debug information
# -c => Doesn't create WinMain error if there is no main in the file
# -O0 => No optmization, faster compilation time, better for debugging builds
# -MMD => Create .d files for dependencies of users files only (not system files)
# -MP => Handle renamed or missing files for dependency



############################## call action ##############################

all : build

buildrun : build run

remake: clean buildrun

build : init_build compile_libs compile link
compile : $(OBJ_FILES)
compile_libs : $(LIBS_REQUIRE)
link : $(EXEC)

# Run exectutable
run :
	$(SHOW)echo "Running $(EXEC)"
ifeq ($(DETECTED_OS),Linux)
	$(SHOW)LD_LIBRARY_PATH="$(OBJ_LIBS_DIR)" $(EXEC)
else
	$(SHOW)$(EXEC)
endif

debug :
ifeq ($(DETECTED_OS),Linux)
	LD_LIBRARY_PATH="$(OBJ_LIBS_DIR)" gdb -quiet $(EXEC)
else
	gdb -quiet $(EXEC)
endif

init_build:
	$(SHOW)echo "Create Build Directories"
	$(SHOW)mkdir -p $(BUILD_DIR)
	$(SHOW)mkdir -p $(OBJ_DIR)
	$(SHOW)mkdir -p $(OBJ_LIBS_DIR)
	$(SHOW)mkdir -p $(DEPS_DIR)

cleanall : clean clean_libs

clean : clean_exec
	$(SHOW)rm -rf $(BUILD_DIR)
	$(SHOW)echo "Clean Project"
	$(SHOW)rm -rf $(OBJ_DIR)
	$(SHOW)rm -rf $(OBJ_LIBS_DIR)
	$(SHOW)rm -rf $(DEPS_DIR)

clean_exec:
	$(SHOW)echo "Clean Executable"
	$(SHOW)rm -rf $(EXEC)

clean_libs:
	$(SHOW)echo "Clean Submodules"
	$(SHOW)rm -rf $(SUBMODULES)/**/build

# Format all files of the project with clang format
format:
	$(SHOW)clang-format -i --verbose -style=file:.clang-format \
	$(call GET_FILES,$(SOURCES_DIR),*.cpp) \
	$(call GET_FILES,$(SOURCES_DIR),*.hpp) \
	$(call GET_FILES,$(SOURCES_DIR),*.tpp)

nothing:



############################## project compilation ##############################

INCLUDES := -I"$(SOURCES_DIR)" -isystem"$(LIBS_INC)"
LIBRARIES := -L"$(OBJ_LIBS_DIR)"

# set up the dependencies
-include $(DEPS_FILES)

# Creating the object files of the project
.SECONDEXPANSION:
$(OBJ_FILES) : $(OBJ_DIR)/%.o : $(SOURCES_DIR)/$$(subst -,/,%).cpp
	$(SHOW)echo "Compile $(subst $(SOURCES_DIR)/,,$<)"
	$(SHOW)$(CXX) $(COMPILE_FLAGS) $< -o $@ $(INCLUDES)

# Create the executable by Linking all the object files and the libraries together
$(EXEC) : $(OBJ_FILES)
	$(SHOW)echo "Building and Linking $@"
	$(SHOW)$(CXX) $(LINK_FLAGS) $^ -o $@ $(LIBRARIES)
