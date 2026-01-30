# bash

# FreeType

# Define some global variables
developer_path="/Applications/Xcode.app/Contents/Developer"
# Your signing identity to sign the xcframework. Execute "security find-identity -v -p codesigning" and select one from the list
signing_identity=YOUR_SIGNING_IDENTITY

# Android NDK path
ndk_path="/Users/evgenij/Library/Android/sdk/ndk/29.0.13846066"

# FreeType source code folder
target="freetype-2.14.1"


# Console output formatting
# https://stackoverflow.com/a/2924755
bold=$(tput bold)
normal=$(tput sgr0)


exit_if_error() {
  local result=$?
  if [ $result -ne 0 ] ; then
     echo "Received an exit code $result, aborting"
     exit 1
  fi
}


build_library() {
  local platform=$1
  local arch=$2
  local target_os=$3

  # Reset variables
  export LT_SYS_LIBRARY_PATH=""
  export AR=""
  export CC=""
  export AS=""
  export CXX=""
  export LD=""
  export RANLIB=""
  export STRIP=""
  export CPPFLAGS=""
  export CFLAGS=""

  # Determine host based on platform and architecture
  # Apple
  if [[ "$platform" == "MacOSX" ]] || \
    [[ "$platform" == "iPhoneOS" ]] || [[ "$platform" == "iPhoneSimulator" ]] || \
    [[ "$platform" == "AppleTVOS" ]] || [[ "$platform" == "AppleTVSimulator" ]] || \
    [[ "$platform" == "WatchOS" ]] || [[ "$platform" == "WatchSimulator" ]] || \
    [[ "$platform" == "XROS" ]] || [[ "$platform" == "XRSimulator" ]]; then
    # Determine the host
    if   [[ "$arch" == "arm64" ]];  then local host="arm-apple-darwin"
    elif [[ "$arch" == "x86_64" ]]; then local host="x86_64-apple-darwin"
    fi

    # LibPNG artifact
    libpng_artifact_path="/Users/evgenij/Developer/Xcode\\\\ projects/LibPNG/Binaries/png.xcframework"

    # Get the xcframework target name
    if [[ "$platform" == "MacOSX" ]]; then
      local framework_target="macos-arm64_x86_64"
    elif [[ "$platform" == "iPhoneOS" ]]; then
      local framework_target="ios-$arch"
    elif [[ "$platform" == "iPhoneSimulator" ]]; then
      local framework_target="ios-arm64_x86_64-simulator"
    elif [[ "$platform" == "AppleTVOS" ]]; then
      local framework_target="tvos-$arch"
    elif [[ "$platform" == "AppleTVSimulator" ]]; then
      local framework_target="tvos-arm64_x86_64-simulator"
    elif [[ "$platform" == "WatchOS" ]]; then
      local framework_target="watchos-$arch"
    elif [[ "$platform" == "WatchSimulator" ]]; then
      local framework_target="watchos-arm64_x86_64-simulator"
    elif [[ "$platform" == "XROS" ]]; then
      local framework_target="xros-$arch"
    elif [[ "$platform" == "XRSimulator" ]]; then
      local framework_target="xros-arm64_x86_64-simulator"
    else
      echo "Unknown platform $platform"
      exit 1
    fi

    # Target sdk path
    local sysroot="$developer_path/Platforms/$platform.platform/Developer/SDKs/$platform.sdk"

    # Provide the system zlib library from the Xcode toolchain
    export ZLIB_CFLAGS="-I${sysroot}/usr/include"
    export ZLIB_LIBS="-L${sysroot}/usr/bin -llibz.tbd"

    # Provide the system bzip2 library from the Xcode toolchain
    enable_bzip2=yes
    export BZIP2_CFLAGS="-I${sysroot}/usr/include"
    export BZIP2_LIBS="-L${sysroot}/usr/bin -llibbz2.tbd"

    # Provide libpng precompiled libraries from the LibPNG Xcode Framework
    export LIBPNG_CFLAGS="-I\"$libpng_artifact_path/$framework_target/Headers\""
    # Something breaks in the "-L" flag when the libpng_artifact_path variable contains whitespaces, no matter how they are escaped
    # export LIBPNG_LIBS="-L\"$libpng_artifact_path/$framework_target\" -llibpng16"
    # Use direct path to the static library instead, with the "-l" (not "-L") flag:
    export LIBPNG_LIBS="-l\"$libpng_artifact_path/$framework_target/libpng16.a\""

    local arch_flags="-arch $arch"
    local target_os_flags="-mtargetos=$target_os"
    export CC="$developer_path/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
    export LT_SYS_LIBRARY_PATH="-isysroot $sysroot/usr/include"
    export CPPFLAGS="-I$sysroot/usr/include"
    export CFLAGS="-isysroot $sysroot $arch_flags -std=c17 $target_os_flags -O2"
  # Android
  elif [[ "$platform" == "Android" ]]; then
    # Determine the host
    if   [[ "$arch" == "aarch64" ]];  then local host="aarch64-linux-android"
    elif [[ "$arch" == "arm" ]];      then local host="arm-linux-androideabi"
    elif [[ "$arch" == "i686" ]];     then local host="i686-linux-android"
    elif [[ "$arch" == "riscv64" ]];  then local host="riscv64-linux-android"
    elif [[ "$arch" == "x86_64" ]];   then local host="x86_64-linux-android"
    fi

    # LibPNG artifact
    libpng_artifact_path='/Users/evgenij/Developer/Xcode projects/LibPNG/Binaries/png.artifactbundle'

    # Target sdk path
    local sysroot="$ndk_path/toolchains/llvm/prebuilt/darwin-x86_64/sysroot"

    # Provide the system zlib library from the Android NDK toolchain
    export ZLIB_CFLAGS="-I${sysroot}/usr/include"
    export ZLIB_LIBS="-L${sysroot}/usr/lib/${host} -llibz.a"

    # Provide the system bzip2 library from the Android NDK toolchain
    # bzip2 is not available as a system library in Android
    enable_bzip2=no
    #export BZIP2_CFLAGS="-I${sysroot}/usr/include"
    #export BZIP2_LIBS="-L${sysroot}/usr/lib/${host} -llibbz2.a"

    # Provide libpng precompiled libraries from the LibPNG artifact bundle
    export LIBPNG_CFLAGS="-I\"$libpng_artifact_path/Include\""
    export LIBPNG_LIBS="-l\"$libpng_artifact_path/$host/libpng16.a\""

    local arch_flags=""
    local target_os_flags="--target=$host$target_os"
    export LT_SYS_LIBRARY_PATH=""

    local toolchain="$ndk_path/toolchains/llvm/prebuilt/darwin-x86_64"
    export AR=$toolchain/bin/llvm-ar
    export CC="$toolchain/bin/clang $target_os_flags"
    export AS=$CC
    export CXX="$toolchain/bin/clang++ $target_os_flags"
    export LD=$toolchain/bin/ld
    export RANLIB=$toolchain/bin/llvm-ranlib
    export STRIP=$toolchain/bin/llvm-strip

    export CPPFLAGS=""
    export CFLAGS="-std=c17 -O2"
  else
    echo "Unknown platform $platform"
    exit 1
  fi

  prefix=$(pwd)/build/$platform/$arch/install
  
  # Create the build directory if not exists
  mkdir -p build/$platform/$arch/tmp
  exit_if_error

  cd build/$platform/$arch/tmp
  exit_if_error

  if false; then
    sh ./../$target/configure -h -n
    return
  fi

  TO_IMPLEMENT=no
  sh ./../../../../$target/configure \
  --host=$host \
  --prefix $prefix \
  --enable-shared=no \
  --enable-static=yes \
  --enable-year2038 \
  --with-zlib=yes \
  --with-bzip2=$enable_bzip2 \
  --with-png=yes \
  --with-harfbuzz=no \
  --with-brotli=$TO_IMPLEMENT \
  --with-librsvg=$TO_IMPLEMENT \
  #--with-sysroot[=DIR] \
  #--target=TARGET \
  #--with-old-mac-fonts=no \
  #--with-fsspec=no \
  #--with-fsref=no \
  #--with-quickdraw-toolbox=no \
  #--with-quickdraw-carbon=no \
  #--with-ats=yes
  exit_if_error

  make
  exit_if_error

  make install
  exit_if_error

  # Go back to origin
  cd ../../../..

  # Remove temporary files
  rm -rf build/$platform/$arch/tmp

  # Copy custom umbrella header
  cp Contents/libfreetype.h build/$platform/$arch/install/include/freetype2/libfreetype.h
  exit_if_error

  # About modules
  # https://clang.llvm.org/docs/Modules.html
  # Without module.modulemap libfreetype is not exposed to Swift
  # Copy the module map into the directory with installed header files
  mkdir -p build/$platform/$arch/install/include/freetype2/libfreetype-Module
  cp Contents/module.modulemap build/$platform/$arch/install/include/freetype2/libfreetype-Module/module.modulemap
  exit_if_error
}


