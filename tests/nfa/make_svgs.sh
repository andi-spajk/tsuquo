#!/bin/bash

set -xe

dot -Tsvg a.dot -o a.svg
dot -Tsvg b.dot -o b.svg
dot -Tsvg a_or_b.dot -o a_or_b.svg
dot -Tsvg a_or_b_closure.dot -o a_or_b_closure.svg
