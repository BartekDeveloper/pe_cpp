# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Define the project name and languages used
# CMake will automatically detect the correct compilers (gcc, g++, nasm)
project(PE_Project C CXX ASM)

# Define variables for directories
set(EXE_NAME pe)
set(SRC_DIRS src)
set(DOCS_DIR docs)
set(INCLUDE_DIR external/include)
set(STATIC_DIR external/static)
set(DYNAMIC_DIR external/dynamic)

# --- Source File Discovery ---
# Find C, C++, and Assembly source files recursively in the 'src' directory.
# file(GLOB_RECURSE) is used for direct translation of Makefile's wildcard,
# though for larger projects, explicitly listing sources or using target_sources
# with CONFIGURE_DEPENDS is often preferred for better dependency tracking.
file(GLOB_RECURSE SRC_DIR_SOURCES_C "${SRC_DIRS}/*.c" "${SRC_DIRS}/**/*.c")
file(GLOB_RECURSE SRC_DIR_SOURCES_CPP "${SRC_DIRS}/*.cpp" "${SRC_DIRS}/**/*.cpp" "${SRC_DIRS}/*.cc" "${SRC_DIRS}/**/*.cc")
file(GLOB_RECURSE SRC_DIR_SOURCES_ASM "${SRC_DIRS}/*.s" "${SRC_DIRS}/**/*.s")

# Check for main.c or main.cpp in the project root, as per the Makefile's logic.
set(MAIN_SOURCE "")
if (EXISTS "${CMAKE_SOURCE_DIR}/main.c")
    set(MAIN_SOURCE "${CMAKE_SOURCE_DIR}/main.c")
elseif (EXISTS "${CMAKE_SOURCE_DIR}/main.cpp")
    set(MAIN_SOURCE "${CMAKE_SOURCE_DIR}/main.cpp")
endif()

# Combine all discovered sources into a single list
list(APPEND ALL_SOURCES ${SRC_DIR_SOURCES_C} ${SRC_DIR_SOURCES_CPP} ${SRC_DIR_SOURCES_ASM})
if (MAIN_SOURCE)
    list(APPEND ALL_SOURCES ${MAIN_SOURCE})
endif()

# --- Executable Definition ---
# Add the executable target using all collected source files.
# CMake automatically determines the linker based on the source files.
add_executable(${EXE_NAME} ${ALL_SOURCES})

# --- Compiler Standards ---
# Set C and C++ language standards for the target.
target_compile_features(${EXE_NAME} PUBLIC c_std_23)
target_compile_features(${EXE_NAME} PUBLIC cxx_std_23)

# --- Include Directories ---
# Add the external include directory to the target's include paths.
target_include_directories(${EXE_NAME} PUBLIC ${INCLUDE_DIR})

# --- Compiler Options (Debug/Release) ---
# Apply compiler flags based on the build configuration (Debug or Release).
# -g -O0 for Debug, -O3 for Release.
target_compile_options(${EXE_NAME} PRIVATE
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3>
)

