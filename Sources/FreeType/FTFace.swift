//
//  FTFace.swift
//  FreeType
//
//  Created by Evgenij Lutz on 08.09.25.
//

import FreeTypeC


public extension FTFace {
    func listCharacterCodes() -> [FTCharacterCode] {
        let codes = __listCharacterCodesUnsafe()
        return codes.map { $0 }
    }
}
