#!/bin/bash

for f in ./dots/*; do
  if [[ ${f:(-4):4} == ".dot" ]]; then
    dot -Tsvg "$f" -o "./svgs/${f:7:-4}.svg"
    if [[ $? == 0 ]]; then
      echo "success: converted $f to ./svgs/${f:7:-4}.svg"
    fi
  fi
done
