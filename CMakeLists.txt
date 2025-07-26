# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Define the project name and languages used
# CMake will automatically detect the correct compilers (gcc, g++, nasm)
project(PE_Project C CXX ASM)

# Define variables for directories
set(EXE_NAME pe)
set(SRC_DIRS src)
set(DOCS_DIR docs) # This is a source directory for your Doxyfile, not build output
set(INCLUDE_DIR external/include)
set(STATIC_DIR external/static)
set(DYNAMIC_DIR external/dynamic)

# --- Source File Discovery ---
# Find C, C++, and Assembly source files recursively in the 'src' directory.
# Using recursive globs for direct translation of Makefile's wildcard.
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

# --- Output Directory for Executable/Libraries ---
# This controls where the final 'pe' executable will be placed within the build directory.
# For example, if you configure with `cmake -S . -B out`, then 'pe' will go into `out/debug` or `out/release`.
set_target_properties(${EXE_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>" # For static libraries if any, though you only have an exe
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>" # For shared libraries if any
)
# REMOVED: set(CMAKE_CURRENT_BINARY_DIR ./out/) -- This variable is read-only and incorrect for setting output paths.

# --- Compiler Standards ---
# Set C and C++ language standards for the target.
target_compile_features(${EXE_NAME} PUBLIC c_std_23)
target_compile_features(${EXE_NAME} PUBLIC cxx_std_23)

# --- Include Directories ---
# Add the external include directory to the target's include paths.
target_include_directories(${EXE_NAME} PUBLIC ${INCLUDE_DIR})

# --- Compiler Options (debug/release) ---
# Apply compiler flags based on the build configuration (debug or release).
# -g -O0 for debug, -O3 for release.
target_compile_options(${EXE_NAME} PRIVATE
    $<$<CONFIG:debug>:-g -O0>
    $<$<CONFIG:release>:-O3>
)

# Apply C++ specific flags (fconstexpr-ops-limit)
target_compile_options(${EXE_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:$<$<CONFIG:debug>:-fconstexpr-ops-limit=335544321>>
    $<$<COMPILE_LANGUAGE:CXX>:$<$<CONFIG:release>:-fconstexpr-ops-limit=335544321>>
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
# IMPORTANT: Removed 'ALL'. This target will NOT run by default.
# It will run when 'pe' executable is built, due to add_dependencies below.
add_custom_target(interfaces
    COMMAND ${CMAKE_COMMAND} -E make_directory interfaces # Ensure the directory exists in source tree
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/make_interfaces.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating interface files"
)
# Make the 'pe' executable DEPEND on 'interfaces'. This means 'interfaces' will run *before* 'pe' builds.
add_dependencies(${EXE_NAME} interfaces)


# Target for cleaning generated interface files.
add_custom_target(clean_interfaces
    COMMAND ${CMAKE_COMMAND} -E remove_directory interfaces
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Cleaning interface files"
)

# Target for generating documentation using Doxygen.
# IMPORTANT: Removed 'ALL'. This target will NOT run by default.
find_package(Doxygen)
if (DOXYGEN_FOUND)
    add_custom_target(docs
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

# Target to build the project in debug mode and then run the executable.
add_custom_target(run
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${EXE_NAME} --config debug
    COMMAND $<TARGET_FILE:${EXE_NAME}> # This generator expression gets the full path to the built executable
    COMMENT "Building in debug mode and running the executable"
)

# Target to build the project in release mode and then run the executable.
add_custom_target(release_run
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${EXE_NAME} --config release
    COMMAND $<TARGET_FILE:${EXE_NAME}> # This generator expression gets the full path to the built executable
    COMMENT "Building in release mode and running the executable"
)

# Target to build the project in debug mode, launch GDB, and run the executable.
add_custom_target(gdb
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${EXE_NAME} --config debug
    COMMAND gdb -ex 'run' $<TARGET_FILE:${EXE_NAME}> # This generator expression gets the full path to the built executable
    COMMENT "Building in debug mode, launching GDB, and running the program"
)

add_custom_target(help_me
    COMMAND ${CMAKE_COMMAND} -E echo "  --- CMake Build Commands ---"
    COMMAND ${CMAKE_COMMAND} -E echo "    To configure for a build in the 'out' directory (recommended):"
    COMMAND ${CMAKE_COMMAND} -E echo "      cmake -S . -B out"
    COMMAND ${CMAKE_COMMAND} -E echo "    To configure for a specific build type (e.g., debug):"
    COMMAND ${CMAKE_COMMAND} -E echo "      cmake -S . -B out -DCMAKE_BUILD_TYPE\=debug"
    COMMAND ${CMAKE_COMMAND} -E echo "    To build \[after configuring\ ]: cmake --build out"
    COMMAND ${CMAKE_COMMAND} -E echo "    To build a specific configuration \[for multi-config generators\ ]: cmake --build out --config \[Debug\ or Release\]"
    COMMAND ${CMAKE_COMMAND} -E echo "    To clean build artifacts: cmake --build out --target clean"
    COMMAND ${CMAKE_COMMAND} -E echo "  "
    COMMAND ${CMAKE_COMMAND} -E echo "  --- Custom Targets \[run with 'cmake --build out --target <target_name>'\] ---"
    COMMAND ${CMAKE_COMMAND} -E echo "    interfaces: Generate interface files (also runs automatically when 'pe' builds)."
    COMMAND ${CMAKE_COMMAND} -E echo "    clean_interfaces: Remove interface files."
    COMMAND ${CMAKE_COMMAND} -E echo "    docs: Generate documentation with Doxygen."
    COMMAND ${CMAKE_COMMAND} -E echo "    copyright: Add copyright to source files."
    COMMAND ${CMAKE_COMMAND} -E echo "    clean_copyright: Remove copyright from source files."
    COMMAND ${CMAKE_COMMAND} -E echo "    run: Build in debug and run the executable."
    COMMAND ${CMAKE_COMMAND} -E echo "    release_run: Build in release and run the executable."
    COMMAND ${CMAKE_COMMAND} -E echo "    gdb: Build in debug, launch GDB, and run the program."
    COMMENT "Prints help_me message for CMake commands and custom targets."
)