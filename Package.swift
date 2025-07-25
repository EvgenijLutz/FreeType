// swift-tools-version: 6.1
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "FreeTypeFramework",
    platforms: [
        .macOS(.v11),
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
        // Probably will be deleted in the future
        .library(
            name: "FreeTypeFrameworkTarget",
            targets: ["libfreetype"]
        ),
    ],
    targets: [
        .binaryTarget(
            name: "libfreetype",
            path: "libfreetype.xcframework"
        ),
        .target(
            name: "FreeTypeWrapper",
            dependencies: [
                .target(name: "libfreetype")
            ],
            swiftSettings: [
                .interoperabilityMode(.C)
            ]
        ),
        .target(
            name: "FreeTypeFramework",
            dependencies: [
                .target(name: "FreeTypeWrapper")
            ],
            swiftSettings: [
                .interoperabilityMode(.C)
            ]
        )
    ]
)
