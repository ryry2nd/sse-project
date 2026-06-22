FROM ubuntu:24.04

RUN apt update

#normal
RUN apt install -y \
	cmake \
	ninja-build \
	python3 \
	python3-venv \
	clang-20 \
	llvm-20 \
	ccache \
	build-essential \
	nasm \
	git

# sdl stuff
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
	libzstd-dev \
	zlib1g-dev \
	libwayland-dev \
	wayland-protocols \
	libxkbcommon-dev \
	libegl1-mesa-dev \
	libgles2-mesa-dev \
	libdecor-0-dev \
	pkg-config

#Windows stuff
RUN apt install -y \
	mingw-w64 \
	libz-mingw-w64-dev \
	gcc-mingw-w64-x86-64 \
	g++-mingw-w64-x86-64

RUN apt install -y libclang-rt-20-dev-wasm32
RUN apt install -y gcc-mingw-w64

RUN rm -rf /var/lib/apt/lists/*

# default build entry (you can override from build.sh)
ENTRYPOINT ["bash", "./docker/build.sh"]