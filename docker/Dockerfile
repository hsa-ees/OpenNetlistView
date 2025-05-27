FROM debian:bookworm AS system

# user id of the user
ARG USER_ID=1000
ARG GROUP_ID=1000

USER root

SHELL [ "/bin/bash", "-c" ]


################################################################################

# setting needed variables for build
ENV DEBIAN_FRONTEND=noninteractive
ENV EMSCRIPT_VERSION="3.1.56"
ENV QT_VERSION="6.8.1"
ENV QT_VERSION_SHORT="6.8"
ENV QT_DIR="/opt/qt"
ENV QT_WASM_DIR="${QT_DIR}/wasm_singlethread"
ENV QT_DESKTOP_DIR="${QT_DIR}/gcc_64"

ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

### Install dependencies #######################################################

FROM system AS deps

RUN apt-get update -q \
    && apt-get install -qy --no-install-recommends \
    build-essential \
    cmake \
    clang \
    git ca-certificates \
    wget \
    python3 \
    llvm-dev \
    libgl1-mesa-dev \
    libxcb-xinerama0-dev \
    libfontconfig1-dev \
    libclang-dev \
    libc++-dev \
    libc++abi-dev \
    ninja-build \
    curl \
    doxygen \
    graphviz \
    libfontconfig1-dev \
    libfreetype-dev \
    libx11-dev \
    libx11-xcb-dev \
    libxcb-cursor-dev \
    libxcb-glx0-dev \
    libxcb-icccm4-dev \
    libxcb-image0-dev \
    libxcb-keysyms1-dev \
    libxcb-randr0-dev \
    libxcb-render-util0-dev \
    libxcb-shape0-dev \
    libxcb-shm0-dev \
    libxcb-sync-dev \
    libxcb-util-dev \
    libxcb-xfixes0-dev \
    libxcb-xinerama0-dev \
    libxcb-xkb-dev \
    libxcb1-dev \
    libxext-dev \
    libxfixes-dev \
    libxi-dev \
    libxkbcommon-dev \
    libxkbcommon-x11-dev \
    libxrender-dev \
    libwayland-dev \
    libwayland-egl1-mesa \
    libwayland-server0 \
    libgles2-mesa-dev \
    xvfb \
    xauth \
    libxcb-cursor0 \
    libxcb-cursor-dev && \
    rm -rf /var/lib/apt/lists/* \
    && apt-get -y autoremove \
    && apt-get -y autoclean \
    && rm -rf /var/lib/apt/lists/*


#### Webassembly ###############################################################

FROM deps AS wasm

# install the webasembly emscripten compiler
RUN git clone https://github.com/emscripten-core/emsdk.git /opt/emsdk && \
    cd /opt/emsdk && ./emsdk install  ${EMSCRIPT_VERSION} && ./emsdk activate  ${EMSCRIPT_VERSION} && \
    echo "source /opt/emsdk/emsdk_env.sh" >> ~/.bashrc && \
    source ~/.bashrc

# add the wasm tool to path
ENV PATH="/opt/emsdk/upstream/emscripten:${PATH}"

#### QT GCC ####################################################################

FROM wasm AS qt_gcc

WORKDIR /build

# download the qt src
RUN wget https://download.qt.io/official_releases/qt/${QT_VERSION_SHORT}/${QT_VERSION}/single/qt-everywhere-src-${QT_VERSION}.tar.xz

RUN mkdir -p ${QT_DESKTOP_DIR} && \
    tar xf qt-everywhere-src-${QT_VERSION}.tar.xz && \
    cd qt-everywhere-src-${QT_VERSION} && mkdir build && cd build &&\
    ../configure -platform linux-g++ -prefix ${QT_DESKTOP_DIR} -nomake examples -nomake tests -xcb && \
    ninja -j$(nproc) && ninja install && cd .. && rm -rf build


####  QT Webassembly ###########################################################
FROM qt_gcc AS qt_wasm

# set the host qt as variable for wasm build
ENV QT_HOST_PATH=${QT_DESKTOP_DIR}

# build the wasm build of qt
RUN mkdir -p $QT_WASM_DIR && \
    source /opt/emsdk/emsdk_env.sh && \
    cd qt-everywhere-src-${QT_VERSION} && mkdir build && cd build && \
    ../configure -xplatform wasm-emscripten -prefix $QT_WASM_DIR -nomake examples -nomake tests -feature-wasm-exceptions && \
    ninja -j$(nproc) && ninja install && cd .. && rm -rf build



#### qt_cleanup ################################################################

FROM qt_wasm AS qt_cleanup

# cleanup the qt source
RUN rm -rf /build

#### Doc #######################################################################
FROM qt_cleanup AS doc

# install requirments for documentation
RUN apt-get update && apt-get install -y \
    python3 python3-sphinx python3-myst-parser python3-sphinx-rtd-theme \
    python3-breathe python3-sphinxcontrib.svg2pdfconverter inkscape gpg

# install miktex for pdf generation
RUN curl -fsSL https://miktex.org/download/key | gpg --dearmor | tee /usr/share/keyrings/miktex-keyring.gpg > /dev/null
RUN echo "deb [signed-by=/usr/share/keyrings/miktex-keyring.gpg] https://miktex.org/download/debian bookworm universe" | tee /etc/apt/sources.list.d/miktex.list
RUN apt-get update && apt-get install -y \
    miktex

# configure miktex
RUN miktexsetup --verbose --package-set=basic --shared=yes finish
RUN initexmf --verbose --admin --set-config-value [MPM]AutoInstall=1
RUN mpm --admin --verbose --package-level=basic --upgrade

#### Final ####################################################################
FROM doc AS final

# add clang-tidy and clang-format
# ssh for git clone and such
# fuse for appimages
# gdb for debugging
# libjemalloc as dependency for program to prevent lockups during run on native linux
RUN apt-get update && apt-get install -y \
    clang-tidy clang-format openssh-client fuse gdb \
    libjemalloc2 libjemalloc-dev && \
    rm -rf /var/lib/apt/lists/*

# add user-user with uid ${USER_ID} and gid ${USER_ID}
# needs to be replaced by host uid and gid of user
# on linux host, where user is running
RUN groupadd -g ${GROUP_ID} user && groupmod -g ${USER_ID} user && \
    useradd -s /bin/bash -m -u ${USER_ID} -g user user && \
    export HOME=/home/user

# add entrypoint.sh and give it to the user user
COPY docker-entrypoint.sh /usr/local/bin/
RUN chown user:user /usr/local/bin/docker-entrypoint.sh

# need to have permissions on the emsdk installation
# otherwise the configuration will fail
# add emsdk group to prevent compile errors
RUN groupadd emsdk && usermod -aG emsdk user
# give the /opt/emsdk folder to the emsdk group
RUN chown -Rv root:emsdk /opt/emsdk
# change the permissions for the group to 7
RUN chmod -Rv g+rwx /opt/emsdk

USER user

# set locale
ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8
ENV LD_LIBRARY_PATH="${QT_DIR}/gcc_64/lib"

# add the emscripten to the bashrc file
ENV QT_QPA_PLATFORM_PLUGIN_PATH=/opt/qt/gcc_64/plugins/platforms
ENV EMSDK_QUIET=1
RUN echo "source /opt/emsdk/emsdk_env.sh" >> ~/.bashrc

ENTRYPOINT ["/usr/local/bin/docker-entrypoint.sh"]
