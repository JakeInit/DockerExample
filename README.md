# Docker Example Application #

* [What is this repository for?](#whatfor)
* [Terminology](#terminology)
* [How to get set up?](#setup)
* [Important Notes](#inotes)
* [Install and Register Nvidia](#nvidia)
* [Project Work Flow](#workflow)
  * [Build the Container and image on the Dev Machine](#buildcontainer)
  * [Developing the application on the dev machine and testing inside the image](#development)
  * [Exporting the image from the dev machine](#imageexport)
  * [Placing image onto target device](#imageimport)
* [Using make to implement docker commands](#makecommands)
* [Cleaning host machine of all images](#cleanup)
* [Who do I talk to?](#who)

### What is this repository for? <a name="whatfor"></a>

* To show how to use docker with an example project

### Terminology <a name="terminology"></a>
* Dev Machine - the device where the application development is being done (most likely a laptop)
* Target Machine - the device that the containerized image is meant to be run on.
* Hosted Machine - the device that holds the docker container. Can be the dev or target machine.

### How to get set up? <a name="setup"></a>

* For Ubuntu 18.04, install docker engine from [here](https://docs.docker.com/engine/install/ubuntu/)
* For additional help on installing docker for Ubuntu 18.04, see docs/DockerSetup.txt
* For other OS, install docker desktop for GUI interface. Download and setup found [here](https://docs.docker.com/get-started/)
* The example in this repo is based on this [link](https://ddanilov.me/dockerized-cpp-build)
* Create a docker hub account [here](https://hub.docker.com/). User does not have to. Directions also specify sharing image without docker hub.
* To be able to push to docker hub, delete the file config.json inside ~/.docker on the hosted machine (verified on Ubuntu 18.04)
* If Nvidia CUDA support is needed, follow directions in section [Install and Register Nvidia](#nvidia).
* After installing docker, need to install the following on dev machine for creating non-native system architectures type images.

```
docker run --privileged --rm tonistiigi/binfmt --install all
```

* If buildx commands ever stop working for arm64, re-run the above. It is likely compatible builders were removed by a prune command.
* Source for creating images for specific architecture systems found [here](https://developer.arm.com/documentation/102475/0100/Multi-architecture-images)
* Source for cross compiling (much faster) found [here](https://www.docker.com/blog/faster-multi-platform-builds-dockerfile-cross-compilation-guide/)

### Important Notes <a name="inotes"></a>

* Conan can be incorporated with docker. Uncertain if there are special instructions or can just add to RUN of docker build environment file.
* Have not verified if cross-compilation works.
* It is uncertain if CUDA support is needed, but the following sections shows how to set it up if so.
* When adding run commands to DockerFileBuildEnv, try to concatenate commands to reduce number of run commands. This reduces complexity of image by decreasing layers created.

### Install and Register Nvidia <a name="nvidia"></a>

* This section is only necessary if CUDA support for the image is needed.
* Do this before creating the image if CUDA support is necessary.
* The following steps are implemented on the host machine that is creating the image.
* It is not tested of verified if this needs done on the target device as well.
* Link to installing Nvidia drivers for different systems can be found [here](https://docs.nvidia.com/datacenter/tesla/tesla-installation-notes/index.html)
* To install Nvidia drivers in Ubuntu 18.04, run the following

```
sudo apt-get install linux-headers-$(uname -r)
distribution=$(. /etc/os-release;echo $ID$VERSION_ID | sed -e 's/\.//g')
wget https://developer.download.nvidia.com/compute/cuda/repos/$distribution/x86_64/cuda-keyring_1.0-1_all.deb
sudo dpkg -i cuda-keyring_1.0-1_all.deb
```

* See [post install actions](https://docs.nvidia.com/cuda/cuda-installation-guide-linux/index.html#post-installation-actions) for mandatory setup
* Registering Nvidia and options link can be found [here](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/user-guide.html)
* To register Nvidia with docker, run the following on the hosted machine.

```
sudo dockerd --add-runtime=nvidia=/usr/bin/nvidia-container-runtime [...]
```

* Specify docker cuda capabilities that are desired for the image when running on the hosted machine.
* See two separate examples below that can potentially be used:

```
docker run --rm --runtime=nvidia \
    -e NVIDIA_VISIBLE_DEVICES=2,3 \
    -e NVIDIA_DRIVER_CAPABILITIES=compute,utility \
    nvidia/cuda nvidia-smi
    
docker run --rm --gpus 'all,"capabilities=compute,utility"' \
    nvidia/cuda:11.0.3-base-ubuntu20.04 nvidia-smi
```

* User can avoid calling out capabilities and options when running the docker image.
* In DockerFileBuildEnv, add the following to set up cuda capabilities without needing to specify with command line at runtime.

```
ENV NVIDIA_REQUIRE_CUDA "cuda>=11.0 driver>=450" # restricts cuda version and driver
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES compute,utility
```

### Project Work Flow <a name="workflow"></a>
* For specific steps for using emulated arm64 container, see docs/HelloWorld_ARM64_build.txt
#### Build the Container and image on the Dev Machine <a name="buildcontainer"></a>

* Be certain to have the file "DockerFileBuildEnv" inside the main repo
* This file is what is used to build the container and image.
* The following commands build images based on the current host machine architecture
```
sudo docker build -t username/example:0.00.01 -f DockerfileBuildEnv .
```

* Note that the docker tag name needs to be in the format <hub-user>/<repo-name>[:<tag>] to push to docker hub
* If not using docker hub, then the command to build the container can be run as

```
sudo docker build -t example/example_build:0.00.01 -f DockerfileBuildEnv .
```

* To build an image for an arm device, run

```
sudo docker build --platform=linux/arm64 -t example/example_build:0.00.01 -f DockerfileBuildEnv .
```

#### Developing the application on the dev machine and testing inside the image <a name="development"></a>

* Create a build folder in the main repo on the dev machine.

```
mkdir build
```

* For list of available options when running, go [here](https://docs.docker.com/engine/reference/commandline/run/)
* Run the following on the dev machine inside the main repo

```
sudo docker run -it --rm --name=example \
    --mount type=bind,source=${PWD},target=/repoName \
	example/example_build:0.00.01 \
	bash

```

* This opens a bash environment inside the image.
* The user can now make changes to the source code on the dev machine in real time and
  changes will occur live inside the image. Testing can be done inside the container.
* Build the code on the dev machine as normal inside the build directory

```
cmake ..
make
```

* Inside the container, be sure to be in the repo folder that was mounted
* Test inside the container in bash as normal

```
./build/applicationName
```

#### Exporting the image from the dev machine <a name="imageexport"></a>
* If pushing to docker hub, login first with username and password

```
sudo docker login
```

* To upload an image to docker hub, run the following

```
sudo docker push username/example:0.00.01
```

* If not uploading to docker hub, create a .tar file with

```
docker image save -o example.example_build:0.00.01.tar example/example_build:0.00.01
```

* This can be copied via ssh with scp or moved via USB.

#### Placing image onto target device <a name="imageimport"></a>

* Pull from docker repo on target device by

```
docker login
docker pull [OPTIONS] NAME[:TAG|@DIGEST]
```

* If not using docker hub, load a .tar with

```
docker load < example.example_build:0.00.01.tar
```

* Run the docker image, build, and run the application

```
sudo docker run -it --rm example/example_build:0.00.01 \
	bash -c \
	"cd repoName/build && \
	cmake .. && \
	make && \
	./applicationName"
```

* Remember to reference the image based on how it was imported. Starting with username if pulled from docker hub.
* It is possible to set up so that docker image runs on target device setup with systemd.
* This needs looked into.

### Cleaning up images <a name="cleanup"></a>

* Removes all images on the hosted machine

```
sudo docker image prune -a
```

* Removes image by id.

```
sudo docker image ls
sudo docker rmi <imageID>
```

* If docker cannot remove the image, can be forced by adding -f option, or can kill all running containers

```
docker kill $(docker ps -q)
```

### Using make to implement docker commands <a name="makecommands"></a>

* This section is intended to be used on a dev machine.
* Must have existing make file in main repo (recommend just starting with one in this repo).
* Copy directory buildtools with sub makefile to main project repo.
* Ensure top level make includes make file in buildtools
* Run make commands from the main repo with sudo
* Use make help to print options
* make with no target defaults to just build
* auto complete works when calling out command options

* In the Makefile in the upper directory, these are specific variables that should be defined
  1. PROJECT_NAME
  2. DOCKER_DEPS_VERSION
  3. DOCKER_HUB_USER <your docker hub user name>
  4. DOCKER_SYS_ARCH # set to linux/arm64 if cross-compiling

* Note that the username will be all lower case.
* From the main repo, run

```
sudo make build-docker-deps-image # This builds the image/container
```

* The image and container are now build on the hosted machine.
* The image name will be in the format <hub-user>/<repo-name>[:<tag>].
  This is necessary to properly push the image.
* Make a build folder on the hosted machine in the main repo

```
mkdir build
```

* Once the container and image exists, enter the image with

```
sudo make login # Logs into container and opens bash interface
```

* The above mounts the project repo to the image.
* The user can now make changes to the source code on the host machine and
  changes will occur live inside the container. Testing the application can be done inside the image
* Build inside the main repo of the hosted machine as normal or can run the following to build inside the image

```
sudo make build
```

* When ready to upload the image, do the following:

```
sudo make push-image-to-hub
```

* Will need to enter docker hub username and password

### Who do I talk to? <a name="who"></a>

* Jacob Morgan <jrunnerxc@gmail.com>

