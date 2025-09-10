//
//  FTLibrary.cpp
//  FreeType
//
//  Created by Evgenij Lutz on 08.09.25.
//

#include <libfreetype.h>
#include <FreeTypeC/FTLibrary.hpp>
#include <FreeTypeC/FTFace.hpp>


FT_REF_INTERFACE_IMPL(FTLibrary)


FTLibrary::FTLibrary(FT_Library_impl nonnull library):
_referenceCounter(1),
_library(library) {
}


FTLibrary::~FTLibrary() {
    FT_Done_FreeType(_library);
}


FTLibrary* nullable FTLibrary::create(CommonError* nullable error) {
    resetError(error);
    FT_Library library;
    auto errorCode = FT_Init_FreeType(&library);
    if (errorCode) {
        setFTError(error, errorCode);
        return nullptr;
    }
    
    return new FTLibrary(library);
}


long FTLibrary::readNumFaces(const char* nonnull path, CommonError* nullable error) {
    resetError(error);
    // About face examination (see the face_index parameter):
    // https://freetype.org/freetype2/docs/reference/ft2-face_creation.html#ft_open_face
    FT_Face face;
    auto errorCode = FT_New_Face(_library, path, -1, &face);
    if (errorCode) {
        setFTError(error, errorCode);
        return -1;
    }
    
    // Number of faces within the font file
    auto numFaces = face->num_faces;
    
    // Clean up
    FT_Done_Face(face);
    
    return numFaces;
}


long FTLibrary::readNumFaceInstances(const char* nonnull path, long faceIndex, CommonError* nullable error) {
    resetError(error);
    // About face examination (see the face_index parameter):
    // https://freetype.org/freetype2/docs/reference/ft2-face_creation.html#ft_open_face
    FT_Face face;
    auto errorCode = FT_New_Face(_library, path, -(std::abs(faceIndex) + 1), &face);
    if (errorCode) {
        setFTError(error, errorCode);
        return -1;
    }
    
    // 16-30 bits are the number of named instances in face 'faceIndex' if we have a variation font (or zero otherwise)
    auto styleFlags = face->style_flags;
    
    // Clean up
    FT_Done_Face(face);
    
    return styleFlags >> 16;
}


FTFace* nullable FTLibrary::openFace(const char* nonnull path, long faceIndex, long faceInstanceIndex, CommonError* nullable error) {
    resetError(error);
    auto faceFlags = std::abs(faceIndex) | (std::abs(faceInstanceIndex) << 16);
    FT_Face face;
    auto errorCode = FT_New_Face(_library, path, faceFlags, &face);
    
    // Or also load it from memory:
    //auto errorCode = FT_New_Memory_Face(_library, nullptr, 123, faceFlags, &face);
    
    if (errorCode) {
        setFTError(error, errorCode);
        return nullptr;
    }
    
    return new FTFace(face);
}


//void FTLibrary::exportGlyphs(const char* nonnull path, long faceIndex, long faceInstanceIndex, long width, long height, CommonError* nullable error, FTProgressCallback nullable progressCallback) {
//    // Success by default
//    resetError(error);
//    
//    // Open the font face
//    auto faceFlags = std::abs(faceIndex) | (std::abs(faceInstanceIndex) << 16);
//    FT_Face face;
//    auto errorCode = FT_New_Face(_library, path, faceFlags, &face);
//    if (errorCode) {
//        setFTError(error, errorCode);
//        return nullptr;
//    }
//    
//    // Clean up
//    FT_Done_Face(face);
//}
