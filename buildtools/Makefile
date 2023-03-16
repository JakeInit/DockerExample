PROJECT_NAME?=project

ARCH=$(shell dpkg --print-architecture) # Use for Linux
#ARCH=$(wmic OS get OSArchitecture) # Use for Windows

DOCKER_CC?=gcc
DOCKER_CXX?=g++

# Image is identified as <hub-user>/<repo-name>[:<tag>]

DOCKER_HUB_USER?=
DOCKER_REPO?=${PROJECT_NAME}
DOCKER_VERSION?=latest

DOCKER_TAG_NAME?=${DOCKER_HUB_USER}/${DOCKER_REPO}:${DOCKER_VERSION}

DOCKER_CONTAINER_NAME?=${PROJECT_NAME}_container
DOCKER_VOLUME_NAME?=

DOCKER_FILE?=DockerfileDevEnv

DOCKER_SYS_ARCH?=linux/${ARCH}
CONAN_PROFILE?=default

DOCKER_IMAGE_BUILD_FLAGS?=--no-cache=true

DOCKER_PREPEND_MAKEFILES?=
DOCKER_APPEND_MAKEFILES?=

DOCKER_CMAKE_FLAGS?=

DOCKER_SHELL?=bash
DOCKER_SOURCE_PATH?=/${PROJECT_NAME}
HOST_MOUNT_PATH?=${CURDIR}
DOCKER_BUILD_DIR?=build
DOCKER_CTEST_TIMEOUT?=5000

CONTAINER_VOLUME_PATH?=

DOCKER_TEST_CORE_DIR?=${DOCKER_BUILD_DIR}/cores

DOCKER_ADDITIONAL_RUN_PARAMS?=

DOCKER_BASIC_RUN_PARAMS?=-it --init --rm \
						--memory-swap=-1 \
						--ulimit core=-1 \
						--name="${DOCKER_CONTAINER_NAME}" \
						--platform=${DOCKER_SYS_ARCH} \
						--workdir=${DOCKER_SOURCE_PATH} \
						--mount type=bind,source=${HOST_MOUNT_PATH},target=${DOCKER_SOURCE_PATH} \
						${DOCKER_ADDITIONAL_RUN_PARAMS} \
						${DOCKER_TAG_NAME}

# Macro to determine if container is already running
IF_CONTAINER_RUNS=$(shell docker container inspect -f '{{.State.Running}}' ${DOCKER_CONTAINER_NAME} 2>/dev/null)

# Default make command
.DEFAULT_GOAL:=build

-include ${DOCKER_PREPEND_MAKEFILES}

.PHONY: help
help: ##
	@cat $(MAKEFILE_LIST) | grep -E '^[a-zA-Z_-]+:.*?## .*$$' | \
		awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

.PHONY: print
print: ## Print info about user defined parameters for docker
	@echo
	@echo "Host mount path = ${HOST_MOUNT_PATH}"
	@echo "Target mount path = ${DOCKER_SOURCE_PATH}"
	@echo "Image tag name = ${DOCKER_TAG_NAME}"
	@echo "Current architecture = ${ARCH}"
	@echo "User defined architecture = ${DOCKER_SYS_ARCH}"
	@echo "Container name = ${DOCKER_CONTAINER_NAME}"
	@echo "Dockerfile to build environment = ${DOCKER_FILE}"
	@echo

.PHONY: gen_cmake
gen_cmake: ## Generate cmake files in docker container mounted in host build path
	docker run ${DOCKER_BASIC_RUN_PARAMS} \
		${DOCKER_SHELL} -c \
		"mkdir -p ${DOCKER_SOURCE_PATH}/${DOCKER_BUILD_DIR} && \
		cd ${DOCKER_BUILD_DIR} && \
		CC=${DOCKER_CC} CXX=${DOCKER_CXX} \
		cmake ${DOCKER_CMAKE_FLAGS} .." \
		cd .. && chmod -R 777 ${DOCKER_BUILD_DIR}
	@echo
	@echo "CMake finished."

.PHONY: build
build: gen_cmake ## Build source in docker container mounted in host build path. In order to build a specific target run: make TARGET=<target name>.
	docker run ${DOCKER_BASIC_RUN_PARAMS} \
		${DOCKER_SHELL} -c \
		"cd ${DOCKER_BUILD_DIR} && \
	make -j$(($(nproc) - 1)) ${TARGET}"
	chmod -R 777 ${DOCKER_BUILD_DIR}
	@echo
	@echo "Build finished. The binaries are in ${HOST_MOUNT_PATH}/${DOCKER_BUILD_DIR}"

.PHONY: test
test: ## Run all tests
	docker run ${DOCKER_BASIC_RUN_PARAMS} \
		${DOCKER_SHELL} -c \
		"mkdir -p ${DOCKER_TEST_CORE_DIR} && \
	cd ${DOCKER_BUILD_DIR} && \
	ctest --timeout ${DOCKER_CTEST_TIMEOUT} --output-on-failure"

