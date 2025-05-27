#!/bin/bash

BUILD_GENERATOR="Ninja"

# check if the QT_DIR environment variable is set

if [ -z "$QT_DIR" ]; then
    echo "Error: QT_DIR environment variable is not set"
    exit 1
fi

# check if libjemalloc is installed for development use pkg-config to check
if ! pkg-config --exists jemalloc; then
    echo "Error: libjemalloc is not installed"
    exit 1
fi

# change into the directory the script is located in
cd "$(dirname "$0")"

# create the build folder if it doesn't exist
if [ ! -d "appimg-build" ]; then
    mkdir appimg-build
fi

cd appimg-build

# copy the default AppDir
cp -r ../appdir .

/usr/bin/cmake \
    -DCMAKE_BUILD_TYPE:STRING=Release \
    -DCMAKE_TOOLCHAIN_FILE:FILEPATH=$QT_DIR/lib/cmake/Qt6/qt.toolchain.cmake \
    -DCMAKE_INSTALL_PREFIX:FILEPATH=appdir/usr -G $BUILD_GENERATOR ..

if [ $? -ne 0 ]; then
    echo "Error: cmake configuration failed"
    exit 1
fi

ninja -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Error: ninja build failed"
    exit 1
fi

ninja install

if [ $? -ne 0 ]; then
    echo "Error: ninja install failed"
    exit 1
fi

# Install the required dependencies

if [[ ! -f "linuxdeploy-x86_64.AppImage" ]]; then
    wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
    chmod u+x linuxdeploy-x86_64.AppImage
fi

if [[ ! -f "linuxdeploy-plugin-qt-x86_64.AppImage" ]]; then
    wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
    chmod u+x linuxdeploy-plugin-qt-x86_64.AppImage
fi


cd ..

# get the version from git
export VERSION=$(git describe --tags --always --dirty)

# create the QMAKE variable needed for linuxdeploy with the qt plugin
export QMAKE=$QT_DIR/bin/qmake
export LD_LIBRARY_PATH=$QT_DIR/lib
export EXTRA_PLATFORM_PLUGINS=libqwayland-generic.so
export EXTRA_QT_MODULES="waylandcompositor"

# run linuxdeploy
if [[ -f "appimg-build/linuxdeploy-x86_64.AppImage" ]]; then
    appimg-build/linuxdeploy-x86_64.AppImage -l /usr/lib/x86_64-linux-gnu/libjemalloc.so.2  --appdir appimg-build/appdir --plugin qt --output appimage
else
    echo "Error: linuxdeploy-x86_64.AppImage not found"
    exit 1
fi


