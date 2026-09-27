#!/usr/bin/env bash
# Unit 1, Demo 1.1 (slides 43 to 47). Builds a library using only the compiler.
#   bash build.sh
set -euo pipefail
cd "$(dirname "$0")"
CXX="${CXX:-$(command -v clang++ || command -v g++)}"
rm -rf out && mkdir -p out
echo "Using: $CXX"

echo; echo "== Slide 43: compile main.cpp on its own; the linker can't find the definitions =="
if "$CXX" -std=c++20 -I include src/main.cpp -o out/demo 2> out/link_error.txt; then
  echo "unexpected: it linked"
else
  grep -m1 'undefined reference' out/link_error.txt || head -3 out/link_error.txt
  echo "   ^ 'undefined reference' is a linker problem, not a syntax problem"
fi

echo; echo "== Slide 44: compile the module =="
"$CXX" -std=c++20 -I include -c src/kinematics.cpp -o out/kinematics.o
nm -C out/kinematics.o | grep to_wheel_speeds      # 'T' = defined here

echo; echo "== Slide 45: archive it into a static library =="
ar rcs out/libdiffbot_kinematics.a out/kinematics.o
ar t out/libdiffbot_kinematics.a

echo; echo "== Slide 46: link against it (-l flags go after the files that use them) =="
"$CXX" -std=c++20 -I include src/main.cpp -L out -ldiffbot_kinematics -o out/diffbot_demo
./out/diffbot_demo

echo; echo "== Slide 47: the same library, shared =="
"$CXX" -std=c++20 -I include -fPIC -c src/kinematics.cpp -o out/kinematics_pic.o
"$CXX" -shared out/kinematics_pic.o -o out/libdiffbot_kinematics_shared.so
"$CXX" -std=c++20 -I include src/main.cpp -L out -ldiffbot_kinematics_shared -o out/diffbot_demo_shared
if ./out/diffbot_demo_shared 2>/dev/null; then
  echo "(it ran without LD_LIBRARY_PATH; the loader already knew where to look)"
else
  echo "without LD_LIBRARY_PATH: 'error while loading shared libraries' (a runtime error)"
fi
echo -n "with    LD_LIBRARY_PATH: "
LD_LIBRARY_PATH=out ./out/diffbot_demo_shared
