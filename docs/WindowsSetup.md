# Windows Setup #

* Install [Chocolatey](https://chocolatey.org/install) inside powershell as an admin

```
choco install make
choco install cmake
choco install mingw
```
Edit system environmental variables under control panel and then search environemntal variables.
Add the following under path or verify that it exists:

```
C:\ProgramData\chocolatey\bin
```

In the powershell commandline:
```
wsl --install
wsl --update
```

Can install Ubuntu on Windows if desired. First check what is available then install desired version
```
wsl --list --online
wsl --install -d Ubuntu-22.04
```

View installed distros with
```
wsl -l -v
```

Get [Docker Desktop](https://docs.docker.com/desktop/install/windows-install/) if desired.
```
choco install docker-desktop
```

* Docker engine is used in this repo for the projects.
* Will most likely use Docker engine since it is free for corporate use.

```
choco install docker-engine
choco install docker-cli
choco install docker-compose
```
