# Docker name 
# charlesdias/device_drivers_bbb
#
# docker build --no-cache --build-arg "host_uid=$(id -u)" --build-arg "host_gid=$(id -g)" -t charlesdias/device_drivers_bbb:<version> -t charlesdias/device_drivers_bbb:latest .
#
# docker push charlesdias/device_drivers_bbb --all-tags

FROM ubuntu:22.04

LABEL maintainer="Charles Dias <charlesdias@tecnofuturo.com.br>"

# Download Linux support tools
RUN apt-get update && apt-get -y upgrade && \
    apt-get -y install build-essential lzop u-boot-tools net-tools bison flex libssl-dev libncurses5-dev \
    libncursesw5-dev unzip chrpath xz-utils minicom wget git-core curl bc kmod vim libgmp-dev libmpc-dev cpio

# Clean up APT when done
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# In Ubuntu, /bin/sh is a link to /bin/dash. The dash shell does not support 
# the source command. Replace dash with bash
RUN rm /bin/sh && ln -s bash /bin/sh

# Set environment variables, which are used later in the Dockerfile.
ENV USER_NAME linuxdev

ARG host_uid=1000
ARG host_gid=1000
RUN groupadd -g $host_gid $USER_NAME && \
    useradd -g $host_gid -m -s /bin/bash -u $host_uid $USER_NAME

USER $USER_NAME

# Set up a development tools directory
WORKDIR /home/dev
ADD . /home/dev

# Get and install the Arm GCC compiler
RUN wget https://releases.linaro.org/components/toolchain/binaries/latest-7/arm-linux-gnueabihf/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz \
      && tar -xf gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz \
      && rm gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz

# Set up the compiler path
ENV PATH="/home/dev/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin:${PATH}"

# Set up the working directory
WORKDIR /home/ldd