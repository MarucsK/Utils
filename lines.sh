#!/bin/bash

INCLUDE_DIR="./include"

TOTAL_LINES=0

while IFS= read -r -d $'\0' file; do
    LINE_COUNT=$(wc -l < "$file")
    TOTAL_LINES=$((TOTAL_LINES + LINE_COUNT))

    echo "  $LINE_COUNT lines: $file"
done < <(find "$INCLUDE_DIR" -name "*.hpp" -print0)

echo "----------------------------------------"
echo "total：$TOTAL_LINES lines"

exit 0