.PHONY: clean
clean: ## Clean build directory
	docker run ${DOCKER_BASIC_RUN_PARAMS} \
		${DOCKER_SHELL} -c \
		"rm -rf ${DOCKER_BUILD_DIR}/*"

.PHONY: login
login: ## Login to the container. Note: if the container is already running, login into existing one
	@if [ "${IF_CONTAINER_RUNS}" != "true" ]; then \
		echo "Opening new container with name ${DOCKER_CONTAINER_NAME}" && \
		if [ "${CONTAINER_VOLUME_PATH}" != "" ]; then \
			echo "Will open with volume ${DOCKER_VOLUME_NAME}" && \
			docker volume create ${DOCKER_VOLUME_NAME} && \
			docker run -v ${DOCKER_VOLUME_NAME}:/root/${CONTAINER_VOLUME_PATH} ${DOCKER_BASIC_RUN_PARAMS} \
			${DOCKER_SHELL}; \
		else \
			docker run ${DOCKER_BASIC_RUN_PARAMS} \
			${DOCKER_SHELL}; \
		fi \
	else \
		docker exec -it ${DOCKER_DEPS_CONTAINER} \
			${DOCKER_SHELL}; \
	fi

.PHONY: open_w_zerotier
open_w_zerotier: ## Opens a container with zerotier capabilities or joins one already running
	@if [ "${IF_CONTAINER_RUNS}" != "true" ]; then \
		echo "Opening new container" && \
		if [ "${CONTAINER_VOLUME_PATH}" != "" ]; then \
			echo ""Will open with volume ${DOCKER_VOLUME_NAME}"" && \
			docker volume create ${DOCKER_VOLUME_NAME} && \
			docker run -v ${DOCKER_VOLUME_NAME}:/root/${CONTAINER_VOLUME_PATH} ${DOCKER_BASIC_RUN_PARAMS} \
			--cap-add NET_ADMIN --device /dev/net/tun \
			${DOCKER_SHELL}; \
		else \
			docker run ${DOCKER_BASIC_RUN_PARAMS} \
			--cap-add NET_ADMIN --device /dev/net/tun \
			${DOCKER_SHELL}; \
		fi \
	else \
		echo "Entering existing container" && \
		docker exec -it ${DOCKER_CONTAINER_NAME} \
			${DOCKER_SHELL}; \
	fi

.PHONY: build-host-arch-image
build-host-arch-image: ## Build image to match host architecture.
	docker build ${DOCKER_IMAGE_BUILD_FLAGS} -t ${DOCKER_TAG_NAME} \
		-f ./${DOCKER_FILE} .
	@echo
	@echo "Build finished. Docker image name: \"${DOCKER_TAG_NAME}\"."
	@if [ "${DOCKER_VERSION}" == "" ]; then \
		echo "To push to docker hub, Must specify ${CURDIR}/Makefile: DOCKER_VERSION"; \
	fi

-include ${DOCKER_APPEND_MAKEFILES}

.PHONY: buildx-images
buildx-images: ## Build images for amd64 and arm64.
	@if [ "${DOCKER_VERSION}" == "" ]; then \
		echo "To push to docker hub, Must specify ${CURDIR}/Makefile: DOCKER_VERSION"; \
		exit 0; \
	fi
	docker run --privileged --rm tonistiigi/binfmt --install all
	docker buildx create --use
	docker buildx build --platform=linux/amd64,linux/arm64 ${DOCKER_IMAGE_BUILD_FLAGS} -t ${DOCKER_TAG_NAME} \
		-f ./${DOCKER_FILE} --push .
	@echo
	@echo "Build finished. Docker image name: \"${DOCKER_TAG_NAME}\"."

-include ${DOCKER_APPEND_MAKEFILES}

.PHONY: build-user-defined-arch-image
build-user-defined-arch-image: ## Build image based on architecture defined by the user.
	@if [ "${DOCKER_VERSION}" == "" ]; then \
		echo "To push to docker hub, Must specify ${CURDIR}/Makefile: DOCKER_VERSION"; \
		exit 0; \
	fi
	docker run --privileged --rm tonistiigi/binfmt --install all
	docker buildx create --use
	docker buildx build --platform=${DOCKER_SYS_ARCH} ${DOCKER_IMAGE_BUILD_FLAGS} -t ${DOCKER_TAG_NAME} \
		-f ./${DOCKER_FILE} --push .
	@echo
	@echo "Build finished. Docker image name: \"${DOCKER_TAG_NAME}\"."

-include ${DOCKER_APPEND_MAKEFILES}

.PHONY: push-image-to-hub
push-image-to-hub: ## Push the image to the docker hub
	docker login
	@if [ "${DOCKER_HUB_USER}" != "" ]; then \
		docker push ${DOCKER_TAG_NAME}; \
	else \
	  echo "Cannot push. Must specify ${CURDIR}/Makefile: DOCKER_HUB_USER"; \
	fi
