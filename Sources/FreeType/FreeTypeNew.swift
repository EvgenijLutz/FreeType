//
//  FreeTypeNew.swift
//  FreeType
//
//  Created by Evgenij Lutz on 08.07.26.
//

import Foundation
import libfreetype_c


public enum FreeTypeError: Error {
    case notImplemented
    case unknownError
    case valueShouldNotBeNil
}


extension FT_Error {
    func throwIfError() throws {
        switch self {
        case 0:
            break
            
        default:
            throw FreeTypeError.unknownError
        }
    }
}


func getOrThrow<T>(_ value: T?) throws -> T {
    guard let value else {
        throw FreeTypeError.valueShouldNotBeNil
    }
    
    return value
}


public final class FreeTypeLibrary {
    private var library: FT_Library
    
    public let major: Int
    public let minor: Int
    public let patch: Int
    public let version: String
    
    
    public init() throws {
        // Init the library
        var library: FT_Library?
        let error = FT_Init_FreeType(&library)
        try error.throwIfError()
        self.library = try getOrThrow(library)
        
        // Get the library version
        var major: FT_Int = .init(FREETYPE_MAJOR)
        var minor: FT_Int = .init(FREETYPE_MINOR)
        var patch: FT_Int = .init(FREETYPE_PATCH)
        FT_Library_Version(library, &major, &minor, &patch)
        
        self.major = .init(major)
        self.minor = .init(minor)
        self.patch = .init(patch)
        self.version = "\(major).\(minor).\(patch)"
    }
    
    deinit {
        FT_Done_FreeType(library)
    }
    
    
    public func numFaces(path: String) throws -> Int {
        var face: FT_Face?
        let error = FT_New_Face(library, path, -1, &face)
        try error.throwIfError()
        let numFaces = face?.pointee.num_faces ?? 0
        FT_Done_Face(face)
        return .init(numFaces)
    }
    
    
    public func numInstances(path: String, index: Int) throws -> Int {
        var face: FT_Face?
        let error = FT_New_Face(library, path, .init(index & 0xFFFF), &face)
        try error.throwIfError()
        let numInstances = ((face?.pointee.style_flags ?? 0) >> 16) & 0xFFFF
        FT_Done_Face(face)
        return .init(numInstances)
    }
    
    
    /// Open a face.
    ///
    /// - Seealso: [Face creation](https://freetype.org/freetype2/docs/reference/ft2-face_creation.html#ft_open_face)
    public func openFace(path: String, index: Int = 0, instance: Int = 0) throws -> FreeTypeFace {
        var face: FT_Face?
        let faceIndex = (index & 0xFFFF) | ((instance & 0xFFFF) << 16)
        let error = FT_New_Face(library, path, .init(faceIndex), &face)
        try error.throwIfError()
        return .init(try getOrThrow(face))
    }
}


extension UnsafeMutablePointer<FT_String> {
    var swiftString: String {
        String(cString: self)
    }
}


public struct FreeTypeCharacter: Sendable {
    public let index: FT_UInt32
    public let code: FT_UInt64
}


public final class FreeTypeFace: @unchecked Sendable {
    private var face: FT_Face
    
    public let numFaces: Int
    public let fontName: String?
    public let styleName: String?
    
    fileprivate init(_ face: FT_Face) {
        self.face = face
        
        numFaces = face.pointee.num_faces
        fontName = face.pointee.family_name?.swiftString
        styleName = face.pointee.style_name?.swiftString
    }
    
    deinit {
        FT_Done_Face(face)
    }
    
    
    public func listCharacters() -> [FreeTypeCharacter] {
        var characters: [FreeTypeCharacter] = []
        
        // Get first character info
        var currentIndex: FT_UInt = 0
        var currentCode = unsafe FT_Get_First_Char(face, &currentIndex)
        characters.append(.init(index: currentIndex, code: currentCode))
        
        // Get subsequent character infos
        while currentIndex != 0 {
            currentCode = unsafe FT_Get_Next_Char(face, currentCode, &currentIndex)
            characters.append(.init(index: currentIndex, code: currentCode))
        }
        
        return characters
    }
    
    
    public func render(code: FT_UInt64, width: Int, height: Int) throws -> Data {
        
        func from_26_6(_ value: FT_Pos) -> Float {
            let denom = Float(1.0 / 64.0)
            let x = Float(value >> 6)
            let y = Float(value & 0b111111) * denom
            return x + y;
        }
        
        func to_26_6(_ value: Float) -> FT_Long {
            let nom = Float(1.0 * 64.0)
            let x = FT_Long(floor(value)) << 6
            let y = FT_Long(nom * (value - Float(x)))
            return x | y
        }
        
        // Set render size
        var error = FT_Set_Pixel_Sizes(face, .init(width), .init(height))
        try error.throwIfError()
        
        // Render the character
        error = FT_Load_Char(face, code, .init(FT_LOAD_RENDER))
        try error.throwIfError()
        
        // Get bitmap info
        //let bitmap: FT_Bitmap = face.pointee.glyph.pointee.bitmap
        // TODO: Finished here
//        switch bitmap.pixel_mode {
//            case FT_PIXEL_MODE_GRAY
//        }
        
        let data = Data(count: width * height)
        return data
    }
}
