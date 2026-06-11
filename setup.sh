#!/usr/bin/env bash

set -e


if ! command -v docker >/dev/null 2>&1; then
	sudo apt update
	sudo apt install docker.io
	sudo systemctl enable --now docker
	sudo usermod -aG docker $USER
fi

if ! docker info >/dev/null 2>&1; then
	echo "Docker not running. Try logging out and logging back in or rebooting"
	exit 1
fi

docker build -t sse-builder .