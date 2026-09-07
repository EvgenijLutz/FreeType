# FreeType

[![](https://img.shields.io/endpoint?url=https%3A%2F%2Fswiftpackageindex.com%2Fapi%2Fpackages%2FEvgenijLutz%2FFreeType%2Fbadge%3Ftype%3Dswift-versions)](https://swiftpackageindex.com/EvgenijLutz/FreeType) [![](https://img.shields.io/endpoint?url=https%3A%2F%2Fswiftpackageindex.com%2Fapi%2Fpackages%2FEvgenijLutz%2FFreeType%2Fbadge%3Ftype%3Dplatforms)](https://swiftpackageindex.com/EvgenijLutz/FreeType) [![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

Carefuly compiled with love [FreeType](https://freetype.org/index.html) library as an Xcode Framework and distributed as a Swift package so you can conviniently integrate it in your Xcode project.

- Use Swift, C or C++ to access the FreeType API
- Built using `gnu make` and `c17` with `-O2` optimisation level
- Available for all Apple platforms including simulators and both arm64 and x86 (when applicable) architectures
- Supports color bitmap glyph formats in the PNG format using [libpng](https://github.com/pnggroup/libpng) (`png=enabled` feature), which is already prebuilt in the [LibPNG](https://github.com/EvgenijLutz/LibPNG) and linked to this package.

As a temporary feature, the FreeType package also includes [msdfgen](https://github.com/Chlumsky/msdfgen) to generate SDF textures. It will be likely moved to a separate package. 


## Install

Add the following dependency to your Package.swift:

```Swift
.package(url: "https://github.com/EvgenijLutz/FreeType.git", from: "2.14.3")
```

And you're good to go!


## Framework contents

There are several products in the `FreeType`:


### libfreetype.xcframework - original freetype interface

Precompiled as an XCFramework `FreeType` library without any extensions. After adding the package to your project, you can import the library in in `C`/`C++`:
```C
#include <ft2build.h>
#include <freetype/freetype.h>
```

In the previous releases, libfreetype.xcframework contained an umbrella header and a clang module map file to directly expose the original FreeType APIs to Swift. After Xcode 26.3 release, module map discovery was broken (or became less tolerant to arbitrary modulemap placement in the XCFramework bundle) for static XCFrameworks with custom module maps. Since then, the Swift compiler could not generate Swift interface and thus, you cannot import the module directly. We will probably give up on embedding clang modulemaps into static XCFrameworks and provide a separate target that exposes its interface instead.


### FreeTypeC - best for C++

A `C++` library that extends `FreeType`'s interface, links the `FreeType` target, empty at the moment. You can import the library in `C++`:
```cpp
#include <FreeTypeC/FreeTypeC.h>
```

or in `Swift`:
```Swift
import FreeTypeC
```


### FreeType - best for Swift

A `Swift` library that extends `FreeTypeC`'s interface, links the `FreeTypeC` target, empty at the moment. You can import the library in `Swift`:

```Swift
import FreeType
```


## TODOs

- Expose Android artifact bundle. The library is successfully compiled, but not yet tested on a real device.
- Support Windows
- Support Linux


# Building FreeType
Download the source code [here](https://download.savannah.gnu.org/releases/freetype/) and unpack the archive. From the FreeType package, copy all files from the `Resources/Build` directory into the downloaded FreeType sources. Using terminal, navigate to the FreeType sources directory:
```bash
cd PATH_TO_FREETYPE_SOURCES
```

Clone `LibPNG` and `Brotli` to use it for building `FreeType` with `libpng` and `brotli` support.
```bash
git clone https://github.com/EvgenijLutz/LibPNG.git
git clone https://github.com/EvgenijLutz/Brotli.git
```

In the `build-apple.sh` script, modify the following variables:
- `libpng_artifact_path` - path to `LibPNG.xcframework` in the previously downloaded `LibPNG` repo,
- `brotlicommon_artifact_path` - paths to `brotlicommon.xcframework` in the previously downloaded `Brotli` repo,
- `brotlienc_artifact_path` - `brotlienc.xcframework`,
- `brotlidec_artifact_path` - `brotlidec.xcframework`,
- `platforms_path` - path to the `Contents/Developer/Platforms` in your `Xcode` app,
- `signing_identity` - your Xcode signing identity to sign the framework you compile, see the comment for this variable in the `build-apple.sh` script for instructions.
And execute the script:
```bash
bash build-apple.sh
```

If everything succeeds, you'll get `libfreetype.xcframework` in the `build` folder. Voilà!