# Apply C++ specific flags (fconstexpr-ops-limit)
target_compile_options(${EXE_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:$<$<CONFIG:Debug>:-fconstexpr-ops-limit=335544321>>
    $<$<COMPILE_LANGUAGE:CXX>:$<$<CONFIG:Release>:-fconstexpr-ops-limit=335544321>>
)

# --- Linker Directories and Libraries ---
# Find static libraries (.a) in the specified directory.
file(GLOB STATIC_LIBS_FULL_PATH "${STATIC_DIR}/*.a*")
foreach(LIB_PATH ${STATIC_LIBS_FULL_PATH})
    get_filename_component(LIB_NAME ${LIB_PATH} NAME_WE) # Get filename without extension
    string(REGEX REPLACE "^lib" "" LIB_NAME ${LIB_NAME}) # Remove 'lib' prefix (e.g., libfoo.a -> foo)
    list(APPEND STATIC_LIBS_NAMES ${LIB_NAME})
endforeach()

# Find dynamic libraries (.so) in the specified directory.
file(GLOB DYNAMIC_LIBS_FULL_PATH "${DYNAMIC_DIR}/*.so*")
foreach(LIB_PATH ${DYNAMIC_LIBS_FULL_PATH})
    get_filename_component(LIB_NAME ${LIB_PATH} NAME_WE) # Get filename without extension
    string(REGEX REPLACE "^lib" "" LIB_NAME ${LIB_NAME}) # Remove 'lib' prefix (e.g., libbar.so -> bar)
    list(APPEND DYNAMIC_LIBS_NAMES ${LIB_NAME})
endforeach()

# Add directories where the linker should search for libraries.
link_directories(${STATIC_DIR} ${DYNAMIC_DIR})

# Link the discovered libraries to the executable target.
target_link_libraries(${EXE_NAME} PUBLIC ${STATIC_LIBS_NAMES} ${DYNAMIC_LIBS_NAMES})

# --- Custom Targets (Mapping Makefile rules to CMake) ---

# 'make_dirs': CMake automatically creates necessary build directories, so no explicit target is needed.
# 'clean': CMake provides a 'clean' target by default (e.g., 'cmake --build . --target clean').

# Target for generating interface files using a script.
# 'ALL' keyword makes this target part of the default build, like in the Makefile.
add_custom_target(interfaces ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory interfaces # Ensure the directory exists
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/make_interfaces.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating interface files"
)

# Target for cleaning generated interface files.
add_custom_target(clean_interfaces
    COMMAND ${CMAKE_COMMAND} -E remove_directory interfaces
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Cleaning interface files"
)

# Target for generating documentation using Doxygen.
# Requires Doxygen to be installed and found by CMake.
find_package(Doxygen)
if (DOXYGEN_FOUND)
    add_custom_target(docs ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile # Assumes Doxyfile exists in project root
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating documentation with Doxygen"
    )
else()
    message(WARNING "Doxygen not found. The 'docs' target will not be available.")
endif()

# Target for adding copyright headers to source files using a script.
add_custom_target(copyright
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/add_copyright.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Adding copyright to source files"
)

# Target for removing copyright headers from source files using a script.
add_custom_target(clean_copyright
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/remove_copyright.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Removing copyright from source files"
)

# Target to build the project in Debug mode and then run the executable.
add_custom_target(run
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${EXE_NAME} --config Debug
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/Debug/${EXE_NAME}
    COMMENT "Building in Debug mode and running the executable"
)

# Target to build the project in Release mode and then run the executable.
add_custom_target(release_run
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${EXE_NAME} --config Release
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/Release/${EXE_NAME}
    COMMENT "Building in Release mode and running the executable"
)

# Target to build the project in Debug mode, launch GDB, and run the executable.
add_custom_target(gdb
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${EXE_NAME} --config Debug
    COMMAND gdb -ex 'run' ${CMAKE_CURRENT_BINARY_DIR}/Debug/${EXE_NAME}
    COMMENT "Building in Debug mode, launching GDB, and running the program"
)

# Target to print a help message for CMake commands and custom targets.
add_custom_target(help
    COMMAND ${CMAKE_COMMAND} -E echo "--- CMake Build Commands ---"
    COMMAND ${CMAKE_COMMAND} -E echo "  To configure (e.g., for Debug): cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug"
    COMMAND ${CMAKE_COMMAND} -E echo "  To build (after configuring): cmake --build build"
    COMMAND ${CMAKE_COMMAND} -E echo "  To build a specific configuration: cmake --build build --config <Debug|Release>"
    COMMAND ${CMAKE_COMMAND} -E echo "  To clean build artifacts: cmake --build build --target clean"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "--- Custom Targets (run with 'cmake --build build --target <target_name>') ---"
    COMMAND ${CMAKE_COMMAND} -E echo "  interfaces: Generate interface files."
    COMMAND ${CMAKE_COMMAND} -E echo "  clean_interfaces: Remove interface files."
    COMMAND ${CMAKE_COMMAND} -E echo "  docs: Generate documentation with Doxygen."
    COMMAND ${CMAKE_COMMAND} -E echo "  copyright: Add copyright to source files."
    COMMAND ${CMAKE_COMMAND} -E echo "  clean_copyright: Remove copyright from source files."
    COMMAND ${CMAKE_COMMAND} -E echo "  run: Build in Debug and run the executable."
    COMMAND ${CMAKE_COMMAND} -E echo "  release_run: Build in Release and run the executable."
    COMMAND ${CMAKE_COMMAND} -E echo "  gdb: Build in Debug, launch GDB, and run the program."
    COMMENT "Prints help message for CMake commands and custom targets."
)

