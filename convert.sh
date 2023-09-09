#!/bin/bash

extension="svg"
if [[ $# == 1 ]]; then
  if [[ $1 != "svg"  && $1 != "png" && $1 != "jpg" && $1 != "jpeg" ]]; then
      echo "error: invalid cmdline args"
      echo "USAGE: $ ./convert.sh extension"
      echo
      echo "valid extensions are svg, png, and jpg/jpeg"
    exit
  else
    extension=$1
  fi
elif [[ $# != 0 ]]; then
  echo "error: invalid cmdline args"
  echo "USAGE: $ ./convert.sh extension"
  echo
  echo "valid extensions are svg, png, and jpg/jpeg"
  exit
fi
# no args means we automatically use svg

for f in ./dots/*; do
  if [[ ${f:(-4):4} == ".dot" ]]; then
    dot -T$extension "$f" -o "./saves/${f:7:-4}.$extension"
    if [[ $? == 0 ]]; then
      echo "success: converted $f to ./saves/${f:7:-4}.$extension"
    fi
  fi
done

