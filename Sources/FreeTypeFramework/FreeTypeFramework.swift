//
//  FreeTypeFramework.swift
//  FreeTypeFramework
//
//  Created by Evgenij Lutz on 25.07.25.
//

import Foundation
@_exported import FreeTypeWrapper

//public struct FTTestStruct {
//    public let text: String
//
//    public init(text: String) {
//        self.text = text
//    }
//}


func test() {
    var library: FT_Library? = nil
    FT_Init_FreeType(&library)
    var face: FT_Face? = nil
    _ = FT_New_Face(library, "tahoma.ttf", 0, &face)
}
