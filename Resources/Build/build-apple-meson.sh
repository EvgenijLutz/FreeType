# bash

# Get some help
# meson configure >> help-meson.txt


# Define some global variables
libpng_framework_path='/Users/evgenij/Developer/Xcode projects/LibPNGFramework/LibPNG.xcframework'
platforms_path='/Applications/Xcode.app/Contents/Developer/Platforms'
# Your signing identity to sign the xcframework. Execute "security find-identity -v -p codesigning" and select one from the list
signing_identity=YOUR_SIGNING_IDENTITY


# Console output formatting
# https://stackoverflow.com/a/2924755
bold=$(tput bold)
normal=$(tput sgr0)


# Create the build directory if not exists
mkdir -p build-apple


exit_if_error() {
  local result=$?
  if [ $result -ne 0 ] ; then
     echo "Received an exit code $result, aborting"
     exit 1
  fi
}


build_library() {
  local platform_name=$1
  local arch=$2
  local target_os=$3

  local config_name="$platform_name/$arch"

  if [[ "$arch" == "arm64" ]]; then
    local meson_cpu="arm64"
    local meson_cpu_family="aarch64"
  elif [[ "$arch" == "x86_64" ]]; then
    local meson_cpu="x86"
    local meson_cpu_family="x86_64"
  else
    echo "Unknown architecture"
    exit 1
  fi

  echo "Build for ${bold}$config_name${normal}"
  mkdir -p build-apple/$config_name

  # Remove config file if exists
  rm -f "build-apple/$config_name.txt"
  exit_if_error

  # Generate meson config file for cross-compile
  mkdir -p build-apple/$config_name/pkg-config
  rm -f   "build-apple/$config_name/pkg-config/libpng.pc"
  echo \
"[binaries]
c = 'clang'
cpp = 'clang++'
ar = 'ar'
strip = 'strip'
pkg-config = 'pkg-config'

[host_machine]
system = 'darwin'
cpu_family = '$meson_cpu_family'
cpu = '$meson_cpu'
endian = 'little'" \
>> "build-apple/$config_name.txt"
  exit_if_error

  # Create custom pkg-config file for libpng to target specific platform and architecture
  mkdir -p build-apple/$config_name/
  if [[ "$platform_name" == "MacOSX" ]]; then
    local libpng_target="macos-arm64_x86_64"
  elif [[ "$platform_name" == "iPhoneOS" ]]; then
    local libpng_target="ios-$arch"
  elif [[ "$platform_name" == "iPhoneSimulator" ]]; then
    local libpng_target="ios-arm64_x86_64-simulator"
  elif [[ "$platform_name" == "AppleTVOS" ]]; then
    local libpng_target="tvos-$arch"
  elif [[ "$platform_name" == "AppleTVSimulator" ]]; then
    local libpng_target="tvos-arm64_x86_64-simulator"
  elif [[ "$platform_name" == "WatchOS" ]]; then
    local libpng_target="watchos-$arch"
  elif [[ "$platform_name" == "WatchSimulator" ]]; then
    local libpng_target="watchos-arm64_x86_64-simulator"
  elif [[ "$platform_name" == "XROS" ]]; then
    local libpng_target="xros-$arch"
  elif [[ "$platform_name" == "XRSimulator" ]]; then
    local libpng_target="xros-arm64_x86_64-simulator"
  else
    echo "Unknown platform $platform_name"
    exit 1
  fi
  local libpng_path="$libpng_framework_path/$libpng_target"
  echo \
"prefix=\"$libpng_path\"
exec_prefix=\${prefix}
libdir=\${exec_prefix}
includedir=\${prefix}/Headers 

Name: libpng
Description: Loads and saves PNG files
Version: 1.6.50
Requires.private: zlib
Libs: -L\${libdir} -llibpng16
Libs.private: -lz
Cflags: -I\"\${includedir}\"
" \
>> "build-apple/$config_name/pkg-config/libpng.pc"
  exit_if_error

  # Setup meson build
  meson setup build-apple/$config_name \
    --cross-file build-apple/$config_name.txt \
    -Dprefix=$(pwd)/build-apple/$config_name/install \
    -Dauto_features=disabled \
    -Dbuildtype=minsize \
    -Ddebug=false \
    -Ddefault_library=static \
    -Dlayout=mirror \
    -Doptimization=2 \
    -Dprefer_static=false \
    -Dstrip=true \
    -Dpkg_config_path="$(pwd)/build-apple/$config_name/pkg-config" \
    -Dc_args="-isysroot '$platforms_path/$platform_name.platform/Developer/SDKs/$platform_name.sdk' -arch $arch -mtargetos=$target_os" \
    -Dc_link_args="-arch $arch -mtargetos=$target_os" \
    -Dc_std=c17 \
    -Dbrotli=disabled \
    -Dbzip2=disabled \
    -Dharfbuzz=disabled \
    -Dmmap=enabled \
    -Dpng=enabled \
    -Dtests=disabled \
    -Dzlib=system
  exit_if_error

  # Build
  echo "Build"
  ninja -C build-apple/$config_name
  exit_if_error
  
  # Install compiled libraries and headers into the install folder
  ninja -C build-apple/$config_name install
  exit_if_error

  # Copy custom umbrella header
  cp libfreetype.h build-apple/$config_name/install/include/freetype2/libfreetype.h
  exit_if_error

  # About modules
  # https://clang.llvm.org/docs/Modules.html
  # Without module.modulemap FreeType is not exposed to Swift
  # Copy the module map into the directory with installed header files
  mkdir -p build-apple/$config_name/install/include/freetype2/FreeType-Module
  cp module.modulemap build-apple/$config_name/install/include/freetype2/FreeType-Module/module.modulemap
  exit_if_error
}


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


