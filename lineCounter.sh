#!/bin/bash

dir="src/engine"

engine_cpp_lines=$(find "$dir" -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" \) -print0 \
  | xargs -0 cat 2>/dev/null | wc -l)

engine_hdr_lines=$(find "$dir" -type f \( -name "*.h" -o -name "*.hpp" \) -print0 \
  | xargs -0 cat 2>/dev/null | wc -l)

engine_cpp_count=$(find "$dir" -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" \) | wc -l)
engine_hdr_count=$(find "$dir" -type f \( -name "*.h" -o -name "*.hpp" \) | wc -l)

echo "Engine: C++ files:    $engine_cpp_count"
echo "Engine: Header files: $engine_hdr_count"
echo "Engine: C++ lines:    $engine_cpp_lines"
echo "Engine: Header lines: $engine_hdr_lines"
echo "Engine: Total lines:  $((engine_cpp_lines + engine_hdr_lines))"

echo ""


dir="src/modules"

modules_cpp_lines=$(find "$dir" -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" \) -print0 \
  | xargs -0 cat 2>/dev/null | wc -l)

modules_hdr_lines=$(find "$dir" -type f \( -name "*.h" -o -name "*.hpp" \) -print0 \
  | xargs -0 cat 2>/dev/null | wc -l)

modules_cpp_count=$(find "$dir" -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" \) | wc -l)
modules_hdr_count=$(find "$dir" -type f \( -name "*.h" -o -name "*.hpp" \) | wc -l)

echo "Modules: C++ files:    $modules_cpp_count"
echo "Modules: Header files: $modules_hdr_count"
echo "Modules: C++ lines:    $modules_cpp_lines"
echo "Modules: Header lines: $modules_hdr_lines"
echo "Modules: Total lines:  $((modules_cpp_lines + modules_hdr_lines))"

echo ""

echo "C++ files:    $((engine_cpp_count + modules_cpp_count))"
echo "Header files: $((engine_hdr_count + modules_hdr_count))"
echo "C++ lines:    $((engine_cpp_lines + modules_cpp_lines))"
echo "Header lines: $((engine_hdr_lines + modules_hdr_lines))"
echo "Total lines:  $((engine_cpp_lines + modules_cpp_lines + engine_hdr_lines + modules_hdr_lines))"