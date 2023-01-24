PROJECT_NAME=example
DOCKER_DEPS_REPO=${PROJECT_NAME}
DOCKER_CC=gcc
DOCKER_CXX=g++
DOCKER_DEPS_FILE=DockerfileBuildEnv
DOCKER_DEPS_IMAGE_BUILD_FLAGS=--no-cache=true
DOCKER_BUILD_DIR=build
DOCKER_SHELL=bash
DOCKER_DEPS_VERSION=0.00.01
DOCKER_HUB_USER=jakeinit
DOCKER_SYS_ARCH=linux/amd64,linux/arm64

include buildtools/Makefile

