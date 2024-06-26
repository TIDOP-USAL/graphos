![GRAPHOS](res/img/graphos.png)

[![License: GPL v3](https://img.shields.io/badge/License-GPL_v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

Open-source photogrammetric software for 3D reconstruction.

![gui](res/img/gui.png)

## Getting Started

### Prerequisites


* **[CMake](https://cmake.org/) v3.10+**
* **C++ Compiler** with support for C++14
* Git

### Dependencies

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

## Docker 

### Build the Docker Image:

docker build -t graphos .

### Running a Container with GPU Support:

docker run --gpus all --name GRAPHOS -it graphos
docker run -it --name GRAPHOS -v /home/user/data:/app graphos


## License

GRAPHOS is distributed under the GNU General Public License Version 3 (GPL v3). Please see the [LICENSE](LICENSE) file for more information.