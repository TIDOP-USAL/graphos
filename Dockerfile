FROM nvidia/cuda:11.5.2-devel-ubuntu20.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC
ENV QT_QPA_PLATFORM=offscreen
ENV XDG_RUNTIME_DIR=/app

# Sets the versions of the dependencies
ARG OPENCV_VERSION=4.5.4
ARG COLMAP_VERSION=3.7
ARG CGAL_VERSION=v5.5
ARG CERES_VERSION=2.0.0
ARG GDAL_VERSION=3.5.0
ARG PROJ_VERSION=9.0.0
ARG GLOG_VERSION=0.5.0
ARG OPENMVS_VERSION=2.2.0
#ARG POISSONRECON_VERSION=13.58

# Sets the working directory
WORKDIR /app

ARG INSTALL_PREFIX=/usr/local

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    ninja-build \
    wget \
    unzip \
    pkg-config \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libwebp-dev \
    libboost-all-dev \
    libsqlite3-dev \
    sqlite3 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* 



# Update CMake to version 3.21.4
RUN wget https://github.com/Kitware/CMake/releases/download/v3.21.4/cmake-3.21.4-linux-x86_64.sh && \
    chmod +x cmake-3.21.4-linux-x86_64.sh && \
    ./cmake-3.21.4-linux-x86_64.sh --skip-license --prefix=/usr/local && \
    rm cmake-3.21.4-linux-x86_64.sh


# Clone and compile OpenCV with CUDA support
RUN git clone --branch ${OPENCV_VERSION} https://github.com/opencv/opencv.git /tmp/opencv && \
    git clone --branch ${OPENCV_VERSION} https://github.com/opencv/opencv_contrib.git /tmp/opencv_contrib && \
    mkdir -p /tmp/opencv/build && \
    cd /tmp/opencv/build && \
    cmake .. -GNinja \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} \
          -DOPENCV_EXTRA_MODULES_PATH=/tmp/opencv_contrib/modules \
          -DWITH_CUDA=ON \
          -DCUDA_ARCH_BIN="5.0 5.2 6.0 6.1 7.0 7.5 8.0 8.6" \
          -DCUDA_ARCH_PTX="" \
          -DENABLE_FAST_MATH=OFF \
          -DCUDA_FAST_MATH=OFF \
          -DWITH_CUBLAS=ON \
          -DWITH_GSTREAMER=OFF \
          -DWITH_V4L=ON \
          -DBUILD_opencv_python2=OFF \
          -DBUILD_opencv_python3=OFF \
          -DBUILD_JAVA=OFF \
          -DBUILD_TESTS=OFF \
          -DBUILD_PERF_TESTS=OFF \
          -DCMAKE_CXX_FLAGS="-Wno-deprecated-declarations -march=x86-64 -mtune=generic" .. && \
    ninja -j2 && \
    ninja install && \
    ldconfig && \
    cd /app && \
    rm -rf /tmp/opencv /tmp/opencv_contrib


# Build and install fmt 10
RUN git clone --branch 10.0.0 https://github.com/fmtlib/fmt.git /tmp/fmt && \
    cd /tmp/fmt && \
    mkdir build && \
    cd build && \
    cmake .. -GNinja \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} \
          -DFMT_TEST=OFF \
          -DFMT_DOC=OFF && \
    ninja && \
    ninja install && \
    cd /app && \
    rm -rf /tmp/fmt


# Build and install PROJ 9.0
RUN wget https://download.osgeo.org/proj/proj-${PROJ_VERSION}.tar.gz && \
    tar -xvf proj-${PROJ_VERSION}.tar.gz && \
    cd proj-${PROJ_VERSION} && \
    mkdir build && \
    cd build && \
    cmake .. -GNinja \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} \
          -DBUILD_TESTING=OFF \
          -DENABLE_CURL=OFF \
          -DBUILD_PROJSYNC=OFF && \
    ninja && \
    ninja install && \
    ldconfig && \
    cd /app && rm -rf proj-${PROJ_VERSION} proj-${PROJ_VERSION}.tar.gz


# Build and install GDAL 3.5
RUN git clone --branch release/3.5 https://github.com/OSGeo/gdal.git /tmp/gdal && \
    cd /tmp/gdal && \
    mkdir build && \ 
    cd build && \
    cmake .. -GNinja \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} \
          -DBUILD_TESTING=OFF \        
          -DBUILD_APPS=OFF && \  
    ninja && \
    ninja install && \
    cd /app && \
    rm -rf /tmp/gdal

RUN git clone --branch 3.4 https://gitlab.com/libeigen/eigen.git /tmp/eigen && \
    cd /tmp/eigen && \
    mkdir build && \ 
    cd build && \
    cmake .. -GNinja \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} \
          -DBUILD_TESTING=OFF \   
          -DEIGEN_BUILD_TESTING=OFF \  
          -DBUILD_APPS=OFF && \  
    ninja && \
    ninja install && \
    cd /app && \
    rm -rf /tmp/eigen

RUN apt-get update && apt-get install -y \
    libexpat1-dev \
    libfreeimage-dev \
    libflann-dev \
    libmetis-dev \
    libsuitesparse-dev \
    libgflags-dev \	
    libglew-dev \
    libgtk-3-dev \
    libgmp-dev \
    libmpfr-dev \
    qtbase5-dev \
    qttools5-dev \
    qttools5-dev-tools \
    qttranslations5-l10n \
    libqt5opengl5-dev \	
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*


