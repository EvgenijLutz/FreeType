# FreeType

Carefuly compiled with love [FreeType](https://freetype.org/index.html) library as an Xcode Framework and distributed as a Swift package so you can conviniently integrate it in your Xcode project.

- Use Swift, C or C++ to access the FreeType API
- Built using `gnu make` and `c17` with `-O2` optimisation level
- Available for all Apple platforms including simulators and both arm64 and x86 (when applicable) architectures
- Supports color bitmap glyph formats in the PNG format using [libpng](https://github.com/pnggroup/libpng) (`png=enabled` feature), which is already prebuilt in the [LibPNG](https://github.com/EvgenijLutz/LibPNG) and linked to this package.

As a temporary feature, the FreeType package also includes [msdfgen](https://github.com/Chlumsky/msdfgen) to generate SDF textures. It will be likely moved to a separate package. 


## Installing FreeType

Add the following dependency to your Package.swift:

```Swift
.package(url: "https://github.com/EvgenijLutz/FreeType.git", from: "2.14.1-alpha1")
```

And you're good to go!


## Framework contents

There are several products in the `FreeType`:


### libfreetype - original freetype interface

Precompiled as an XCFramework `FreeType` library without any extensions. After adding the package to your project, you can import the library in `Swift`:
```Swift
import libfreetype
```

or in `C`/`C++`:
```C
#include <libfreetype.h>
```


### FreeTypeC - best for C/C++

A `C++` library that extends `FreeType`'s interface, links the `FreeType` target, empty at the moment. You can import the library in `Swift`:
```Swift
import FreeTypeC
```

or in `C`/`C++`:
```C
#include <FreeTypeC.h>
```


### FreeType - best for Swift

A `Swift` library that extends `FreeTypeC`'s interface, links the `FreeTypeC` target, empty at the moment. You can import the library in `Swift`:

```Swift
import FreeType
```


## TODOs

- Expose Android artifact bundle. The library is successfully compiled, but not yet tested.
- Support Windows
- Support Linux


# The journey of compiling FreeType
Currently used `freetype 2.14.1` and `Xcode 26.2`. Download the source code [here](https://download.savannah.gnu.org/releases/freetype/) and unpack the archive. From the FreeType package, copy all files from the `Resources/Build` directory into the downloaded FreeType sources. Using terminal, navigate to the FreeType sources directory:
```bash
cd PATH_TO_FREETYPE_SOURCES
```

Clone somewhere the `LibPNG` to use it for building `FreeType` with `libpng` support.
```bash
git clone https://github.com/EvgenijLutz/LibPNG.git
```

In the `build-apple.sh` script, modify the `libpng_framework_path` (path to `LibPNG.xcframework` in the previously downloaded `LibPNG` repo), `platforms_path` (path to the `Contents/Developer/Platforms` in your `Xcode` app) and `signing_identity` (your Xcode signing identity to sign the framework you compile, see the comment for this variable in the `build-apple.sh` script for instructions) variables. And execute the script:
```bash
bash build-apple.sh
```

If everything succeeds, you'll get `libfreetype.xcframework` in the `build` folder. Voilà!
