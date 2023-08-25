#!/bin/bash

set -xe

dot -Tsvg a.dot -o a.svg
dot -Tsvg b.dot -o b.svg
dot -Tsvg a_or_b.dot -o a_or_b.svg
dot -Tsvg a_or_b_closure.dot -o a_or_b_closure.svg
dot -Tsvg cooper_torczon_example2.5.dot -o cooper_torczon_example2.5.svg
dot -Tsvg atplus.dot -o atplus.svg
dot -Tsvg maybe_and.dot -o maybe_and.svg
