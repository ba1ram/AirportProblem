FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    lcov \
    gcovr \
    && rm -rf /var/lib/apt/lists/*


RUN apt-get update && apt-get install -y \
    qt6-base-dev \
    qt6-base-dev-tools \
    libgl1 \
    libgl1-mesa-dev \
    libegl1 \
    libopengl0 \
    libxcb-cursor0 \
    libxcb-xinerama0 \
    libxcb-icccm4 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-randr0 \
    libxcb-render-util0 \
    libxkbcommon-x11-0 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .


RUN mkdir build && cd build && \
    cmake .. -DBUILD_TESTS=ON -DENABLE_COVERAGE=OFF && \
    make -j$(nproc)


CMD ["bash", "-c", "cd build && ctest --output-on-failure"]