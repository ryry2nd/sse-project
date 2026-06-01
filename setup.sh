#!/bin/bash
sudo apt update

sudo apt install -y cmake \
	ninja-build \
	python3 \
	python3-venv \
	clang-21 \
	llvm-21 \
	ccache \
	xorg-dev \
	build-essential \
	nasm \
	mingw-w64 \
	libzstd-dev \
	libz-mingw-w64-dev \
	wine