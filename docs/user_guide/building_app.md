(chp:building_it_yourself)=

# Building it yourself

This chapter describes how to set up the OpenNetlistView project by building it on a Linux
system.

```{note}
If you want to run the prebuilt AppImage you need to look at the chapter {ref}`chp:prebuilt`
```

(sec:building_it_yourself:prerequisites)=

## Getting the repository

To get the repository you need to clone it from the git server using
the following command:

```bash
git clone https://github.com/hsa-ees/OpenNetlistView.git
```

(sec:building_it_yourself:installation:env)=

## Setting up the environment

There are three ways to set up the environment for the application:

- Using the provided Dockerfile (native and wasm)
- Installing Qt through the Qt installer (native only)
- Manually compiling the QT library (native and wasm)

Those ways are in the following sections.

You can chose the way that fits your needs the best. And don't need to
follow all of them.

```{important}
It is recommended to use the Docker container as a development environment.
Because it provides a reproducible way to build the application.
```

(sec:building_it_yourself:setting_up_env:requirements)=

### Requirements

In order to build and run the application you need
to install the following dependencies:

Native Executable:

- Qt6 6.8 or higher
- CMake 3.19 or higher
- Ninja
- A C++17 compiler
- libjemalloc2

For Wayland support you need the following dependencies
for Qt and the application:

- libwayland-dev
- libwayland-egl1-mesa
- libwayland-server0
- libgles2-mesa-dev
- libxkbcommon-dev

Additionaly needed for a Web Assembly build:

- emsdk (for Qt6.8 emsdk 3.1.56)

For building the documentation you need to install the following dependencies:

- Doxygen
- Sphinx
- Breathe
- sphinx_rtd_theme
- myst-parser
- sphinxcontrib-svg2pdfconverter
- inkscape

For generating a AppImage:

- fuse

(sec:building_it_yourself:setting_up_env:installation_wasm)=

### Installation with docker (recommended) (native and wasm)

(sec:building_it_yourself:setting_up_env:docker:container_setup)=

#### Setting up the Container

Because the QT library, for web assembly, provided by the QT installer does not
support exceptions it is necessary to compile the QT library from source.
The docker container can also be used for the native build.

To make this step easier a Dockerfile is provided in the repository.
To build the QT library for web assembly with the Dockerfile you need to run the following commands:

```{note}
In order to run the following commands you need to have Docker installed on your system.
An installation guide for Docker can be found [here](https://docs.docker.com/engine/install/).
```

````{note}
In order to run the `docker-create-diag-image.sh` script and docker commands you
need to be in the docker group or run the command with sudo.

Adding the user to the docker group can be done with the following command:
```bash
sudo usermod -aG docker <username>
```
After this you need to re-login or restart the system.

````

```{note}
The build can take a long time depending on the system you are using.
On a system with 8 cores it can take `~1,5 hour`.
The created image is around `~12GB` in size. During the build `~40GB` are needed.
Also `16GB` of RAM are recommended. If you don't have enough RAM you can use a swap
file.
```

````{important}
On some linux distros, such as arch, the `USER_ID` might not be passed to the docker build
process. In this case the build will fail. To fix this you need to set the `USER_ID`
environment variable inside the `Dockerfile` to the correct value. You
can find the correct value by running the following command:

```bash
id -u
```
````

Execute the following command in the `docker` directory of the repository:

```bash
./docker-create-diag-image.sh
```

Now an image with the name `diag_image` should be created on your system.
You can check this with the following command:

```bash
docker images
```

(sec:building_it_yourself:setting_up_env:installation_wasm:docker:container_usage)=

##### Starting the Container

Before starting the container you need to configure the X server on your system.
To do this you need to run the following command:

```bash
xhost +local:docker
```

```{note}
If you don't want to run the `xhost` command every time you start the container you can add the following line to your `.xsessionsrc` file.
```

The following steps need to be taken in order to start the container:

1. Change into the repository directory

2. Start the container with the `docker compose`. This creates the `diag_container` if
   non existent otherwise it starts the container from the `docker-compose.yml` file
   located in the repository from the image created in the previous step (see ).

```bash
docker compose up -d
```

```{note}
The behavior of the `docker compose` is defined in the `docker-compose.yml` file.
If you want to change the behavior you can change the file accordingly.
When building the native application you need to comment the line for the wasm build and uncomment the line for the native build.
```

3. Open a terminal inside the container.

   - As the normal user

   ```bash
   docker exec -it diag_container bash
   ```

   - As the root user

   ```bash
   docker exec -it --user root diag_container bash
   ```

```{note}
The repository is mounted in the container under `~/diag_viewer`.
```

```{hint}
It is possible to start the container as a development environment inside VSCode.
To do this you need to install the `Remote - Containers` extension in VSCode.
After installing the extension you can open the repository. VSCode will ask you if you want to open the repository in a container.
```

(sec:building_it_yourself:setting_up_env:installation_native)=

### Installation with the QT installer (native only)

