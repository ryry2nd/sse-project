FROM ubuntu:24.04

RUN apt update

#normal
RUN apt install -y \
	cmake \
	ninja-build \
	python3 \
	python3-venv \
	clang-18 \
	llvm-18 \
	ccache \
	build-essential \
	nasm \
	git

# sdl-stuff
RUN apt install -y \
	libx11-dev \
	libxext-dev \
	libxrandr-dev \
	libxinerama-dev \
	libxcursor-dev \
	libxi-dev \
	libxfixes-dev \
	libxss-dev \
	libgl1-mesa-dev \
	libxtst-dev \
	zlib1g-dev \
	libwayland-dev \
	wayland-protocols \
	libxkbcommon-dev

RUN rm -rf /var/lib/apt/lists/*

# default build entry (you can override from build.sh)
ENTRYPOINT ["bash", "./docker/build.sh"]