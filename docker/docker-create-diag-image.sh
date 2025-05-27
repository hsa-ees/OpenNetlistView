#!/bin/bash

ARGS=""

if [[ "$1" == "-c" ]]; then
    ARGS="--no-cache"
fi


docker build . $ARGS --build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) -t diag_image