When compiling the application natively on your system it is
sufficient to install the dependencies with the QT installer.
Which can be found [here](https://www.qt.io/download-open-source).

```{note}
For all the other requirements you can look into the docker file, located under `docker/Dockerfile`
to see how to install them.
Most of the dependencies are available in the package manager of your distribution.
Emscripten needs to be installed manually.
```

(sec:building_it_yourself:setting_up_env:native_wasm)=

### Manual Compilation of the QT Library (native and wasm)

If you want to build the QT library for web assembly without Docker you can run the
commands contained in the Dockerfile under `docker/Dockerfile` manually.

```{note}
For all the other requirements you can look into the docker file, located under `docker/Dockerfile`
to see how to install them.
Most of the dependencies are available in the package manager of your distribution.
Emscripten needs to be installed manually.
```

(sec:building_it_yourself:building)=

## Building the application

(sec:building_it_yourself:building:config)=

### Configuration

The first step is to configure the project with CMake.
To help with this step a script is provided in the repository.
This `setup.sh` script will configure the project and create a `build` directory in the repository.

In oder for the script to work you need to set the `QT_DIR` environment variable to the
directory where the QT library is installed.

It depends if the build is for a native application or for a web assembly application where the QT library is installed.

(sec:building_it_yourself:building:config:qt_dir)=

#### Setting `QT_DIR`

(sec:building_it_yourself:building:config:qt_dir:docker)=

##### Docker

When building the project with the docker container the the `QT_DIR` environment variable is set in the docker compose file.

Deppending if you want to build the project for wasm or for native you need to comment the line for the native build and uncomment the line for the wasm build.

```{note}
This needs to be done before running the `docker compose up -d` command.

If the container is already running you need to stop the container with the `docker compose down` command and start it again with the `docker compose up -d` command. The commands need to be run in the repository directory where the `docker-compose.yml` file is located.
```

(sec:building_it_yourself:building:config:qt_dir:native)=

##### Native tools

When the tools are installed native on the system the `QT_DIR` environment variable needs to be set.

E.g. for a native build on Linux

```bash
export QT_DIR=<Path to QT>/gcc_64
```

or for a web assembly build on Linux

```bash
export QT_DIR=<Path to QT>/wasm_singlethreaded
```

(sec:building_it_yourself:building:config:script)=

#### Running the configuration script

After setting the `QT_DIR` environment variable you can run the configuration script with the following command:

```bash
./setup.sh
```

The script has multiple switches to configure the project.

| Switch         | Description                                                     |
| -------------- | --------------------------------------------------------------- |
| -h             | Show help message and exit                                      |
| -d             | Enable documentation build                                      |
| -p             | Enable profiling                                                |
| -t             | Enable tests                                                    |
| --btype <type> | Set the build type (Debug, Release, RelWithDebInfo, MinSizeRel) |

**Note:** By default the script will configure the project with the `Release` build type.

Example for a debug build with documentation and tests enabled:

```bash
./setup.sh -d -t --btype Debug
```

(sec:building_it_yourself:building:build)=

### Building

After configuring the project with CMake you can build the project with the following command in the `build` directory:

```bash
ninja
```

(sec:building_it_yourself:building:appimage)=

#### Creating a AppImage

It is also possible to create an AppImage of the application.
For this the `create-appimage.sh` script is provided in the repository.
To create the AppImage run the following command in the root of the repository:

```bash
./create-appimage.sh
```

```{note}
In order for this to work the `QT_DIR` environment variable needs to be set.
For a native build.

For this the Wayland libraries are mandatory.
```

The AppImage will be created root of the repository.

(sec:building_it_yourself:building:doc)=

#### Documentation

If you enabled the documentation build with the `setup.sh` script you can build the documentation with the following command in the `build` directory:

For the doxygen documentation:

```bash
ninja doxygen
```

For the user documentation:

```bash
ninja user_guide
```

(sec:building_it_yourself:building:test)=

#### Tests

If you enabled the tests build with the `setup.sh` script you can run the tests with the following command in the `build` directory:

Building the tests:

```bash
ninja
```

Running the tests:

```bash
ctest -T test --output-on-failure
```

(sec:building_it_yourself:building:install)=

### Installing

(sec:building_it_yourself:building:install:native)=

#### Native application

After building the project you can install the application with the following command in the `build` directory:

```bash
ninja install
```

````{note}
The application will be installed in the `/usr/local/bin` directory.
It might be necessary to add the QT library for ```gcc_64``` to the `LD_LIBRARY_PATH` environment variable.

This can be done with the following command:

```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<Path to QT>/gcc_64/lib
```

````

(sec:building_it_yourself:building:install:wasm)=

#### Web assembly application

Installing the web assembly application is not necessary.
Though it is possible to host the application on a web server.
For this the following files need to be hosted on the web server:

- `OpenNetlistView.html`
- `OpenNetlistView.js`
- `OpenNetlistView.wasm`
- `qtlogo.svg`
- `qtloader.js`

(sec:building_it_yourself:running)=

## Running the application

(sec:building_it_yourself:running:native)=

### Native application

To run the native application you need to run the following command inside
the `build` directory:

```bash
./OpenNetlistView
```

or alternatively, if you built the AppImage:

```bash
./OpenNetlistView-<version>.AppImage
```

(sec:building_it_yourself:running:wasm)=

### Web assembly application

To run the web assembly application you need to run the following command inside
the `build` directory:

```bash
python3 -m http.server
```

After running the command you can open the web browser and navigate to
`http://localhost:8000/OpenNetlistView.html` to view the application.

**Alternatively** you can run the following command, inside the `build` directory
to start the web assembly application:

```bash
emrun OpenNetlistView.html
```

```{important}
When running the web assembly application with `emrun` the browser will open automatically.
This needs an installed browser on your system. So it will not work in the docker container.
```

(sec:building_it_yourself:first_diag)=

## Displaying the first diagram

Information on displaying the first diagram can be found in chapter
{ref}`chp:fist_diag`
