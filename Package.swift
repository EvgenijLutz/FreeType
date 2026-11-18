// swift-tools-version: 6.4
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription


let package = Package(
    name: "FreeType",
    platforms: [
        .macOS(.v26),
        .iOS(.v26),
        .tvOS(.v26),
        .watchOS(.v26),
        .visionOS(.v26),
        .custom("Android", versionString: "5.0")
    ],
    products: [
        // Original FreeType
        .library(
            name: "libfreetype",
            targets: ["libfreetype"]
        ),
        // Original FreeType available for Swift
        .library(
            name: "libfreetype_c",
            targets: ["libfreetype_c"]
        ),
        // Generate SDF fonts
        .library(
            name: "msdfgen",
            targets: ["msdfgen"]
        ),
        // C++ helper interfaces for FreeType
        .library(
            name: "FreeTypeC",
            targets: ["FreeTypeC"]
        ),
        // Swift helper interfaces for FreeType
        .library(
            name: "FreeType",
            targets: ["FreeType"]
        ),
    ],
    dependencies: {
#if true
        [
            // freetype uses libpbg to load some fonts that contain png glyphs
            .package(url: "https://github.com/EvgenijLutz/LibPNG.git", from: "1.6.58"),
            .package(url: "https://github.com/EvgenijLutz/Brotli.git", from: "1.2.0"),
        ]
#else
        [
            .package(name: "LibPNG", path: "../LibPNG"),
            .package(name: "Brotli", path: "../Brotli"),
        ]
#endif
    }(),
    targets: [
        {
#if os(macOS) || os(iOS) || os(tvOS) || os(watchOS) || os(visionOS)
            .binaryTarget(
                name: "libfreetype",
                path: "Binaries/libfreetype.xcframework"
            )
#else
            .binaryTarget(
                name: "libfreetype",
                path: "Binaries/libfreetype.artifactbundle"
            )
#endif
        }(),
        .target(
            name: "libfreetype_c",
            dependencies: [
                .target(name: "libfreetype"),
            ],
            linkerSettings: [
                // Link Apple's [hvf](https://developer.apple.com/documentation/hvf) framework.
                .linkedLibrary("hvf", .when(platforms: [.macOS, .iOS, .tvOS, .watchOS, .visionOS]))
            ]
        ),
        .target(
            name: "FreeTypeC",
            dependencies: [
                .target(name: "libfreetype"),
                // Make sure to include libpbg to avoid linking errors because libfreetype needs it
                .product(name: "LibPNGC", package: "LibPNG"),
                .product(name: "libbrotlicommon", package: "Brotli"),
                .product(name: "libbrotlidec", package: "Brotli"),
            ],
            cSettings: [
                .enableWarning("all"),
                .enableWarning("extra"),
                .enableWarning("implicit-fallthrough"),
                .enableWarning("int-conversion"),
                .enableWarning("pointer-to-int-cast")
            ],
            cxxSettings: [
                .enableWarning("all"),
                .enableWarning("extra"),
                .enableWarning("implicit-fallthrough"),
                .enableWarning("int-conversion"),
                .enableWarning("pointer-to-int-cast")
            ],
            linkerSettings: [
                // Link Apple's [hvf](https://developer.apple.com/documentation/hvf) framework.
                .linkedLibrary("hvf", .when(platforms: [.macOS, .iOS, .tvOS, .watchOS, .visionOS]))
            ]
        ),
        .target(
            name: "msdfgen",
            dependencies: [
                .target(name: "FreeTypeC"),
            ],
            cSettings: [
                .enableWarning("all"),
                //.enableWarning("extra"),
                //.enableWarning("implicit-fallthrough"),
                //.enableWarning("int-conversion"),
                //.enableWarning("pointer-to-int-cast"),
                //.define("MSDFGEN_PUBLIC", to: " /* none */ ")
            ],
            cxxSettings: [
                .enableWarning("all"),
                //.enableWarning("extra"),
                //.enableWarning("implicit-fallthrough"),
                //.enableWarning("int-conversion"),
                //.enableWarning("pointer-to-int-cast"),
                //.define("MSDFGEN_PUBLIC", to: " /* none */ ")
            ]
        ),
        .target(
            name: "FreeType",
            dependencies: [
                .target(name: "libfreetype_c"),
                .target(name: "FreeTypeC"),
                .target(name: "msdfgen")
            ],
            swiftSettings: [
                .interoperabilityMode(.Cxx)
            ]
        )
    ],
    // The libpng library was compiled using c17, so set it also here
    cLanguageStandard: .c17,
    // Also use c++20, we don't live in the stone age, but still not ready to accept c++23
    cxxLanguageStandard: .cxx20
)
