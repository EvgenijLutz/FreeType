# FreeTypeFramework

Carefuly compiled with love [FreeType](https://freetype.org/index.html) library as an Xcode Framework and distributed as a Swift package so you can conviniently integrate it in your Xcode project and use in Swift or C.


# Installing FreeTypeFramework

Add the following dependency to your Package.swift:

```Swift
.package(url: "https://github.com/EvgenijLutz/FreeTypeFramework.git", from: "1.0.0")
```

And you're good to go!


# Using FreeTypeFramework

After adding the package to your project, import the library in Swift:
```Swift
import FreeTypeFramework
```

or in C:
```C
#include <libfreetype.h>
```

And you're good to go!


# The journey of compiling FreeType
Currently used version 2.13.3 and Xcode 16.4
```bash
git clone https://gitlab.freedesktop.org/freetype/freetype.git
```

## Docwriter

If you don't have `docwriter` installed yet, you'll much likely get this warning after finishing the Step4:
```plain
'make refdoc' will fail since pip package 'docwriter' is not installed.
  To install, run 'python3 -m pip install docwriter', or to use a Python
  virtual environment, run 'make refdoc-venv' (requires pip package
  'virtualenv').  These operations require Python >= 3.5.
```

To avoid this, install the `docwriter`:
```bash
python3 -m pip install docwriter
```

## Step 1
First, you need to get your signing identity:
```bash
security find-identity -v -p codesigning
```

Choose one of them in the list and execute the following command to define your signing identity and path to Xcode SDKs for later use:
```bash
export ft_developer="/Applications/Xcode.app/Contents/Developer"
export ft_sing_identity=YOUR_SIGN_IDENTITY
```


## Step 2
Set `ft_platform` to MacOSX, iPhoneOS, iPhoneSimulator, XROS, XRSimulator, AppleTVOS, AppleTVSimulator, WatchOS, WatchSimulator. Note that arm64 simulator is only available from iOS 14, tvOS14.
```bash
# Set target platform
export ft_platform="WatchSimulator"
export ft_sysroot="/Applications/Xcode.app/Contents/Developer/Platforms/$ft_platform.platform/Developer/SDKs/$ft_platform.sdk"

# Specify minimum OS version depending on OS
if [[ "$ft_platform" == "MacOSX" ]]; then
     export ft_min_os="-mtargetos=macos11"
elif [[ "$ft_platform" == "iPhoneOS" ]]; then
     export ft_min_os="-mtargetos=ios12"
elif [[ "$ft_platform" == "iPhoneSimulator" ]]; then
     export ft_min_os="-mtargetos=ios14-simulator"
elif [[ "$ft_platform" == "XROS" ]]; then
     export ft_min_os="-mtargetos=xros1"
elif [[ "$ft_platform" == "XRSimulator" ]]; then
     export ft_min_os="-mtargetos=xros1-simulator"
elif [[ "$ft_platform" == "AppleTVOS" ]]; then
     export ft_min_os="-mtargetos=tvos12"
elif [[ "$ft_platform" == "AppleTVSimulator" ]]; then
     export ft_min_os="-mtargetos=tvos12-simulator"
elif [[ "$ft_platform" == "WatchOS" ]]; then
     export ft_min_os="-mtargetos=watchos8"
elif [[ "$ft_platform" == "WatchSimulator" ]]; then
     export ft_min_os="-mtargetos=watchos8-simulator"
else
     export ft_min_os=""
fi
echo "$ft_min_os"
```


## Step 3
```bash
# For every OS:
export ft_arch="arm64"
export ft_host="arm-apple-darwin"

# Additionally to MacOSX, iPhoneSimulator, XRSimulator, AppleTVSimulator and WatchSimulator:
export ft_arch="x86_64"
export ft_host="x86_64-apple-darwin"
```


## Setp 4
Build library with `C 17`. Don't forget to replace path to your downloaded freetype library in the following command:
```bash
make clean

./configure --prefix=/Users/evgenij/Developer/Sources/freetype-2.13.3/install/$ft_platform/$ft_arch \
--host=$ft_host \
--enable-static=yes \
--enable-shared=no \
CC="$ft_developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang -isysroot $ft_sysroot" \
CFLAGS="-arch $ft_arch -std=c17 $ft_min_os -I$ft_sysroot/usr/include/libxml2" \
LDFLAGS="-arch $ft_arch"
```


## Step 5
```bash
make
make install
```


## Optional - check supported OS version
Go to the compiled library folder (somwhere in the install folder) and execute the following command:
```bash
otool -l libfreetype.a | grep -A 1 minos
```


## Setp 6 - only for universal libraries that should support both arm64 and x86
Create a universal library for MacOSX, iPhoneSimulator, XRSimulator, AppleTVSimulator and WatchSimulator targets:
```bash
lipo -create -output install/$ft_platform/libfreetype.a install/$ft_platform/arm64/lib/libfreetype.a install/$ft_platform/x86_64/lib/libfreetype.a
#lipo -create -output install/$ft_platform/libfreetype.dylib install/$ft_platform/arm64/lib/libfreetype.dylib install/$ft_platform/x86_64/lib/libfreetype.dylib
```


## Step 7

FreeTypeFramework uses slighty modified headers (located at Resources/Headers) to make it possible to use in Swift and include as a single header in C.

```bash
# Use generated headers
#xcodebuild -create-xcframework \
#     -library install/MacOSX/libfreetype.a -headers install/MacOSX/arm64/include/freetype2 \
#     -library install/iPhoneOS/arm64/lib/libfreetype.a -headers install/iPhoneOS/arm64/include/freetype2 \
#     -library install/iPhoneSimulator/libfreetype.a -headers install/iPhoneSimulator/arm64/include/freetype2 \
#     -library install/XROS/arm64/lib/libfreetype.a -headers install/XROS/arm64/include/freetype2 \
#     -library install/XRSimulator/libfreetype.a -headers install/XRSimulator/arm64/include/freetype2 \
#     -library install/AppleTVOS/arm64/lib/libfreetype.a -headers install/AppleTVOS/arm64/include/freetype2 \
#     -library install/AppleTVSimulator/arm64/lib/libfreetype.a -headers install/AppleTVSimulator/arm64/include/freetype2 \
#     -library install/WatchOS/arm64/lib/libfreetype.a -headers install/WatchOS/arm64/include/freetype2 \
#     -library install/WatchSimulator/arm64/lib/libfreetype.a -headers install/WatchSimulator/arm64/include/freetype2 \
#     -output install/libfreetype.xcframework

# Use custom headers
xcodebuild -create-xcframework \
     -library install/MacOSX/libfreetype.a -headers install/Headers \
     -library install/iPhoneOS/arm64/lib/libfreetype.a -headers install/Headers \
     -library install/iPhoneSimulator/libfreetype.a -headers install/Headers \
     -library install/XROS/arm64/lib/libfreetype.a -headers install/Headers \
     -library install/XRSimulator/libfreetype.a -headers install/Headers \
     -library install/AppleTVOS/arm64/lib/libfreetype.a -headers install/Headers \
     -library install/AppleTVSimulator/arm64/lib/libfreetype.a -headers install/Headers \
     -library install/WatchOS/arm64/lib/libfreetype.a -headers install/Headers \
     -library install/WatchSimulator/arm64/lib/libfreetype.a -headers install/Headers \
     -output install/libfreetype.xcframework

codesign --timestamp -s $ft_sing_identity install/libfreetype.xcframework
```