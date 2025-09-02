# FreeTypeFramework

Carefuly compiled with love [FreeType](https://freetype.org/index.html) library as an Xcode Framework and distributed as a Swift package so you can conviniently integrate it in your Xcode project.

- Use Swift, C or C++ to access the FreeType API
- Built using `meson` and `c17` with `-O2` optimisation level
- Available for all Apple platforms including simulators and both arm64 and x86 (when applicable) architectures
- Supports color bitmap glyph formats in the PNG format using [libpng](https://github.com/pnggroup/libpng) (`png=enabled` feature), which is already prebuilt in the [LibPNGFramework](https://github.com/EvgenijLutz/LibPNGFramework) and linked to this package.


## Installing FreeTypeFramework

Add the following dependency to your Package.swift:

```Swift
.package(url: "https://github.com/EvgenijLutz/FreeTypeFramework.git", from: "2.13.3-alpha4")
```

Since `libpng` and `FreeType` rely on libz that ships with OS, you will likely need to link the following libraries in your Xcode project (already included in Xcode):
```Plain
libz.tbd
libbz2.1.0.tbd
```

And you're good to go!


## Framework contents

There are several products in the `FreeTypeFramework`:


### FreeType - original freetype interface

Precompiled as an XCFramework `FreeType` library without any extensions. After adding the package to your project, you can import the library in `Swift`:
```Swift
import FreeType
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


### FreeTypeFramework - best for Swift

A `Swift` library that extends `FreeTypeC`'s interface, links the `FreeTypeC` target, empty at the moment. You can import the library in `Swift`:

```Swift
import FreeTypeFramework
```


## TODOs

- Support Windows
- Support Android
- Support Linux


# The journey of compiling FreeType
Currently used `freetype 2.13.3` and `Xcode 16.4`, you also need to have `meson` and `ninja` installed, usually via `homebrew`. Download the source code [here](https://download.savannah.gnu.org/releases/freetype/). From the FreeTypeFramework package, copy all files from the `Resources/Build` directory into the downloaded FreeType sources. Using terminal, navigate to the FreeType sources directory:
```bash
cd PATH_TO_FREETYPE_SOURCES
```

Clone somewhere the `LibPNGFramework` to use it for building `FreeType` with `libpng` support.
```bash
git clone https://github.com/EvgenijLutz/LibPNGFramework.git
```

In the `build-apple-meson.sh` script, modify the `libpng_framework_path` (path to `LibPNGFramework.xcframework` in the previously downloaded `LibPNGFramework` repo), `platforms_path` (path to the `Contents/Developer/Platforms` in your `Xcode` app) and `signing_identity` (your Xcode signing identity to sign the framework you compile, see the comment for this variable in the `build-apple-meson.sh` script for instructions) variables. And execute the script:
```bash
bash build-apple-meson.sh
```

If everything succeeds, you'll get `FreeTypeFramework.xcframework` in the `build-apple` folder. Voilà!
