FROM nvidia/cuda:12.2.2-devel-ubuntu22.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

WORKDIR /app

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
	ninja-build \
    wget \
    unzip \
    pkg-config \
    libopencv-dev \
    libgdal-dev \
    #libboost-all-dev \
    libboost-program-options-dev \
    libboost-filesystem-dev \
    libboost-graph-dev \
    libboost-system-dev \
    libproj-dev \
    libfmt-dev \
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
	
	
# Build and install TidopLib
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

    
# Build and install COLMAP
RUN git clone https://github.com/colmap/colmap.git /tmp/colmap && \
    cd colmap && \
    git fetch https://github.com/colmap/colmap.git ${COLMAP_GIT_COMMIT} && \
    git checkout FETCH_HEAD && \
    mkdir build && \
    cd build && \
    cmake .. -GNinja -DCMAKE_CUDA_ARCHITECTURES=${CUDA_ARCHITECTURES} && \
    ninja && \
    ninja install && \
    cd /app && rm -rf /tmp/colmap
	
	
COPY . /app
	
RUN mkdir build && \
    cd build && \ 
    cmake .. -GNinja -DCMAKE_CUDA_COMPILER=/usr/local/cuda/bin/nvcc . && \
    -DBUILD_GUI=OFF \
    make -j$(nproc)
	
CMD ["./bin/graphos --help"]