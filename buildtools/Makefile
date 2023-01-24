PROJECT_NAME?=project

DOCKER_CC?=gcc
DOCKER_CXX?=g++

#Name local image by <hub-user>/<repo-name>[:<tag>]

DOCKER_HUB_USER?=
DOCKER_DEPS_REPO?=${PROJECT_NAME}
DOCKER_DEPS_VERSION?=latest

DOCKER_TAG_NAME?=${DOCKER_HUB_USER}/${DOCKER_DEPS_REPO}:${DOCKER_DEPS_VERSION}

DOCKER_DEPS_IMAGE?=${PROJECT_NAME}_build
DOCKER_DEPS_CONTAINER?=${DOCKER_DEPS_IMAGE}

DOCKER_DEPS_FILE?=DockerfileBuildEnv

DOCKER_SYS_ARCH?=linux/amd64

DOCKER_DEPS_IMAGE_BUILD_FLAGS?=--no-cache=true

DOCKER_PREPEND_MAKEFILES?=
DOCKER_APPEND_MAKEFILES?=

DOCKER_CMAKE_FLAGS?=

DOCKER_SHELL?=bash
LOCAL_SOURCE_PATH?=${CURDIR}
DOCKER_SOURCE_PATH?=/${PROJECT_NAME}
DOCKER_BUILD_DIR?=build
DOCKER_CTEST_TIMEOUT?=5000

DOCKER_TEST_CORE_DIR?=${DOCKER_BUILD_DIR}/cores

DOCKER_ADDITIONAL_RUN_PARAMS?=

DOCKER_BASIC_RUN_PARAMS?=-it --init --rm \
					  --memory-swap=-1 \
					  --ulimit core=-1 \
					  --name="${DOCKER_DEPS_CONTAINER}" \
					  --workdir=${DOCKER_SOURCE_PATH} \
					  --mount type=bind,source=${LOCAL_SOURCE_PATH},target=${DOCKER_SOURCE_PATH} \
					  ${DOCKER_ADDITIONAL_RUN_PARAMS} \
					  ${DOCKER_TAG_NAME}

IF_CONTAINER_RUNS=$(shell docker container inspect -f '{{.State.Running}}' ${DOCKER_DEPS_CONTAINER} 2>/dev/null)

.DEFAULT_GOAL:=build

-include ${DOCKER_PREPEND_MAKEFILES}

.PHONY: help
help: ##
	@cat $(MAKEFILE_LIST) | grep -E '^[a-zA-Z_-]+:.*?## .*$$' | \
		awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

.PHONY: gen_cmake
gen_cmake: ## Generate cmake files, used internally
	docker run ${DOCKER_BASIC_RUN_PARAMS} \
		${DOCKER_SHELL} -c \
		"mkdir -p ${DOCKER_SOURCE_PATH}/${DOCKER_BUILD_DIR} && \
		cd ${DOCKER_BUILD_DIR} && \
		CC=${DOCKER_CC} CXX=${DOCKER_CXX} \
		cmake ${DOCKER_CMAKE_FLAGS} .."
	@echo
	@echo "CMake finished."

.PHONY: build
build: gen_cmake ## Build source. In order to build a specific target run: make TARGET=<target name>.
	docker run ${DOCKER_BASIC_RUN_PARAMS} \
		${DOCKER_SHELL} -c \
		"cd ${DOCKER_BUILD_DIR} && \
		make -j$(($(nproc) - 1)) ${TARGET}"
	@echo
	@echo "Build finished. The binaries are in ${CURDIR}/${DOCKER_BUILD_DIR}"

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
		"rm -rf ${DOCKER_BUILD_DIR}"

.PHONY: login
login: ## Login to the container. Note: if the container is already running, login into existing one
	@if [ "${IF_CONTAINER_RUNS}" != "true" ]; then \
		docker run ${DOCKER_BASIC_RUN_PARAMS} \
			${DOCKER_SHELL}; \
	else \
		docker exec -it ${DOCKER_DEPS_CONTAINER} \
			${DOCKER_SHELL}; \
	fi

.PHONY: build-docker-deps-image
build-docker-deps-image: ## Build the deps image.
	docker build ${DOCKER_DEPS_IMAGE_BUILD_FLAGS} -t ${DOCKER_TAG_NAME} \
		-f ./${DOCKER_DEPS_FILE} .
	@echo
	@echo "Build finished. Docker image name: \"${DOCKER_TAG_NAME}\"."
	@if [ "${DOCKER_DEPS_VERSION}" == "" ]; then \
		echo "To push to docker hub, Must specify ${CURDIR}/Makefile: DOCKER_DEPS_VERSION"; \
	fi

-include ${DOCKER_APPEND_MAKEFILES}

.PHONY: buildx-images
buildx-images: ## Build images for amd64 and arm64.
	@if [ "${DOCKER_DEPS_VERSION}" == "" ]; then \
		echo "To push to docker hub, Must specify ${CURDIR}/Makefile: DOCKER_DEPS_VERSION"; \
		exit 0; \
	fi
	docker buildx create --use
	docker buildx build --platform=${DOCKER_SYS_ARCH} ${DOCKER_DEPS_IMAGE_BUILD_FLAGS} -t ${DOCKER_TAG_NAME} \
		-f ./${DOCKER_DEPS_FILE} --push .
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
