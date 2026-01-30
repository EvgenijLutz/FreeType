// swift-tools-version: 6.2
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let dependencies: [Package.Dependency] = {
#if true
    [
        // freetype uses libpbg to load some fonts that contain png glyphs
        .package(url: "https://github.com/EvgenijLutz/LibPNG.git", from: .init(1, 6, 50)),
    ]
#else
    [
        .package(name: "LibPNG", path: "../LibPNG"),
    ]
#endif
}()


let libFreeTypeArtifactTarget: Target = {
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
}()


let package = Package(
    name: "FreeType",
    platforms: [
        .macOS(.v14),
        .iOS(.v17),
        .tvOS(.v17),
        .watchOS(.v10),
        .visionOS(.v1),
        .custom("Android", versionString: "21")
    ],
    products: [
        .library(
            name: "libfreetype",
            targets: ["libfreetype"]
        ),
        .library(
            name: "msdfgen",
            targets: ["msdfgen"]
        ),
        .library(
            name: "FreeTypeC",
            targets: ["FreeTypeC"]
        ),
        .library(
            name: "FreeType",
            targets: ["FreeType"]
        ),
    ],
    dependencies: dependencies,
    targets: [
        libFreeTypeArtifactTarget,
        .target(
            name: "FreeTypeC",
            dependencies: [
                .target(name: "libfreetype"),
                // Make sure to include libpbg to avoid linking errors because libfreetype needs it
                .product(name: "LibPNGC", package: "LibPNG")
            ],
            cxxSettings: [
                .enableWarning("all")
            ]
        ),
        .target(
            name: "msdfgen",
            dependencies: [
                .target(name: "FreeTypeC"),
            ],
            cSettings: [
                //.define("MSDFGEN_PUBLIC", to: " /* none */ ")
            ],
            cxxSettings: [
                .enableWarning("all"),
                //.define("MSDFGEN_PUBLIC", to: " /* wtf why are you not working */ ")
            ]
        ),
        .target(
            name: "FreeType",
            dependencies: [
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
