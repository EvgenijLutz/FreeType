//
//  FTLibrary.swift
//  FreeType
//
//  Created by Evgenij Lutz on 08.09.25.
//

import FreeTypeC


public extension FTLibrary {
    /// Creates a FreeType library.
    ///
    /// - Throws: An error if a library cannot be created.
    /// - Returns: a new FreeType library.
    static func create() throws -> FTLibrary {
        var error = CommonError()
        let library = FTLibrary.__createUnsafe(&error)
        guard let library else {
            throw error.ftError
        }
        
        return library
    }
    
    
    /// Reads number of faces in a font.
    ///
    /// - Throws: An error if a font at the specified path cannot be opened.
    /// - Returns: Number of faces.
    func readNumFaces(path: String) throws -> Int {
        return try path.withCString { pointer in
            var error = CommonError()
            let numFaces = __readNumFacesUnsafe(pointer, &error)
            guard numFaces >= 0 else {
                throw error.ftError
            }
            
            return numFaces
        }
    }
    
    
    /// Reads number of instances of a face at the specified index in a font.
    ///
    /// - Throws: An error if a font at the specified path cannot be opened.
    /// - Returns: Number of face instances.
    func readNumFaceInstances(path: String, faceIndex: Int) throws -> Int {
        return try path.withCString { pointer in
            var error = CommonError()
            let numInstances = __readNumFaceInstancesUnsafe(pointer, faceIndex, &error)
            guard numInstances >= 0 else {
                throw error.ftError
            }
            
            return numInstances
        }
    }
    
    
    /// Opens a face in a font file.
    ///
    /// - Parameter path: Path to a font file.
    /// - Parameter faceIndex: Index of a font face.
    /// - Parameter instanceIndex: Index of a font face instance.
    ///
    /// Before opening a face, you probably should first determine the number of faces in the font file and number of face variants if presented. See ``FTLibrary/readNumFaces(path:)`` and ``FTLibrary/readNumFaceInstances(path:faceIndex:)`` for more information.
    ///
    /// - Throws: An error if a font at the specified path cannot be opened.
    /// - Returns: A font face.
    func openFace(at path: String, faceIndex: Int = 0, instanceIndex: Int = 0) throws -> FTFace {
        return try path.withCString { pointer in
            var error = CommonError()
            let face = __openFaceUnsafe(pointer, faceIndex, instanceIndex, &error)
            guard let face else {
                throw error.ftError
            }
            
            return face
        }
    }
    
}
