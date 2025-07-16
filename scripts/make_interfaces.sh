#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# Get the current script's directory.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Get the parent directory, which we'll consider the project root.
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Define the source directory for modules.
SRC_DIR="$PROJECT_ROOT/src"

# Define the destination directory for interface files.
INTERFACES_DIR="$PROJECT_ROOT/interfaces"

# Create the interfaces directory if it doesn't exist.
# The -p flag prevents errors if the directory already exists.
mkdir -p "$INTERFACES_DIR"

# Find directories in src and loop through them safely.
# -print0 ensures names with spaces are handled correctly.
# The while loop reads null-terminated strings.
find "$SRC_DIR" -maxdepth 1 -type d -print0 | while IFS= read -r -d $'\0' current_module_dir; do
    # Skip the base 'src' directory itself if 'find' includes it.
    if [ "$current_module_dir" = "$SRC_DIR" ]; then
        continue
    fi

    # Get the base name of the current directory (e.g., "moduleA").
    MODULE_NAME=$(basename "$current_module_dir")

    # Define the full paths to the potential interface files.
    H_INTERFACE_FILE="${current_module_dir}/${MODULE_NAME}_interface.h"
    HPP_INTERFACE_FILE="${current_module_dir}/${MODULE_NAME}_interface.hpp"

    # Check if the .h interface file exists and copy it.
    if [ -f "$H_INTERFACE_FILE" ]; then
        echo "Copying $H_INTERFACE_FILE to $INTERFACES_DIR/"
        cp "$H_INTERFACE_FILE" "$INTERFACES_DIR/"
    fi

    # Check if the .hpp interface file exists and copy it.
    if [ -f "$HPP_INTERFACE_FILE" ]; then
        echo "Copying $HPP_INTERFACE_FILE to $INTERFACES_DIR/"
        cp "$HPP_INTERFACE_FILE" "$INTERFACES_DIR/"
    fi
done

echo "All relevant interface files have been copied to $INTERFACES_DIR."