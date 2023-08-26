#!/bin/bash

set -xe

for f in ./dots/*; do
  dot -Tsvg "$f" -o "./svgs/${f:7:-4}.svg"
done
