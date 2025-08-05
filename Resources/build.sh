#!/bin/bash

# Define some global variables
ft_developer=/Applications/Xcode.app/Contents/Developer
# Your signing identity to sign the xcframework. Execute "security find-identity -v -p codesigning" and select one from the list
ft_sing_identity=B42A10624E8E06BC95CD03069100C6E67121D61B


# Console output formatting
# https://stackoverflow.com/a/2924755
bold=$(tput bold)
normal=$(tput sgr0)


# Create the build directory if not exists
mkdir -p build


# Remove logs if exist
rm -f "build/log.txt"


exit_if_error() {
  local result=$?
  if [ $result -ne 0 ] ; then
     echo "Received an exit code $result, aborting"
     exit 1
  fi
}


# Compile the library for specific OS and architecture
build_library() {
  local ft_target=$1
  local ft_platform=$2
  local ft_arch=$3
  local ft_host=$4
  local ft_min_os=$5

  ft_sysroot="$ft_developer/Platforms/$ft_platform.platform/Developer/SDKs/$ft_platform.sdk"

  echo "Build for ${bold}$ft_target${normal}"

  # Remove build directory for the current platform if exists
  rm -rf build/$ft_target
  exit_if_error

  # Clean last CMake configuration if exists
  make clean >> build/log.txt
  #exit_if_error
  
  # Configure CMake
  echo "Configure CMake"
  bash configure --prefix=$(pwd)/build/$ft_target \
    --host=$ft_host \
    --enable-static=yes \
    --enable-shared=no \
    CC="$ft_developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang -isysroot $ft_sysroot" \
    CFLAGS="-arch $ft_arch -std=c17 -mtargetos=$ft_min_os -I$ft_sysroot/usr/include/libxml2" \
    LDFLAGS="-arch $ft_arch" >> build/log.txt
  exit_if_error
  
  # Build
  make >> build/log.txt
  exit_if_error
  
  # Install compiled libraries and headers into the install folder
  make install >> build/log.txt
  exit_if_error

  # Copy custom umbrella header
  cp libfreetype.h build/$ft_target/include/freetype2/libfreetype.h
  exit_if_error

  # The module.modulemap seems useless, ignore it at the moment
  # Copy the module map into the directory with installed header files
  #mkdir -p build/$ft_target/include/Modules
  #cp module.modulemap build/$ft_target/include/Modules/module.modulemap
  #exit_if_error

 # Strip installed library
 strip -S build/$ft_target/lib/libfreetype.a >> build/log.txt
 exit_if_error
}

build_library macos-arm64              MacOSX           arm64  arm-apple-darwin    macos11
build_library macos-x86_64             MacOSX           x86_64 x86_64-apple-darwin macos10.12
build_library ios-arm64                iPhoneOS         arm64  arm-apple-darwin    ios12
build_library ios-simulator-arm64      iPhoneSimulator  arm64  arm-apple-darwin    ios14-simulator
build_library ios-simulator-x86_64     iPhoneSimulator  x86_64 x86_64-apple-darwin ios12-simulator
build_library tvos-arm64               AppleTVOS        arm64  arm-apple-darwin    tvos12
build_library tvos-simulator-arm64     AppleTVSimulator arm64  arm-apple-darwin    tvos12-simulator
build_library tvos-simulator-x86_64    AppleTVSimulator x86_64 x86_64-apple-darwin tvos12-simulator
build_library watchos-arm64            WatchOS          arm64  arm-apple-darwin    watchos8
build_library watchos-simulator-arm64  WatchSimulator   arm64  arm-apple-darwin    watchos8-simulator
build_library watchos-simulator-x86_64 WatchSimulator   x86_64 x86_64-apple-darwin watchos8-simulator
build_library xros-arm64               XROS             arm64  arm-apple-darwin    xros1
build_library xros-simulator-arm64     XRSimulator      arm64  arm-apple-darwin    xros1-simulator
build_library xros-simulator-x86_64    XRSimulator      x86_64 x86_64-apple-darwin xros1-simulator


create_framework() {
  # Remove previously created framework if exists
  rm -rf build/FreeType.xcframework
  exit_if_error

  # Merge macOS arm and x86 binaries
  mkdir -p build/macos
  exit_if_error
  lipo -create -output build/macos/libfreetype.a \
    build/macos-arm64/lib/libfreetype.a \
    build/macos-x86_64/lib/libfreetype.a
  exit_if_error

  # Merge iOS simulator arm and x86 binaries
  mkdir -p build/ios-simulator
  exit_if_error
  lipo -create -output build/ios-simulator/libfreetype.a \
    build/ios-simulator-arm64/lib/libfreetype.a \
    build/ios-simulator-x86_64/lib/libfreetype.a
  exit_if_error

  # Merge tvOS simulator arm and x86 binaries
  mkdir -p build/tvos-simulator
  exit_if_error
  lipo -create -output build/tvos-simulator/libfreetype.a \
    build/tvos-simulator-arm64/lib/libfreetype.a \
    build/tvos-simulator-x86_64/lib/libfreetype.a
  exit_if_error

  # Merge watchOS simulator arm and x86 binaries
  mkdir -p build/watchos-simulator
  exit_if_error
  lipo -create -output build/watchos-simulator/libfreetype.a \
    build/watchos-simulator-arm64/lib/libfreetype.a \
    build/watchos-simulator-x86_64/lib/libfreetype.a
  exit_if_error

  # Merge visionOS simulator arm and x86 binaries
  mkdir -p build/xros-simulator
  exit_if_error
  lipo -create -output build/xros-simulator/libfreetype.a \
    build/xros-simulator-arm64/lib/libfreetype.a \
    build/xros-simulator-x86_64/lib/libfreetype.a
  exit_if_error

  # Create the framework with multiple platforms
  xcodebuild -create-xcframework \
    -library build/macos/libfreetype.a             -headers build/macos-arm64/include/freetype2 \
    -library build/ios-arm64/lib/libfreetype.a     -headers build/ios-arm64/include/freetype2 \
    -library build/ios-simulator/libfreetype.a     -headers build/ios-simulator-arm64/include/freetype2 \
    -library build/tvos-arm64/lib/libfreetype.a    -headers build/tvos-arm64/include/freetype2 \
    -library build/tvos-simulator/libfreetype.a    -headers build/tvos-simulator-arm64/include/freetype2 \
    -library build/watchos-arm64/lib/libfreetype.a -headers build/watchos-arm64/include/freetype2 \
    -library build/watchos-simulator/libfreetype.a -headers build/watchos-simulator-arm64/include/freetype2 \
    -library build/xros-arm64/lib/libfreetype.a    -headers build/xros-arm64/include/freetype2 \
    -library build/xros-simulator/libfreetype.a    -headers build/xros-simulator-arm64/include/freetype2 \
    -output build/FreeType.xcframework
  exit_if_error

  # And sign the framework
  codesign --timestamp -s $ft_sing_identity build/FreeType.xcframework
  exit_if_error
}
create_framework




# fin