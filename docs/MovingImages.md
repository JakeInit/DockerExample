# Moving Images
* This file shows basic functionality for moving images to different devices.
* This document assumes that a Docker Hub account has been created.

#### Exporting the image from the dev machine
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
docker image save -o username/example:0.00.01.tar username/example:0.00.01
```

* If images were built using buildx (used when making images not matching host architecture), then may need to get image id:

```
sudo docker buildx imagetools inspect username/example:0.00.01
```

* Example output:

Name:      docker.io/jakeinit/hello_world:0.00.01
MediaType: application/vnd.docker.distribution.manifest.list.v2+json
Digest:    sha256:7aa217f7bb18ea20c2b6518a27c79dbd7119de8671a939807bc2097101ee4d0a

Manifests:
Name:      docker.io/jakeinit/hello_world:0.00.01@sha256:b4b22a5164be8b8e21fe683f40ac139a11a0f3cdf564fd1e591ce3189d7ee89e
MediaType: application/vnd.docker.distribution.manifest.v2+json
Platform:  linux/amd64

Name:      docker.io/jakeinit/hello_world:0.00.01@sha256:93c51b8491c287ddf5eaf7f2c4313c63cc91ce9f7d5c43143405b90ab6437f40
MediaType: application/vnd.docker.distribution.manifest.v2+json
Platform:  linux/arm64

* Note there are two different architectures here (amd64 and arm64)
* Therefore, to reference the arm architecture

```
sudo docker run -it --rm --name="hello_world_build" --workdir=/hello_world --mount type=bind,source=/home/jake/cppLibraries/Docker_HelloWorld,target=/hello_world jakeinit/hello_world:0.00.01@sha256:93c51b8491c287ddf5eaf7f2c4313c63cc91ce9f7d5c43143405b90ab6437f40 bash
```

* The above will run the arm container version and below will create a tar for the above hello_world example.

```
sudo docker image save -o hello.tar jakeinit/hello_world:0.00.01@sha256:93c51b8491c287ddf5eaf7f2c4313c63cc91ce9f7d5c43143405b90ab6437f40
```

* The tar files can be copied via ssh with scp or moved via USB.

#### Placing image onto target device

* Pull from docker repo on target device by

```
docker login
docker pull [OPTIONS] NAME[:TAG|@DIGEST]
```

* If not using docker hub, load a .tar with

```
docker load < username/example:0.00.01.tar
```

* Remember to reference the image based on how it was imported. Starting with username if pulled from docker hub.
* It is possible to set up so that docker image runs on target device setup with systemd.
* This needs looked into.
