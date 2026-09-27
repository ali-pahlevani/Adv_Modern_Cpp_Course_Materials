#!/usr/bin/env bash
# Unit 1, slide 23. Runs the four compilation stages one at a time.
#   bash compile_stages.sh
set -euo pipefail
cd "$(dirname "$0")"
CXX="${CXX:-$(command -v clang++ || command -v g++)}"
mkdir -p out

echo "Using: $CXX"
"$CXX" -std=c++20 -E hello.cpp -o out/hello.i    ; echo "1. preprocess -> out/hello.i  ($(wc -l < out/hello.i) lines, from a 10-line file)"
"$CXX" -std=c++20 -S out/hello.i -o out/hello.s  ; echo "2. compile    -> out/hello.s  (assembly)"
"$CXX" -std=c++20 -c out/hello.s -o out/hello.o  ; echo "3. assemble   -> out/hello.o  (object file, with holes)"
"$CXX"            out/hello.o    -o out/hello    ; echo "4. link       -> out/hello    (executable)"

echo
echo "Undefined symbols in hello.o (marked U); the linker has to resolve these:"
nm -C out/hello.o | grep ' U ' | head -5 || true
echo
ls -lh out/hello.i out/hello.s out/hello.o out/hello
echo
./out/hello
