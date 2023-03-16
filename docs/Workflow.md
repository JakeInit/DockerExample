# Workflow #
* This document explains how to use docker in a workflow.
* Images for other projects can be created similarly and follow a similar workflow. See CreateImage.md
* It is assumed that Docker has been set up on the host machine. See DockerSetup.md

## The Development Environment
* A development environment image was created.
* The environment simulates a Linux/arm64 machine.
* It is intended to be used on a development device such as a laptop.
* Pull the development image:

```
sudo docker pull DOCKER_HUB_USER/PROJECT_NAME:DOCKER_VERSION
```

* A volume can be created for any folder on the image for changes to remain persistent.
* Volumes are not part of the image and reside on the host machine and need manually deleted if no longer desired.

### Make Commands
* The user should copy the folder buildtools and the file "Makefile" to the project repo that is being developed.
* This will allow for make commands that will simplify using docker.\
* To see list of options with descriptions, run

```
make help
```

* The following variables should be set in the top level make file
1. PROJECT_NAME
2. DOCKER_VERSION
3. DOCKER_HUB_USER  # Your docker hub user name
4. DOCKER_SYS_ARCH  # Tells docker what platform to build image as.
5. HOST_MOUNT_PATH  # Tells where to mount src directory on host

* These parameters can be verified by running:

```
make print
```

* The make functionality has tab completion. Therefore, it is not required to fully type the desired functionality.

### Project Development
* Updates and changes to the git repo should be done as normal
* To run the docker container and open an interactive bash inside it, run:

```
sudo make login
```

* This will open the image inside a folder based on the project name.
* A volume can be created to make a folder persistent.
* All files in the mounted repo should be seen in the image.
* The user can choose to work in this environment or on the host.
* Changes on either will be reflected in both because the repo is mounted to the image during container runtime.
* This will allow the user to test in an arm environment.
* After a build, all build files will appear in the build folder of the git repository.
* Note that the binaries will be arm architecture.

## Using the Production Environment

* This is intended to be used on an arm device.

```
docker pull DOCKER_HUB_USER/PROJECT_NAME:DOCKER_VERSION
```

* This will pull the production image.
* Binaries should be placed in this environment in order to test on the module.
* Binaries can be set to run on start up for the final production product.
