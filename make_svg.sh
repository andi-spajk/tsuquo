#!/bin/bash

for f in *; do
  if [[ ${f:(-4):4} == ".dot" ]]; then
    dot -Tsvg "$f" -o "${f:0:-4}.svg"
    echo "success: converted $f to ${f:0:-4}.svg"
  fi
done
