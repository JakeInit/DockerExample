# Installing Docker for Ubuntu #

### Pre-requisites
```
sudo apt install -y init-system-helpers qemu-system-x86 pass uidmap gnome-shell-extension-appindicator
```

```
sudo apt install ca-certificates curl gnupg lsb-release
sudo mkdir -p /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
  $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
sudo apt update
```

### Ubuntu 20.04 and Other OS
* Note that Docker Desktop is not supported in 18.04
* Now install [Docker Desktop](https://docs.docker.com/desktop/) from Orientation and Setup

### Ubuntu 18.04
* Use [Docker Engine](https://docs.docker.com/engine/install/ubuntu/):

```
sudo apt install docker-ce docker-ce-cli containerd.io docker-compose-plugin
```

* After installing docker, need to install the following on dev machine for creating non-host system architectures type images.

```
docker run --privileged --rm tonistiigi/binfmt --install all
```

* If builds for different architectures won't work, re-run the above. It is likely compatible builders were removed by a prune command.

### Test with Docker Sample Application
```
sudo docker run -d -p 80:80 docker/getting-started
```

* View docker in webpage at http://localhost:80

* View docker version

```
sudo docker version
```

* Example Output:
Client: Docker Engine - Community
 Version:           20.10.17
 API version:       1.41
 Go version:        go1.17.11
 Git commit:        100c701
 Built:             Mon Jun  6 23:02:56 2022
 OS/Arch:           linux/amd64
 Context:           default
 Experimental:      true

Server: Docker Engine - Community
 Engine:
  Version:          20.10.17
  API version:      1.41 (minimum version 1.12)
  Go version:       go1.17.11
  Git commit:       a89b842
  Built:            Mon Jun  6 23:01:02 2022
  OS/Arch:          linux/amd64
  Experimental:     false
 containerd:
  Version:          1.6.7
  GitCommit:        0197261a30bf81f1ee8e6a4dd2dea0ef95d67ccb
 runc:
  Version:          1.1.3
  GitCommit:        v1.1.3-0-g6724737
 docker-init:
  Version:          0.19.0
  GitCommit:        de40ad0

### General Run Format for Docker
* docker run [OPTIONS] IMAGE [COMMAND] [ARG...]
* Options found [here](https://docs.docker.com/engine/reference/commandline/run/)
* Example:

```
sudo docker run -it --rm --name=example \
    --mount type=bind,source=${PWD},target=/repoName \
	example/example_build:0.00.01 \
	bash
```

* This opens a bash environment inside the image while mounting the current directory to the image.
* The user can now make changes to the source code on the dev machine in real time and
  changes will occur live inside the image. Testing can be done inside the container.
* Build the code on the dev machine as normal when ready to test.
* The following builds the binaries inside the container mounted on the host:

```
sudo docker run -it --rm example/example_build:0.00.01 \
	bash -c \
	"cd repoName/build && \
	cmake .. && \
	make && \
	./applicationName"
```

### Cleaning up Images and Volumes

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
sudo docker kill $(docker ps -q)
```

* Clean up volumes

```
sudo docker volume rm $(sudo docker volume ls -q --filter dangling=true)
```

```
sudo docker volume prune
```
