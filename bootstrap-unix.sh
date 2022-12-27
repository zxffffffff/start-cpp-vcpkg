#!/bin/sh

root_path=$(dirname $(readlink -f  "$0"))
cd ${root_path}

# vcpkg (vcpkg.json)
vcpkg_path=${root_path}/vcpkg
vcpkg_exe=${vcpkg_path}/vcpkg
echo vcpkg_path: ${vcpkg_path}
echo vcpkg_exe: ${vcpkg_exe}
if [ ! -d ${vcpkg_exe} ]; then
  vcpkg_script=${vcpkg_path}/bootstrap-vcpkg.sh
  echo vcpkg_script: ${vcpkg_script}
  ${vcpkg_script}
fi
VCPKG_DEFAULT_TRIPLET=x64-windows
# echo "${vcpkg_exe} install --triplet x64-windows"
# ${vcpkg_exe} install --triplet x64-windows

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
vcpkg_installed=build/vcpkg_installed/x64-windows
if "${config}"=="Debug" then
  vcpkg_installed=${vcpkg_installed}/debug
fi
echo vcpkg_installed: ${vcpkg_installed}
echo "cp ${vcpkg_installed}/bin/*.so ${install_path}/bin"
cp ${vcpkg_installed}/bin/*.so ${install_path}/bin
echo "${install_path}/bin/sample-main"
${install_path}/bin/sample-main
