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
        if ! grep -qF "$LICENSE_CHECK_STRING" "$f"; then
        echo "Adding copyright to $f"
        echo -e "\n\n/*" >> "$f"
        cat "$LICENSE_FILE" >> "$f"
        echo -e "\n*/" >> "$f"
    fi
fi
done
