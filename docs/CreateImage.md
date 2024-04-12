# Creating a Docker Image #

### Assumptions:
* This document assumes that docker has been set up on the host system.
* Details for setting up Docker can be found under [DockerSetup.md](DockerSetup.md)

### Dockerfile
* There are two dockerfiles in this repository. They are:

1. DockerFileDevEnv
2. DockerFileProdEnv

* The files create the image using two build stages.
* Everything in the intermediate stage is removed from the final image, including "id_rsa".
* Using --from=intermediate allows us to copy git repositories to the final image.
* Run commands are concatenated to reduce complexity of image by decreasing layers created.

### Getting private bitbucket repositories into the development image
* Git repos are installed as part of the image using ssh.
* The user must create a public ssh key to be able to add these repos.

```
ssh-keygen
ssh-add -k ~/.ssh/id_rsa
cat ~/.ssh/id_rsa.pub | xclip -sel clip
```

* The last command copies the ssh key onto the clip board.
* Login to user bitbucket account in a browser, click on the user icon at the top right, and navigate to Personal Settings->SSH Keys
* Click "Add keys", paste the public ssh key into the box, and give the key a name.
* Then it will be required to copy the file ~/.ssh/id_rsa into this repo. The .gitignore excludes this file from pushes.
* At this point, repos can be added to the Development file as part of the image using an ssh clone request.

### Production File
* DockerFileProdEnv is used to create a production environment for an arm device.
* The production environment is a minimally set up image to be able to run projects in Ubuntu 18.04.
* Git is not installed. Therefore, cannot pull other git repos. Not meant to be a development environment.

### Development File
* DockerFileDevEnv file is used to create a development environment.
* The image is intended to be used on a host machine such as a laptop.
* Git is installed and therefore users can pull git repos into this image (must mount git repo to host if persistent changes are desired).
* Zerotier is not installed in the image. It should be installed on the host machine if it will be used.

### Docker Hub Account
* The user should have a docker hub account to have a username
* This will make moving images around devices easier with docker pull and push

### Docker Command to Create Image
* There are multiple ways to create images and images with different host architectures
* To create an image matching the host architecture, run:

```
sudo docker build --no-cache=true -t username/example:0.00.01 -f ./${DOCKER_FILE} .
```

* In the above example, DOCKER_FILE can be replaced with any of the two files in this repository.
* To be able to create architectures not matching the host, run

```
docker run --privileged --rm tonistiigi/binfmt --install all
```

* This command above sometimes needs re-run after a prune.
* It is likely that if the non-host architecture failed, that it needs re-run.
* To create an image matching that builds multiple architectures, run:

```
sudo docker buildx create --use
sudo docker buildx build --platform=linux/amd64,linux/arm64 --no-cache=true -t username/example:0.00.01 -f ./${DOCKER_FILE} --push .
```

* The above will create platforms for both Ubuntu desktop and arm64. Platforms can be added or removed as needed.
* The images are pushed to the Docker Hub Account after creation.
* To build a single image that does not match the host architecture, run:

```
sudo docker buildx create --use
sudo docker buildx build --platform=linux/arm64 --no-cache=true -t username/example:0.00.01 -f ./${DOCKER_FILE} --push .
```

* Again, platform flag argument can be replaced with whatever architecture system that is desired.
* The image will be pushed to Docker Hub once completed.
* Using make files in this repo, images can be created with the following commands:

```
sudo make build-host-arch-image
sudo make build-multiple-arch-images
sudo make build-user-defined-arch-image
```

* Using the above make command will require setting the appropriate variables in the upper level makefile.
1. PROJECT_NAME
2. DOCKER_VERSION		# Version of the image being created
3. DOCKER_HUB_USER  # Your docker hub user name
4. DOCKER_SYS_ARCH  # Tells docker what platform to build image as.
5. DOCKER_OPEN_ARCH # Tells Docker what platform to open under. Should match image system architecture.
6. HOST_MOUNT_PATH  # Tells where to mount src directory on host
