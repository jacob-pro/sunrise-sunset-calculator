#!/usr/bin/env bash

if cat /proc/version | grep microsoft; then
  SUFFIX=.exe
else
  SUFFIX=
fi

if ! command -v bindgen$SUFFIX &>/dev/null; then
  echo "Installing bindgen..."
  cargo$SUFFIX install bindgen
fi

bindgen$SUFFIX --with-derive-default --ctypes-prefix=cty "sunrise-sunset-calculator/include/ssc.h" > "src/binding.rs"