# Build for Apple systems
build_library MacOSX           arm64  macos11
build_library MacOSX           x86_64 macos10.13
build_library iPhoneOS         arm64  ios12
build_library iPhoneSimulator  arm64  ios14-simulator
build_library iPhoneSimulator  x86_64 ios12-simulator
build_library AppleTVOS        arm64  tvos12
build_library AppleTVSimulator arm64  tvos12-simulator
build_library AppleTVSimulator x86_64 tvos12-simulator
build_library WatchOS          arm64  watchos8
build_library WatchSimulator   arm64  watchos8-simulator
build_library WatchSimulator   x86_64 watchos8-simulator
build_library XROS             arm64  xros1
build_library XRSimulator      arm64  xros1-simulator
build_library XRSimulator      x86_64 xros1-simulator

# Build for Android
build_library Android aarch64 21
build_library Android arm     21
build_library Android i686    21
build_library Android riscv64 35
build_library Android x86_64  21


# Xcode Framework for all Apple platforms
create_xcframework() {
  # Remove previously created framework if exists
  rm -rf build/libfreetype.xcframework
  exit_if_error

  # Merge macOS arm and x86 binaries
  mkdir -p build/MacOSX
  exit_if_error
  lipo -create -output build/MacOSX/libfreetype.a \
    build/MacOSX/arm64/install/lib/libfreetype.a \
    build/MacOSX/x86_64/install/lib/libfreetype.a
  exit_if_error

  # Merge iOS simulator arm and x86 binaries
  mkdir -p build/iPhoneSimulator
  exit_if_error
  lipo -create -output build/iPhoneSimulator/libfreetype.a \
    build/iPhoneSimulator/arm64/install/lib/libfreetype.a \
    build/iPhoneSimulator/x86_64/install/lib/libfreetype.a
  exit_if_error

  # Merge tvOS simulator arm and x86 binaries
  mkdir -p build/AppleTVSimulator
  exit_if_error
  lipo -create -output build/AppleTVSimulator/libfreetype.a \
    build/AppleTVSimulator/arm64/install/lib/libfreetype.a \
    build/AppleTVSimulator/x86_64/install/lib/libfreetype.a
  exit_if_error

  # Merge watchOS simulator arm and x86 binaries
  mkdir -p build/WatchSimulator
  exit_if_error
  lipo -create -output build/WatchSimulator/libfreetype.a \
    build/WatchSimulator/arm64/install/lib/libfreetype.a \
    build/WatchSimulator/x86_64/install/lib/libfreetype.a
  exit_if_error

  # Merge visionOS simulator arm and x86 binaries
  mkdir -p build/XRSimulator
  exit_if_error
  lipo -create -output build/XRSimulator/libfreetype.a \
    build/XRSimulator/arm64/install/lib/libfreetype.a \
    build/XRSimulator/x86_64/install/lib/libfreetype.a
  exit_if_error

  # Create the framework with multiple platforms
  xcodebuild -create-xcframework \
    -library build/MacOSX/libfreetype.a                      -headers build/MacOSX/arm64/install/include/freetype2 \
    -library build/iPhoneOS/arm64/install/lib/libfreetype.a  -headers build/iPhoneOS/arm64/install/include/freetype2 \
    -library build/iPhoneSimulator/libfreetype.a             -headers build/iPhoneSimulator/arm64/install/include/freetype2 \
    -library build/AppleTVOS/arm64/install/lib/libfreetype.a -headers build/AppleTVOS/arm64/install/include/freetype2 \
    -library build/AppleTVSimulator/libfreetype.a            -headers build/AppleTVSimulator/arm64/install/include/freetype2 \
    -library build/WatchOS/arm64/install/lib/libfreetype.a   -headers build/WatchOS/arm64/install/include/freetype2 \
    -library build/WatchSimulator/libfreetype.a              -headers build/WatchSimulator/arm64/install/include/freetype2 \
    -library build/XROS/arm64/install/lib/libfreetype.a      -headers build/XROS/arm64/install/include/freetype2 \
    -library build/XRSimulator/libfreetype.a                 -headers build/XRSimulator/arm64/install/include/freetype2 \
    -output build/libfreetype.xcframework
  exit_if_error

  # And sign the framework
  codesign --timestamp -s $signing_identity build/libfreetype.xcframework
  exit_if_error
}
create_xcframework


