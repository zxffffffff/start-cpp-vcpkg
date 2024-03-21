#!/bin/sh
set -e

root_path=$(dirname $(readlink -f "$0"))
cd ${root_path}

# vcpkg (vcpkg.json)
vcpkg_path=${root_path}/vcpkg
vcpkg_exe=${vcpkg_path}/vcpkg
echo vcpkg_path: ${vcpkg_path}
echo vcpkg_exe: ${vcpkg_exe}
if [ ! -x ${vcpkg_exe} ]; then
  vcpkg_sh=${vcpkg_path}/bootstrap-vcpkg.sh
  echo vcpkg_sh: ${vcpkg_sh}
  sudo apt-get update
  sudo apt-get install build-essential tar curl zip unzip
  sudo sh ${vcpkg_sh}
fi

# cmake (Debug/Release)
config=Release
install_path=install/${config}
vcpkg_cmake=${vcpkg_path}/scripts/buildsystems/vcpkg.cmake
echo config: ${config}
echo install_path: ${install_path}
echo vcpkg_cmake: ${vcpkg_cmake}
rm -rf ${install_path}

echo "cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=${vcpkg_cmake} -DCMAKE_INSTALL_PREFIX=${install_path}"
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=${vcpkg_cmake} -DCMAKE_INSTALL_PREFIX=${install_path}

echo "cmake --build build --target install --config ${config}"
cmake --build build --target install --config ${config}

# run (main & test)
export LD_LIBRARY_PATH=${install_path}/bin
echo "${install_path}/bin/sample-main"
${install_path}/bin/sample-main
echo "${install_path}/bin/sample-tools-test"
${install_path}/bin/sample-tools-test
echo "${install_path}/bin/sample-net-test"
${install_path}/bin/sample-net-test
echo "${install_path}/bin/sample-db-test"
${install_path}/bin/sample-db-test
echo "${install_path}/bin/sample-datacenter-test"
${install_path}/bin/sample-datacenter-test
