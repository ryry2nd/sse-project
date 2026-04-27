#!/bin/bash

dir="src/"

cpp_lines=$(find "$dir" -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" \) -print0 \
  | xargs -0 cat 2>/dev/null | wc -l)

hdr_lines=$(find "$dir" -type f \( -name "*.h" -o -name "*.hpp" \) -print0 \
  | xargs -0 cat 2>/dev/null | wc -l)

cpp_count=$(find "$dir" -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" \) | wc -l)
hdr_count=$(find "$dir" -type f \( -name "*.h" -o -name "*.hpp" \) | wc -l)

echo "C++ files:    $cpp_count"
echo "Header files: $hdr_count"
echo "C++ lines:    $cpp_lines"
echo "Header lines: $hdr_lines"
echo "Total lines:  $((cpp_lines + hdr_lines))"