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
ARG GDAL_VERSION=3.5
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
          -DBUILD_opencv_apps=OFF \
          -DBUILD_opencv_aruco=OFF \
          -DBUILD_opencv_bgsegm=OFF \
          -DBUILD_opencv_bioinspired=OFF \
          -DBUILD_opencv_calib3d=ON \
          -DBUILD_opencv_ccalib=OFF \
          -DBUILD_opencv_core=ON \
          -DBUILD_opencv_datasets=OFF \
          -DBUILD_opencv_cudaarithm=ON \
          -DBUILD_opencv_cudabgsegm=OFF \
          -DBUILD_opencv_cudacodec=OFF \
          -DBUILD_opencv_cudafeatures=ON \
          -DBUILD_opencv_cudafilters=ON \
          -DBUILD_opencv_cudaimgproc=ON \
          -DBUILD_opencv_cudalegacy=ON \
          -DBUILD_opencv_cudaobjdetect=OFF \
          -DBUILD_opencv_cudaoptflow=OFF \
          -DBUILD_opencv_cudastereo=OFF \
          -DBUILD_opencv_cudawarping=ON \
          -DBUILD_opencv_cudev=ON \
          -DBUILD_opencv_dnn=ON \
          -DBUILD_opencv_dnn_objdetect=OFF \
          -DBUILD_opencv_dnn_superres=OFF \
          -DBUILD_opencv_dpm=OFF \
          -DBUILD_opencv_face=OFF \
          -DBUILD_opencv_features=ON \
          -DBUILD_opencv_flann=ON \
          -DBUILD_opencv_fuzzy=OFF \
          -DBUILD_opencv_gapi=OFF \
          -DBUILD_opencv_hfs=OFF \
          -DBUILD_opencv_highgui=ON \
          -DBUILD_opencv_img_hash=OFF \
          -DBUILD_opencv_imgcodecs=ON \
          -DBUILD_opencv_imgproc=ON \
          -DBUILD_opencv_intensity_transform=OFF \
          -DBUILD_opencv_line_descriptor=OFF \
          -DBUILD_opencv_mcc=OFF \
          -DBUILD_opencv_ml=OFF \
          -DBUILD_opencv_objc_bindings_generator=OFF \
          -DBUILD_opencv_objdetect=ON \
          -DBUILD_opencv_optflow=OFF \
          -DBUILD_opencv_phase_unwrapping=OFF \
          -DBUILD_opencv_photo=ON \
          -DBUILD_opencv_plot=OFF \
          -DBUILD_opencv_python_bindings_generator=OFF \
          -DBUILD_opencv_python_tests=OFF \
          -DBUILD_opencv_quality=OFF \
          -DBUILD_opencv_rapid=OFF \
          -DBUILD_opencv_reg=OFF \
          -DBUILD_opencv_rgbd=OFF \
          -DBUILD_opencv_saliency=OFF \
          -DBUILD_opencv_shape=ON \
          -DBUILD_opencv_signal=OFF \
          -DBUILD_opencv_stereo=OFF \
          -DBUILD_opencv_stitching=ON \
          -DBUILD_opencv_structured_light=OFF \
          -DBUILD_opencv_superres=OFF \
          -DBUILD_opencv_surface_matching=OFF \
          -DBUILD_opencv_text=OFF \
          -DBUILD_opencv_tracking=OFF \
          -DBUILD_opencv_ts=OFF \
          -DBUILD_opencv_video=ON \
          -DBUILD_opencv_videoio=ON \
          -DBUILD_opencv_videostab=OFF \
          -DBUILD_opencv_wechat_qrcode=OFF \
          -DBUILD_opencv_world=OFF \
          -DBUILD_opencv_xfeatures2d=ON \
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


# Build and install PROJ
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


# Build and install GDAL
RUN git clone --branch release/${GDAL_VERSION} https://github.com/OSGeo/gdal.git /tmp/gdal && \
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

# EIGEN
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
    libgeotiff-dev \
    libssl-dev \
    libcurl4-openssl-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*


# Build and install glog 0.5.0
RUN git clone https://github.com/google/glog.git /tmp/glog && \
    cd /tmp/glog && \
    git checkout v${GLOG_VERSION} && \
    mkdir build && \
    cd build && \
    cmake .. -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} \ 
             -DBUILD_TESTING=OFF \
             -DBUILD_SHARED_LIBS=OFF && \
    make -j$(nproc) && \
    make install && \
    cd /app && rm -rf /tmp/glog

	
# Build and install Ceres Solver
RUN git clone https://ceres-solver.googlesource.com/ceres-solver /tmp/ceres-solver && \
    cd /tmp/ceres-solver && \
    git checkout tags/${CERES_VERSION} && \
    mkdir build && \
    cd build && \
    cmake .. -GNinja \
          -DBUILD_TESTING=OFF \
          -DBUILD_EXAMPLES=OFF \
          -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} && \
    ninja && \
    ninja install && \
    cd /app && rm -rf /tmp/ceres-solver
	
	
# Build and install CGAL
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

    
RUN git clone https://github.com/hobuinc/laz-perf.git  /tmp/laz-perf && \
    cd /tmp/laz-perf && \
    mkdir build && cd build && \
    cmake .. -GNinja  \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" && \
    ninja && \
    ninja install && \
    cd /app && rm -rf /tmp/laz-perf

RUN git clone https://github.com/RockRobotic/copc-lib.git  /tmp/copc-lib && \
    cd /tmp/copc-lib && \
    mkdir build && \
    cd build && \
    cmake .. -GNinja  \
             -DCMAKE_BUILD_TYPE=Release \
             -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" && \
    ninja && \
    ninja install && \
    cd /app && rm -rf /tmp/copc-lib

RUN git clone https://github.com/PDAL/PDAL.git /tmp/PDAL && \
    cd /tmp/PDAL && \
    git checkout 2.8.0 && \
    mkdir build && \
    cd build && \
    cmake .. -GNinja  \
             -DCMAKE_BUILD_TYPE=Release \
             -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" \
             -DWITH_TESTS=OFF && \
    ninja && \
    ninja install && \
    cd /app && rm -rf /tmp/PDAL


RUN git clone https://github.com/TIDOP-USAL/tidoplib.git /tmp/tidoplib && \
    cd /tmp/tidoplib && \
    git checkout 3.2.0 && \ 
    mkdir build && \
    cd build && \
    cmake .. -GNinja  \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} \
          -DWITH_OPENCV=ON \
          -DWITH_GDAL=ON \
          -DWITH_PROJ=ON \
          -DBUILD_TL_FEAT_MATCH=OFF \
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
          -DBUILD_TRANSLATION=OFF \
          -DWITH_CUDA=ON \
          -DCOLMAP_ROOT=${INSTALL_PREFIX} \
          -DTidopLib_DIR=${INSTALL_PREFIX} && \
    ninja && \
    ninja install && \
    cd /app && rm -rf build

RUN chown -R root:root /app && chmod -R 755 /app

ENV GRAPHOS_GDAL=${INSTALL_PREFIX}/share/gdal
ENV GRAPHOS_PROJ=${INSTALL_PREFIX}/share/proj

# Set the entrypoint
ENTRYPOINT ["/bin/bash"]