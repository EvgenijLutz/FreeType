//
//  FreeType.swift
//  FreeType
//
//  Created by Evgenij Lutz on 25.07.25.
//

import Foundation
import libfreetype
@_exported import FreeTypeC
import msdfgen


func test() {
    var library: FT_Library? = nil
    FT_Init_FreeType(&library)
    
    var face: FT_Face? = nil
    _ = FT_New_Face(library, "tahoma.ttf", 0, &face)
}


public func testFont(path: String, _ index: Int) throws -> SDFImage? {
    let library = try FTLibrary.create()
    let face = try library.openFace(at: path)
//    let characters = face.listCharacterCodes()
//    
//    if let firstCharacterCode = characters.first {
//        print("First character code: \(firstCharacterCode)")
//        let glyphIndex = face.getGlyphIndex(firstCharacterCode)
//        print("Glyph index: \(glyphIndex)")
//    }
//    
//    print("Num characters: \(characters.count)")
    
    return generateSDFImage(face, .init(index))
}
