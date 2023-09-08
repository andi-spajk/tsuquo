#!/bin/bash

for f in *; do
  if [[ ${f:(-4):4} == ".dot" ]]; then
    dot -Tsvg "$f" -o "${f:0:-4}.svg"
    if [[ $? == 0 ]]; then
      echo "success: converted $f to ${f:0:-4}.svg"
    fi
  fi
done
