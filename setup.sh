#!/bin/bash

sudo apt update

sudo apt install -y cmake \
    ninja-build \
    python3 \
    python3-venv \
    clang-22 \
    llvm-22 \
    ccache \
    xorg-dev \
    build-essential \
    libzstd-dev \
	nasm