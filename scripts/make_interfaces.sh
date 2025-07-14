#/bin/bash

# This script will generate an interface file for this project
# It will scan ./src/** and for every directory that contains a .h or .hpp file
# with same name as the directory, it will generate an interface file
# in ./src/interfaces as {DIRNAME}_interface.h or {DIRNAME}_interface.hpp


# Get the current directory
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Get the parent directory
PARENT_DIR="$(dirname "$DIR")"

# Get the project name
PROJECT_NAME="$(basename "$PARENT_DIR")"

# Get the list of directories in src
DIRS=$(find "$PARENT_DIR/src" -maxdepth 1 -type d)

# Loop through the directories
for DIR in $DIRS; do
    # Get the directory name
    DIR_NAME=$(basename "$DIR")

    if [ -f "$DIR/$DIR_NAME.h" ]; then
        # Inside ./src/interfaces create a file with the same name as the directory + _interface.h
        echo "Generating interface file for $DIR_NAME"
        echo "#pragma once" > "$DIR/$DIR_NAME"_interface.h
        echo "#ifndef $DIR_NAME"_INTERFACE_H >> "$DIR/$DIR_NAME"_interface.h
        echo "#define $DIR_NAME"_INTERFACE_H >> "$DIR/$DIR_NAME"_interface.h
        echo "" >> "$DIR/$DIR_NAME"_interface.h

        # Copy the content of the .h file to the interface file (after 5th line)
        tail -n +5 "$DIR/$DIR_NAME".h >> "$DIR/$DIR_NAME"_interface.h
        echo "" >> "$DIR/$DIR_NAME"_interface.h

        # Check if file contains "#endif" at the end of file
        if ! tail -n 10 "$DIR/$DIR_NAME".h | grep -F "#endif"; then
            echo "#endif // $DIR_NAME"_INTERFACE_H >> "$DIR/$DIR_NAME"_interface.h
            echo "" >> "$DIR/$DIR_NAME"_interface.h
        fi
        
        mv "$DIR/$DIR_NAME"_interface.h "./interfaces/$DIR_NAME"_interface.h
    fi

    if [ -f "$DIR/$DIR_NAME.hpp" ]; then
        # Inside ./src/interfaces create a file with the same name as the directory + _interface.hpp
        echo "Generating interface file for $DIR_NAME"
        echo "#pragma once" > "$DIR/$DIR_NAME"_interface.hpp
        echo "#ifndef $DIR_NAME"_INTERFACE_HPP >> "$DIR/$DIR_NAME"_interface.hpp
        echo "#define $DIR_NAME"_INTERFACE_HPP >> "$DIR/$DIR_NAME"_interface.hpp
        echo "" >> "$DIR/$DIR_NAME"_interface.hpp
        
        # Copy the content of the .hpp file to the interface file (after 5th line)
        tail -n +5 "$DIR/$DIR_NAME".hpp >> "$DIR/$DIR_NAME"_interface.hpp
        echo "" >> "$DIR/$DIR_NAME"_interface.hpp

        # Check if file contains "#endif" at the end of file
        if ! tail -n 10 "$DIR/$DIR_NAME".hpp | grep -F "#endif"; then
            echo "#endif // $DIR_NAME"_INTERFACE_HPP >> "$DIR/$DIR_NAME"_interface.hpp
            echo "" >> "$DIR/$DIR_NAME"_interface.hpp
        fi

        
        mv "$DIR/$DIR_NAME"_interface.hpp "./interfaces/$DIR_NAME"_interface.hpp
    fi
done