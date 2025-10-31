// swift-tools-version: 6.2
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "FreeType",
    platforms: [
        .macOS(.v14),
        .iOS(.v17),
        .tvOS(.v17),
        .watchOS(.v10),
        .visionOS(.v1)
    ],
    products: [
        .library(
            name: "libfreetype",
            targets: ["libfreetype"]
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
    dependencies: [
        //.package(url: "https://github.com/EvgenijLutz/LibPNG.git", exact: "1.6.50-alpha3"),
        .package(url: "https://github.com/EvgenijLutz/LibPNG.git", branch: "main")
    ],
    targets: [
        .binaryTarget(
            name: "libfreetype",
            path: "Binaries/libfreetype.xcframework"
        ),
        .target(
            name: "FreeTypeC",
            dependencies: [
                .target(name: "libfreetype"),
                .product(name: "LibPNGC", package: "LibPNG")
            ],
            cxxSettings: [
                .enableWarning("all")
            ]
        ),
        .target(
            name: "FreeType",
            dependencies: [
                .target(name: "FreeTypeC")
            ],
            swiftSettings: [
                .interoperabilityMode(.Cxx)
            ]
        )
    ],
    // The lcms2 library was compiled using c17, so set it also here
    cLanguageStandard: .c17,
    // Also use c++20, we don't live in the stone age, but still not ready to accept c++23
    cxxLanguageStandard: .cxx20
)
