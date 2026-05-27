#!/bin/bash

OS="linux"

while [[ $# -gt 0 ]]; do
  case $1 in
	--os)
	  OS="$2"
	  shift 2
	  ;;
	*)
	  echo "Unknown option $1"
	  exit 1
	  ;;
  esac
done

if [ "$OS" = "linux" ]; then
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
elif [ "$OS" = "windows" ]; then
	# this doesn't even work yet. I hate windows
	pacman -S --noconfirm cmake clang llvm ccache ninja nasm python3 zlib
fi