# Build and install glog 0.5.0
RUN git clone https://github.com/google/glog.git /tmp/glog && \
    cd /tmp/glog && \
    git checkout v${GLOG_VERSION} && \
    mkdir build && \
    cd build && \
    cmake .. -GNinja  \ 
             -DCMAKE_BUILD_TYPE=Release \ 
             -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} \ 
             -DBUILD_TESTING=OFF \
             -DBUILD_SHARED_LIBS=OFF && \
    ninja && \
    ninja && \
    cd /app && rm -rf /tmp/glog

	
# Build and install Ceres Solver 2.0.0
RUN git clone https://ceres-solver.googlesource.com/ceres-solver /tmp/ceres-solver && \
    cd /tmp/ceres-solver && \
    git checkout tags/${CERES_VERSION} && \
    mkdir build && \
    cd build && \
    cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release \
          -DBUILD_TESTING=OFF \
          -DBUILD_EXAMPLES=OFF \
          -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} && \
    ninja && \
    ninja install && \
    cd /app && rm -rf /tmp/ceres-solver
	
	
# Build and install CGAL 5.5
RUN git clone https://github.com/CGAL/cgal.git /tmp/cgal && \
    cd /tmp/cgal && \
    git fetch https://github.com/CGAL/cgal.git ${CGAL_VERSION} && \
    git checkout FETCH_HEAD && \
    mkdir build && \
    cd build && \
    cmake .. -GNinja \
          -DBUILD_TESTING=OFF \	
          -DBUILD_DOC=OFF \	
          -DWITH_tests=OFF \	
          -DWITH_demos=OFF \	
          -DWITH_examples=OFF \		
          -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} && \
    ninja && \
    ninja install && \
    cd /app && rm -rf /tmp/cgal
	

	
# Build and install COLMAP
RUN git clone https://github.com/colmap/colmap.git /tmp/colmap && \
    cd /tmp/colmap && \
    git fetch https://github.com/colmap/colmap.git ${COLMAP_VERSION} && \
    git checkout FETCH_HEAD && \
    mkdir build && \
    cd build && \
    cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CUDA_ARCHITECTURES="5.0;5.2;6.0;6.1;7.0;7.5;8.0;8.6" \ 
    -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} \
    && ninja -j2 && \
    ninja install && \
    cd /app && rm -rf /tmp/colmap

RUN git clone https://github.com/mkazhdan/PoissonRecon.git /tmp/PoissonRecon && \
    cd /tmp/PoissonRecon && \
    git checkout 19a8cbd && \
    make -j$(nproc) poissonrecon && \
    make -j$(nproc) surfacetrimmer && \
    cp Bin/Linux/PoissonRecon ${INSTALL_PREFIX}/bin/ && \
    cp Bin/Linux/SurfaceTrimmer ${INSTALL_PREFIX}/bin/ && \
    cd /app && rm -rf /tmp/PoissonRecon

# Install VCG
RUN git clone https://github.com/cdcseacave/VCG.git /tmp/vcglib && \
    mkdir -p ${INSTALL_PREFIX}/vcglib && \
    cp -r /tmp/vcglib/* ${INSTALL_PREFIX}/vcglib && \
    rm -rf /tmp/vcglib


# Build and install OpenMVS
RUN git clone https://github.com/cdcseacave/openMVS.git /tmp/openMVS && \
    cd /tmp/openMVS && \
    git checkout tags/v${OPENMVS_VERSION} && \
    mkdir _build && \
    cd _build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" \
             -DVCG_DIR="${INSTALL_PREFIX}/vcglib" \	
             -DCUDA_CUDA_LIBRARY=/usr/local/cuda-11.5/lib64/stubs/libcuda.so \
             -DOpenMVS_MAX_CUDA_COMPATIBILITY=ON \
             -DBUILD_TESTING=OFF \
             -DBUILD_SHARED_LIBS=OFF && \
    make -j2 && \
    make install && \
    cd /app && rm -rf /tmp/openMVS

RUN git clone --branch dev_3.1 https://github.com/TIDOP-USAL/tidoplib.git /tmp/tidoplib && \
    cd /tmp/tidoplib && \
    mkdir build && \
    cd build && \
    cmake .. -GNinja  \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} \
          -DWITH_OPENCV=ON \
          -DWITH_GDAL=ON \
          -DWITH_PROJ=ON \
          -DBUILD_APPS=OFF \
          -DBUILD_TEST=OFF \
          -DBUILD_DOC=OFF \
          -DWITH_CUDA=ON \
          -DTIDOPLIB_USE_SIMD_INTRINSICS=ON \
          -DTIDOPLIB_CXX_STANDARD=C++14 && \
    ninja && \
    ninja install && \
    cd /app && rm -rf /tmp/tidoplib	

# Copy project files
COPY . /app

# Build project
RUN mkdir build && \
    cd build && \
    cmake .. -GNinja \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_CUDA_COMPILER=/usr/local/cuda/bin/nvcc \
          -DBUILD_GUI=OFF \
          -DWITH_CUDA=ON \
          -DCOLMAP_ROOT=${INSTALL_PREFIX} \
          -DTidopLib_DIR=${INSTALL_PREFIX} && \
    ninja && \
    ninja install && \
    cd /app && rm -rf build

# Set the entrypoint
ENTRYPOINT ["/bin/bash"]