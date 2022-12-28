#!/bin/sh

root_path=$(dirname $(readlink -f  "$0"))
cd ${root_path}

# vcpkg (vcpkg.json)
vcpkg_path=${root_path}/vcpkg
vcpkg_exe=${vcpkg_path}/vcpkg
echo vcpkg_path: ${vcpkg_path}
echo vcpkg_exe: ${vcpkg_exe}
if [ ! -x ${vcpkg_exe} ]; then
  echo "run vcpkg/bootstrap-vcpkg.sh !!!"
  exit
fi
VCPKG_DEFAULT_TRIPLET=x64-linux
# echo "${vcpkg_exe} install --triplet x64-linux"
# ${vcpkg_exe} install --triplet x64-linux

# cmake (Debug/Release)
config=Debug
install_path=install/${config}
vcpkg_cmake=${vcpkg_path}/scripts/buildsystems/vcpkg.cmake
echo config: ${config}
echo install_path: ${install_path}
echo vcpkg_cmake: ${vcpkg_cmake}
echo "cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=${vcpkg_cmake} -DCMAKE_INSTALL_PREFIX=${install_path}"
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=${vcpkg_cmake} -DCMAKE_INSTALL_PREFIX=${install_path}
echo "cmake --build build --target install --config ${config}"
cmake --build build --target install --config ${config}

# run
export LD_LIBRARY_PATH=${install_path}/lib
echo "${install_path}/bin/sample-main"
${install_path}/bin/sample-main
