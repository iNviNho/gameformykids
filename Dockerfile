FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    git \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libwayland-dev \
    wayland-protocols \
    libxrandr-dev \
    libxinerama-dev
    libxcursor-dev \
    libxi-dev \
    zlib1g-dev
    pkg-config \
    libxkbcommon-dev
    python3 \
    python3-pip \
    python3-dev

WORKDIR /project

COPY . .

RUN cmake -S . -B build
RUN cmake --build build --target mygame -j$(nproc) 
