#!/usr/bin/env bash
# Builds (and optionally runs) every standalone demo, Units 1 to 8.
#
#   bash build_all.sh              # build everything with clang++ (or g++ if absent)
#   bash build_all.sh --run        # build, then run every demo that built
#   CXX=g++ bash build_all.sh      # pick a compiler
#   STD=c++23 bash build_all.sh    # try the C++23 parts (Jazzy's GCC 13)
#
# These demos are not ROS packages. The COLCON_IGNORE file here tells colcon to skip them.
#
# The demo files are exactly as on the slides, so line numbers in compiler errors and
# sanitizer reports match the slides too. That also means a few of them go wrong on purpose;
# the lists below say which, and the script counts those as passing.
set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-$(command -v clang++ || command -v g++)}"
STD="${STD:-c++20}"
FLAGS=(-std="$STD" -Wall -Wextra -Wpedantic -pthread)
RUN=false
[[ "${1:-}" == "--run" ]] && RUN=true

# The slide shows the compiler error
COMPILE_ERROR=(unit3/demo_32.cpp)
# The slide shows the warning, so these are built without -Werror
WARNS=(unit3/demo_36.cpp unit4/demo_45.cpp unit6/demo_68.cpp)
# The slide shows the crash or the undefined behaviour (only matters with --run)
MISBEHAVES=(unit3/demo_38.cpp unit3/exercise_37/occupancy_grid.cpp unit5/demo_54.cpp
            unit6/demo_68.cpp unit7/demo_72_shared.cpp)
# Needs C++23 std::expected: GCC 13 or newer with STD=c++23
NEEDS_EXPECTED=(unit8/demo_84_expected.cpp)

in_list() {
  local item="$1"; shift
  for x in "$@"; do [[ "$x" == "$item" ]] && return 0; done
  return 1
}

has_expected() {
  printf '#include <version>\n#ifndef __cpp_lib_expected\n#error no std::expected\n#endif\nint main() {}\n' \
    | "$CXX" -std="$STD" -x c++ - -o /dev/null 2> /dev/null
}

mkdir -p bin
echo "Compiler: $("$CXX" --version | head -1)   Standard: $STD"
echo

fail=0
for src in unit1/hello.cpp unit2/*.cpp unit[3-8]/*.cpp unit3/exercise_37/*.cpp; do
  name="$(echo "${src%.cpp}" | tr '/' '_')"
  if in_list "$src" "${NEEDS_EXPECTED[@]}" && ! has_expected; then
    printf '  %-48s skipped (needs C++23 std::expected)\n' "$src"
    continue
  fi
  extra=(-Werror)
  in_list "$src" "${WARNS[@]}" && extra=()
  if "$CXX" "${FLAGS[@]}" "${extra[@]}" "$src" -o "bin/$name" 2> "bin/$name.log"; then
    if in_list "$src" "${COMPILE_ERROR[@]}"; then
      printf '  %-48s FAILED (should not compile)\n' "$src"; fail=1
    elif in_list "$src" "${WARNS[@]}"; then
      printf '  %-48s OK (no -Werror: the slide shows its warning)\n' "$src"
    else
      printf '  %-48s OK\n' "$src"; rm -f "bin/$name.log"
    fi
  elif in_list "$src" "${COMPILE_ERROR[@]}"; then
    printf '  %-48s OK, does not compile (see bin/%s.log)\n' "$src" "$name"
  else
    printf '  %-48s FAILED (see bin/%s.log)\n' "$src" "$name"; fail=1
  fi
done

echo
echo "Multi-file demos:"
CXX="$CXX" bash unit1/diffbot_manual/build.sh > bin/diffbot_manual.log 2>&1 \
  && echo "  unit1/diffbot_manual                             OK" \
  || { echo "  unit1/diffbot_manual                             FAILED"; fail=1; }

if command -v cmake > /dev/null; then
  cmake -S unit1/diffbot_cmake -B unit1/diffbot_cmake/build \
        -DCMAKE_CXX_COMPILER="$CXX" > bin/diffbot_cmake.log 2>&1 \
    && cmake --build unit1/diffbot_cmake/build >> bin/diffbot_cmake.log 2>&1 \
    && echo "  unit1/diffbot_cmake                              OK" \
    || { echo "  unit1/diffbot_cmake                              FAILED"; fail=1; }
fi

# Demo 6.2: the template defined in a .cpp file must fail to link; the fixed version must build
if "$CXX" "${FLAGS[@]}" unit6/demo_62/main.cpp unit6/demo_62/printer.cpp \
     -o bin/unit6_demo_62 2> bin/unit6_demo_62.log; then
  echo "  unit6/demo_62                                    FAILED (should not link)"; fail=1
else
  echo "  unit6/demo_62                                    OK, does not link (see bin/unit6_demo_62.log)"
fi
"$CXX" "${FLAGS[@]}" -Werror unit6/demo_62_fixed/main.cpp unit6/demo_62_fixed/printer.cpp \
    -o bin/unit6_demo_62_fixed 2> bin/unit6_demo_62_fixed.log \
  && echo "  unit6/demo_62_fixed                              OK" \
  || { echo "  unit6/demo_62_fixed                              FAILED"; fail=1; }

if $RUN; then
  misbehaving=()
  for src in "${MISBEHAVES[@]}"; do misbehaving+=("$(echo "${src%.cpp}" | tr '/' '_')"); done
  for exe in bin/*; do
    [[ -f "$exe" && -x "$exe" ]] || continue
    name="$(basename "$exe")"
    echo; echo "================ $name ================"
    if in_list "$name" "${misbehaving[@]}"; then
      echo "(goes wrong on purpose, as on the slide)"
    fi
    "./$exe" < /dev/null || echo "[exit code $?]"
  done
fi

exit $fail
