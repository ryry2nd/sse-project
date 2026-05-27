#!/bin/bash

sudo apt update

sudo apt install cmake \
    ninja-build \
    python3 \
    python3-venv \
    clang-20 \
    llvm-20
    ccache \
    xorg-dev \
    build-essential \
    libzstd-dev