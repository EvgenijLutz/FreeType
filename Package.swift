// swift-tools-version: 6.1
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "FreeTypeFramework",
    platforms: [
        .macOS(.v10_13),
        .iOS(.v12),
        .tvOS(.v12),
        .visionOS(.v1),
        .watchOS(.v8)
    ],
    products: [
        .library(
            name: "FreeTypeFramework",
            targets: ["FreeTypeFramework"]
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
        .package(url: "https://github.com/EvgenijLutz/LibPNGFramework.git", exact: "1.6.50-alpha2")
    ],
    targets: [
        .binaryTarget(
            name: "FreeType",
            path: "FreeType.xcframework"
        ),
        .target(
            name: "FreeTypeC",
            dependencies: [
                .product(name: "LibPNGC", package: "LibPNGFramework"),
                .target(name: "FreeType")
            ],
            swiftSettings: [
                .interoperabilityMode(.C)
            ]
        ),
        .target(
            name: "FreeTypeFramework",
            dependencies: [
                .target(name: "FreeTypeC")
            ],
            swiftSettings: [
                .interoperabilityMode(.C)
            ]
        )
    ]
)