create_framework() {
  # Remove previously created framework if exists
  rm -rf build-apple/FreeType.xcframework
  exit_if_error

  # Merge macOS arm and x86 binaries
  mkdir -p build-apple/MacOSX
  exit_if_error
  lipo -create -output build-apple/MacOSX/libfreetype.a \
    build-apple/MacOSX/arm64/install/lib/libfreetype.a \
    build-apple/MacOSX/x86_64/install/lib/libfreetype.a
  exit_if_error

  # Merge iOS simulator arm and x86 binaries
  mkdir -p build-apple/iPhoneSimulator
  exit_if_error
  lipo -create -output build-apple/iPhoneSimulator/libfreetype.a \
    build-apple/iPhoneSimulator/arm64/install/lib/libfreetype.a \
    build-apple/iPhoneSimulator/x86_64/install/lib/libfreetype.a
  exit_if_error

  # Merge tvOS simulator arm and x86 binaries
  mkdir -p build-apple/AppleTVSimulator
  exit_if_error
  lipo -create -output build-apple/AppleTVSimulator/libfreetype.a \
    build-apple/AppleTVSimulator/arm64/install/lib/libfreetype.a \
    build-apple/AppleTVSimulator/x86_64/install/lib/libfreetype.a
  exit_if_error

  # Merge watchOS simulator arm and x86 binaries
  mkdir -p build-apple/WatchSimulator
  exit_if_error
  lipo -create -output build-apple/WatchSimulator/libfreetype.a \
    build-apple/WatchSimulator/arm64/install/lib/libfreetype.a \
    build-apple/WatchSimulator/x86_64/install/lib/libfreetype.a
  exit_if_error

  # Merge visionOS simulator arm and x86 binaries
  mkdir -p build-apple/XRSimulator
  exit_if_error
  lipo -create -output build-apple/XRSimulator/libfreetype.a \
    build-apple/XRSimulator/arm64/install/lib/libfreetype.a \
    build-apple/XRSimulator/x86_64/install/lib/libfreetype.a
  exit_if_error

  # Create the framework with multiple platforms
  xcodebuild -create-xcframework \
    -library build-apple/MacOSX/libfreetype.a                      -headers build-apple/MacOSX/arm64/install/include/freetype2 \
    -library build-apple/iPhoneOS/arm64/install/lib/libfreetype.a  -headers build-apple/iPhoneOS/arm64/install/include/freetype2 \
    -library build-apple/iPhoneSimulator/libfreetype.a             -headers build-apple/iPhoneSimulator/arm64/install/include/freetype2 \
    -library build-apple/AppleTVOS/arm64/install/lib/libfreetype.a -headers build-apple/AppleTVOS/arm64/install/include/freetype2 \
    -library build-apple/AppleTVSimulator/libfreetype.a            -headers build-apple/AppleTVSimulator/arm64/install/include/freetype2 \
    -library build-apple/WatchOS/arm64/install/lib/libfreetype.a   -headers build-apple/WatchOS/arm64/install/include/freetype2 \
    -library build-apple/WatchSimulator/libfreetype.a              -headers build-apple/WatchSimulator/arm64/install/include/freetype2 \
    -library build-apple/XROS/arm64/install/lib/libfreetype.a      -headers build-apple/XROS/arm64/install/include/freetype2 \
    -library build-apple/XRSimulator/libfreetype.a                 -headers build-apple/XRSimulator/arm64/install/include/freetype2 \
    -output build-apple/FreeType.xcframework
  exit_if_error

  # And sign the framework
  codesign --timestamp -s $signing_identity build-apple/FreeType.xcframework
  exit_if_error
}
create_framework




# fin