![GRAPHOS](res/img/graphos.png)

[![License: GPL v3](https://img.shields.io/badge/License-GPL_v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

GRAPHOS is a open-source photogrammetric software for 3D reconstruction.

![gui](res/img/gui.png)

# Getting Started

## Releases

You can download the latest version of GRAPHOS from the GitHub [Releases](https://github.com/TIDOP-USAL/graphos/releases) section.

## Building with CMake

### Prerequisites

* **[CMake](https://cmake.org/) v3.10+**
* **C++ Compiler** with support for C++14
* **Git**

### Dependencies

To properly install and run GRAPHOS, you need the following dependencies:

- [Qt5](https://www.qt.io) [![License: LGPL v3](https://img.shields.io/badge/License-LGPL_v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)
- [tidoplib](https://github.com/TIDOP-USAL/tidoplib) [![License: LGPL v3](https://img.shields.io/badge/License-LGPL_v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)
    - [Boost](https://www.boost.org) [![License](https://img.shields.io/badge/License-Boost_1.0-lightblue.svg)](https://www.boost.org/LICENSE_1_0.txt)
    - [OpenCV](https://opencv.org) [![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
    - [GDAL](https://gdal.org/index.html) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
    - [PROJ](https://proj.org/en/9.4/) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
- [COLMAP](https://colmap.github.io) [![License](https://img.shields.io/badge/License-BSD_3--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)
  - [glog](https://github.com/google/glog) [![License](https://img.shields.io/badge/License-BSD_3--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)
  - [gflags](https://github.com/gflags/gflags) [![License](https://img.shields.io/badge/License-BSD_3--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)
  - [Ceres Solver](http://ceres-solver.org) [![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
  - [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page) [![License: MPL 2.0](https://img.shields.io/badge/License-MPL_2.0-brightgreen.svg)](https://opensource.org/licenses/MPL-2.0)
- [OpenMVS](https://github.com/cdcseacave/openMVS) [![License: AGPL v3](https://img.shields.io/badge/License-AGPL_v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0)
- [PoissonRecon](https://github.com/mkazhdan/PoissonRecon) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
- [CloudCompare](https://www.danielgm.net/cc/) [![License: GPL v2](https://img.shields.io/badge/License-GPL_v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)
- [CMVS](https://www.di.ens.fr/pmvs/) [![License: GPL v3](https://img.shields.io/badge/License-GPL_v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
- [SMVS](https://github.com/flanggut/smvs?tab=BSD-3-Clause-1-ov-file) [![License](https://img.shields.io/badge/License-BSD_3--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

Make sure your environment has these dependencies installed before proceeding with the build.

### Cloning the Repository

```bash
git clone https://github.com/TIDOP-USAL/graphos.git
cd graphos
```

### Building with CMake

Once you have the dependencies installed and the repository cloned, follow these steps to build GRAPHOS using CMake.

#### Step 1: Create the Build Directory

Create a `build` directory where the build files will be generated:

```bash
mkdir build
cd build
```

#### Step 2: CMake Configuration

Configure the project using CMake. If you want to build a version that uses GPU, ensure CUDA support is available. If you do not want to use CUDA, you can disable it during the configuration:

```bash
# To build with CUDA support
cmake .. -DWITH_CUDA=ON

# Or, to build without CUDA support
cmake .. -DWITH_CUDA=OFF
```

#### Step 3: Building the Project

Once configured, compile the project by running:

```bash
make -j$(nproc)
```

The `-j$(nproc)` argument parallelizes the build using all available CPU cores, speeding up the process.

#### Step 4: Installation

After the build completes, install GRAPHOS on your system:

```bash
sudo make install
```

## Docker

GRAPHOS can be run in a Docker container, enabling ease of setup and GPU support for computational tasks.

### Build the Docker Image

To build a Docker image for GRAPHOS, navigate to the root directory of the repository (where the Dockerfile is located) and run:

```bash
docker build -t graphos .
```

This command builds the Docker image and tags it as graphos.

### Running a Container with GPU Support

To run GRAPHOS in a Docker container with GPU support, use the following command:

```bash
docker run -it --name graphos --gpus all -v /home/user/data:/app graphos
```

**Explanation:**

- `-it` Runs the container in interactive mode with a TTY shell.

- `--name graphos` Names the container `graphos` for easier reference.

- `--gpus all` Enables GPU acceleration inside the container. Requires NVIDIA Docker support.

- `-v /home/user/data:/app`  Mounts the local directory `/home/user/data` to `/app` inside the container, allowing the container to access data on the host machine.

### Using GRAPHOS Inside the Container

Once inside the container, you can use GRAPHOS commands as described in this documentation. For example:

```bash
graphos createproj --name my_project
graphos image_manager -p my_project/my_project.xml -i /app/images/image01.jpg
graphos featextract -p my_project/my_project.xml
```

## Issues and Contributions:

If you encounter any issues or would like to contribute, please visit the [Issues](https://github.com/TIDOP-USAL/graphos/issues) section on GitHub.

# GRAPHOS Command-Line Interface

## Overview

GRAPHOS can be used via a graphical interface or from the command line. This document provides instructions on using the available GRAPHOS commands through the command-line interface.

### Usage

```bash
graphos [--version] [-h | --help] [--licence] <command> [<args>]

```

To see help for a specific command:
```bash
graphos <command> --help
```

### Available Commands

- `createproj` - Create a new Graphos project.
- `image_manager` - Manage images in a Graphos project.
- `featextract` - Extract features (SIFT) from images.
- `featmatch` - Match features between images.
- `gcps` - Import ground control points.
- `ori` - Perform 3D reconstruction.
- `dense` - Generate a dense point cloud.
- `mesh` - Create a Poisson reconstruction mesh.
- `undistort` - Undistort images.
- `dem` - Create DSM and/or DTM.
- `export_point_cloud` - Export point cloud.

## Command Details

### `createproj`
Create a new Graphos project.

#### Usage
```bash
graphos createproj [OPTION...]
```

#### Options
- `-n, --name [R]` - Project name or project file (.xml). (Required)
- `-d, --description [O]` - Project description. (Optional)
- `-o, --overwrite [O]` - Force project overwrite (default = false). (Optional)

#### Example
```bash
graphos createproj --name inspector
```

---

### `image_manager`
Manage images in a Graphos project.

#### Usage
```bash
graphos image_manager [OPTION...]
```

#### Options

- `-p, --prj [R]` - Project file. (Required)
- `-i, --image [O]` - Image to add or remove (with option [--delete|-d]). (Optional)
- `-l, --image_list [O]` - List of images to add or remove (with option [--delete|-d]). (Optional)
- `-d, --delete [O]` - Delete an image from the project. (Optional)
- `-c, --camera [O]` - Camera type. Supported values: [Pinhole 1, Pinhole 2, Radial 1, Radial 2, OpenCV 1, OpenCV Fisheye, OpenCV 2, Radial Fisheye 1, Radial Fisheye 2, Radial 3]. (Optional)

#### Examples

```bash
graphos image_manager -p 253/253.xml -i image001.jpg
graphos image_manager -p 253/253.xml -i image001.jpg -d
```

---

### `featextract`

Extract features (SIFT) from images.

#### Usage

```bash
graphos featextract [OPTION...]
```

#### Options

- `-p, --prj [R]` - Project file. (Required)
- `-s, --max_image_size [O]` - Maximum image size (default = 3200). (Optional)
- `--max_features_number [O]` - Maximum number of features to detect (default = 5000). (Optional)
- `--octave_resolution [O]` - SIFT: Number of layers in each octave (default = 3). (Optional)
- `--contrast_threshold [O]` - SIFT: Contrast Threshold (default = 0.006667). (Optional)
- `--edge_threshold [O]` - SIFT: Threshold for filtering out edge-like features (default = 10.0). (Optional)
- `--disable_cuda [O]` - Disable CUDA (default = false). (Optional)

#### Example

```bash
graphos featextract -p 253/253.xml
```

---

### `gcps`

Import of control points

#### Usage
```bash
graphos gcps [OPTION...]
```

#### Options

- `-p, --prj [R]` Project file.
- `--cp [R]` Ground control points file.
- `--crs [O]` CRS.

#### Example

```bash
graphos gcps --p 253/253.xml --cp 253/georef.xml
```

---

### `ori`
Perform 3D reconstruction.

#### Usage
```bash
graphos ori [OPTION...]
```

#### Options

- `-p, --prj [R]` - Project file. (Required)
- `-c, --fix_calibration [O]` - Fix calibration. (Optional)
- `-p, --fix_poses [O]` - Fix poses. (Optional)
- `-a, --absolute_orientation [O]` - Absolute orientation. (Optional)

#### Example

```bash
graphos ori -p 253/253.xml -a
```

---

### `dense`
Generate a dense point cloud.

#### Usage
```bash
graphos dense [OPTION...]
```

#### Options

- `-p, --prj [R]` - Project file. (Required)
- `-m, --method [O]` - Densification method. Supported values: [mvs, pmvs, smvs]. (Optional)
- Other method-specific options like resolution, colors, visibility, smoothing, etc.
- `--disable_cuda [O]` - Disable CUDA (default = false). (Optional)

#### Example

```bash
graphos dense -p 253/253.xml --method pmvs
```

---

### `mesh`

Create a Poisson reconstruction mesh.

#### Usage

```bash
graphos mesh [OPTION...]
```

#### Options

- `-p, --prj [R]` - Project file. (Required)
- `--depth [O]` - Maximum reconstruction depth. (Optional)
- `--boundary_type [O]` - Boundary type. Supported values: [Free, Dirichlet, Neumann]. (Optional)

#### Example

```bash
graphos mesh -p 253/253.xml --depth 12 --boundary_type Dirichlet
```

---

### `dem`

Create DSM (Digital Surface Model) and/or (Digital Terrain Model).

#### Usage

```bash
graphos dem [OPTION...]
```

#### Options

- `-p, --prj [R]` - Project file. (Required)
- `-g, --gsd [O]` - Ground sample distance. (Optional)
- `--dsm [O]` - Create a Digital Surface Model. (Optional)
- `--dtm [O]` - Create a Digital Terrain Model. (Optional)

#### Examples
```bash
graphos dem -p 253/253.xml --gsd 0.1
```

---

### `export_point_cloud`

Export point cloud

#### Usage

```bash
graphos export_point_cloud [OPTION...]
```

#### Options

- `-p, --prj [R]` - Project file.
- `-f, --file [R]` - Export file.
- `--crs [O]` - CRS of the point cloud (default: CRS of the project ).

#### Examples

```bash
graphos export_point_cloud -p 253/253.xml --file point_cloud.ply
```

---


# License

GRAPHOS is distributed under the GNU General Public License Version 3 (GPL v3). Please see the [LICENSE](LICENSE) file for more information.
