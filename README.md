# Docker Example Application #

* [What is this repository for?](#whatfor)
* [Terminology](#terminology)
* [How to get set up?](#setup)
* [Install and Register Nvidia](#nvidia)
* [Testing Docker in this Repo](#repoTest)
* [Who do I talk to?](#who)

### What is this repository for? <a name="whatfor"></a>

* To show how to use docker with an example project

### Terminology <a name="terminology"></a>
* Dev Machine - the device where the application development is being done (most likely a laptop)
* Target Machine - the device that the containerized image is meant to be run on.
* Hosted Machine - the device that holds the docker container. Can be the dev or target machine.

### How to get set up? <a name="setup"></a>

* For Ubuntu 18.04, install docker engine from [here](https://docs.docker.com/engine/install/ubuntu/)
* For additional help on installing docker for Ubuntu 18.04, see [docs/DockerSetup.md](docs/DockerSetup.md)
* For other OS, install docker desktop for GUI interface. Download and setup found [here](https://docs.docker.com/get-started/)
* Create a docker hub account [here](https://hub.docker.com/).
* To be able to push to docker hub, delete the file config.json inside ~/.docker on the hosted machine (verified on Ubuntu 18.04)
* If Nvidia CUDA support is needed, follow directions in section [Install and Register Nvidia](#nvidia). This has not been tested.
* The example in this repo is based on this [link](https://ddanilov.me/dockerized-cpp-build)
* Source for creating images for specific architecture systems found [here](https://developer.arm.com/documentation/102475/0100/Multi-architecture-images)
* Source for cross compiling (much faster) found [here](https://www.docker.com/blog/faster-multi-platform-builds-dockerfile-cross-compilation-guide/)
* Specific project work flow can be found in [docs/Workflow.md](docs/Workflow.md)

### Install and Register Nvidia <a name="nvidia"></a>

* It is uncertain if CUDA support is needed, but this sections shows how to set it up if needed.
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

### Testing Docker in this Repo <a name="repoTest"></a>
* This section verifies docker is installed and working properly on the host machine.
* The test was created for x86_64 Linux Ubuntu 18.04 host machine. Later versions of Ubuntu may work, but are not verified.
* An image is needed to run this test.
* There are pre-made images available to pull.
* To get the hello world pre-made linux/amd64 image, run:

```
sudo docker login
sudo docker pull jakeinit/hello_world:0.00.01
```

* If an image needs to be created, see [docs/CreateImage.md](docs/CreateImage.md)
* For the hello_world image, make sure these parameters are set in the top level make file:
1. PROJECT_NAME=hello_world
2. DOCKER_DEPS_VERSION=0.00.01
3. DOCKER_HUB_USER=jakeinit
4. DOCKER_SYS_ARCH=linux/amd64
5. HOST_MOUNT_PATH=${CURDIR}

Then run:

```
sudo make login
```

* The above will open the image in a folder named "hello_world" with current host directory files and folders mounted inside.
* All changes done to files and folders in image or on host machine will be reflected on both image and host.
* exit the image with "ctrl + d"
* Run

```
sudo make build
```

* This will build the hello world binary in the image architecture format and will reside in the host machine build folder
* The binary can be tested with

```
./build/Example
```

* Output: "Hello From Docker"

### Who do I talk to? <a name="who"></a>

* Jacob Morgan <jrunnerxc@gmail.com>
