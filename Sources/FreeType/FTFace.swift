//
//  FTFace.swift
//  FreeType
//
//  Created by Evgenij Lutz on 08.09.25.
//

import FreeTypeC


@available(macOS 13.3, iOS 16.4, tvOS 16.4, visionOS 1.0, watchOS 9.4, *)
public extension FTFace {
    func listCharacterCodes() -> [FTCharacterCode] {
        let codes = __listCharacterCodesUnsafe()
        return codes.map { $0 }
    }
}
