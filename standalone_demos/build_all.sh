#!/usr/bin/env bash
# Builds (and optionally runs) every standalone demo from Units 1 and 2.
#
#   bash build_all.sh              # build everything with clang++ (or g++ if absent)
#   bash build_all.sh --run        # build, then run every single-file demo
#   CXX=g++ bash build_all.sh      # pick a compiler
#   STD=c++23 bash build_all.sh    # try the C++23 branches (Jazzy's GCC 13)
#
# These demos are not ROS packages. The COLCON_IGNORE file here tells colcon to skip them.
set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-$(command -v clang++ || command -v g++)}"
STD="${STD:-c++20}"
FLAGS=(-std="$STD" -Wall -Wextra -Wpedantic -Werror)
RUN=false
[[ "${1:-}" == "--run" ]] && RUN=true

mkdir -p bin
echo "Compiler: $("$CXX" --version | head -1)   Standard: $STD"
echo

fail=0
for src in unit1/hello.cpp unit2/*.cpp; do
  name="$(basename "${src%.cpp}")"
  if "$CXX" "${FLAGS[@]}" "$src" -o "bin/$name" 2> "bin/$name.log"; then
    printf '  %-22s OK\n' "$name"
    rm -f "bin/$name.log"
  else
    printf '  %-22s FAILED (see bin/%s.log)\n' "$name" "$name"
    fail=1
  fi
done

echo
echo "Unit 1 multi-file demos:"
CXX="$CXX" bash unit1/diffbot_manual/build.sh > bin/diffbot_manual.log 2>&1 \
  && echo "  diffbot_manual         OK" || { echo "  diffbot_manual         FAILED"; fail=1; }

if command -v cmake > /dev/null; then
  cmake -S unit1/diffbot_cmake -B unit1/diffbot_cmake/build \
        -DCMAKE_CXX_COMPILER="$CXX" > bin/diffbot_cmake.log 2>&1 \
    && cmake --build unit1/diffbot_cmake/build >> bin/diffbot_cmake.log 2>&1 \
    && echo "  diffbot_cmake          OK" || { echo "  diffbot_cmake          FAILED"; fail=1; }
fi

if $RUN; then
  for exe in bin/*; do
    [[ -x "$exe" && ! -d "$exe" ]] || continue
    echo; echo "================ $(basename "$exe") ================"
    "./$exe"
  done
fi

exit $fail