# Artifact bundle for Android
create_artifactbundle() {
  # Remove previously created artifact if exists
  rm -rf build/libfreetype.artifactbundle
  exit_if_error

  # Create the artifact bundle folder
  mkdir -p build/libfreetype.artifactbundle
  exit_if_error

  # info.json
  cp Contents/info.json build/libfreetype.artifactbundle/info.json
  exit_if_error

  # Headers
  cp -r build/Android/aarch64/install/include build/libfreetype.artifactbundle/include
  exit_if_error

  # aarch64-linux-android
  mkdir -p build/libfreetype.artifactbundle/aarch64-linux-android
  exit_if_error
  cp build/Android/aarch64/install/lib/libfreetype.a build/libfreetype.artifactbundle/aarch64-linux-android/libfreetype.a
  exit_if_error

  # arm-linux-androideabi
  mkdir -p build/libfreetype.artifactbundle/arm-linux-androideabi
  exit_if_error
  cp build/Android/arm/install/lib/libfreetype.a build/libfreetype.artifactbundle/arm-linux-androideabi/libfreetype.a
  exit_if_error

  # i686-linux-android
  mkdir -p build/libfreetype.artifactbundle/i686-linux-android
  exit_if_error
  cp build/Android/i686/install/lib/libfreetype.a build/libfreetype.artifactbundle/i686-linux-android/libfreetype.a
  exit_if_error

  # riscv64-linux-android
  mkdir -p build/libfreetype.artifactbundle/riscv64-linux-android
  exit_if_error
  cp build/Android/riscv64/install/lib/libfreetype.a build/libfreetype.artifactbundle/riscv64-linux-android/libfreetype.a
  exit_if_error

  # x86_64-linux-android
  mkdir -p build/libfreetype.artifactbundle/x86_64-linux-android
  exit_if_error
  cp build/Android/x86_64/install/lib/libfreetype.a build/libfreetype.artifactbundle/x86_64-linux-android/libfreetype.a
  exit_if_error
}
create_artifactbundle




# fin
