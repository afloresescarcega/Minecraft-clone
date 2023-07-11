#! /bin/bash -
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/:$PKG_CONFIG_PATH
export PKG_CONFIG_PATH=/usr/local/Cellar/glfw/3.3.8/lib/pkgconfig/:$PKG_CONFIG_PATH
export CMAKE_PREFIX_PATH="/usr/local/opt/glew/lib;$CMAKE_PREFIX_PATH"
export CMAKE_PREFIX_PATH="/usr/local/opt/glew/lib;$CMAKE_PREFIX_PATH"

if [ -d "build" ]; then
  rm -rf "build"
fi

mkdir "build"
cd build
cmake -D CMAKE_PREFIX_PATH="/usr/local/opt/glew/lib;/usr/local/opt/glfw/lib" ..
make