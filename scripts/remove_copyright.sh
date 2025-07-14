#!/bin/bash

LICENSE_FILE="LICENSE.md"

if [ ! -f "$LICENSE_FILE" ]; then
    echo "Error: $LICENSE_FILE not found." >&2
    exit 1
fi

LICENSE_CHECK_STRING=$(head -n 1 "$LICENSE_FILE")

# Find all relevant source and header files
find . -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -print0 | while IFS= read -r -d '' f; do
    if [ -n "$f" ] && [ -f "$f" ]; then
        # Check if the file contains the copyright string
        if grep -qF "$LICENSE_CHECK_STRING" "$f"; then
            echo "Removing copyright from $f"
            # Use perl to remove the multi-line copyright block
            # It matches two newlines, then /*, then any content non-greedily, then */
            perl -i -0777 -pe 's/\n{2}\/\*.*?\*\///sg' "$f"
        fi
    fi
done
