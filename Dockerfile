FROM nvidia/cuda:11.5.2-cudnn8-devel-ubuntu20.04

# Establece argumentos para OpenCV y Colmap
ARG OPENCV_VERSION=4.5.4
ARG COLMAP_GIT_COMMIT=dev

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

# Establece el directorio de trabajo
WORKDIR /app

# Instala las dependencias necesarias
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    ninja-build \
    wget \
    unzip \
    pkg-config \
    libgdal-dev \
    libboost-program-options-dev \
    libboost-filesystem-dev \
    libboost-graph-dev \
    libboost-system-dev \
    libproj-dev \
    libeigen3-dev \
    libflann-dev \
    libfreeimage-dev \
    libmetis-dev \
    libgoogle-glog-dev \
    libsqlite3-dev \
    libglew-dev \
    qtbase5-dev \
    libqt5opengl5-dev \
    libcgal-dev \
    libceres-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Clona y compila OpenCV con soporte para CUDA
RUN wget https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip -O /tmp/opencv.zip && \
    wget https://github.com/opencv/opencv_contrib/archive/${OPENCV_VERSION}.zip -O /tmp/opencv_contrib.zip && \
    unzip /tmp/opencv.zip -d /tmp && \
    unzip /tmp/opencv_contrib.zip -d /tmp && \
    mkdir -p /tmp/opencv-${OPENCV_VERSION}/build && \
    cd /tmp/opencv-${OPENCV_VERSION}/build && \
    cmake -D CMAKE_BUILD_TYPE=Release \
          -D CMAKE_INSTALL_PREFIX=/usr/local \
          -D OPENCV_EXTRA_MODULES_PATH=/tmp/opencv_contrib-${OPENCV_VERSION}/modules \
          -D WITH_CUDA=ON \
          -D WITH_CUBLAS=ON \
          -D WITH_GSTREAMER=ON \
          -D WITH_V4L=ON \
          -D BUILD_opencv_python2=OFF \
          -D BUILD_opencv_python3=OFF \
          -D BUILD_opencv_python_test=OFF \		  
          -D BUILD_JAVA=OFF \
          -D BUILD_opencv_java_bindings_generator=OFF \
          -D BUILD_opencv_python_bindings_generator=OFF \
          -D BUILD_TESTS=OFF \
          -D BUILD_PERF_TESTS=OFF .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig && \
    rm -rf /tmp/opencv-${OPENCV_VERSION} /tmp/opencv_contrib-${OPENCV_VERSION} /tmp/*.zip

# Clona, compila e instala fmt versión 10
RUN git clone --branch 10.0.0 https://github.com/fmtlib/fmt.git /tmp/fmt && \
    cd /tmp/fmt && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j$(nproc) && \
    make install && \
    cd / && \
    rm -rf /tmp/fmt

# Clona, compila e instala tidoplib
RUN git clone https://github.com/TIDOP-USAL/tidoplib /tmp/tidoplib && \
    cd /tmp/tidoplib && \
    mkdir build && \
    cd build && \
    cmake .. -GNinja \
    -DWITH_OPENCV=ON \
    -DWITH_GDAL=ON \
    -DWITH_PROJ=ON \
    -DBUILD_APPS=OFF \
    -DBUILD_TEST=OFF \
    -DBUILD_DOC=OFF \
    -DTIDOPLIB_USE_SIMD_INTRINSICS=ON \
    -DTIDOPLIB_CXX_STANDARD=C++14 \
    && ninja && \
    ninja install && \
    cd /app && rm -rf /tmp/tidoplib

# Clona, compila e instala COLMAP
RUN git clone https://github.com/colmap/colmap.git /tmp/colmap && \
    cd /tmp/colmap && \
    git fetch https://github.com/colmap/colmap.git ${COLMAP_GIT_COMMIT} && \
    git checkout FETCH_HEAD && \
    mkdir build && \
    cd build && \
    cmake .. -GNinja -DCMAKE_CUDA_ARCHITECTURES="all" && \
    ninja && \
    ninja install && \
    cd /app && rm -rf /tmp/colmap

# Copia el código fuente de la aplicación al contenedor
COPY . /app

# Establece el directorio de trabajo
WORKDIR /app

# Configura y compila la aplicación
RUN mkdir build && \
    cd build && \
    cmake .. -GNinja -DCMAKE_CUDA_COMPILER=/usr/local/cuda/bin/nvcc -DBUILD_GUI=OFF && \
    ninja

# Define el punto de entrada por defecto
CMD ["./bin/graphos"]