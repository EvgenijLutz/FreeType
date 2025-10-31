//
//  FTLibrary.cpp
//  FreeType
//
//  Created by Evgenij Lutz on 08.09.25.
//

#include <libfreetype.h>
#include <FreeTypeC/FTLibrary.hpp>
#include <FreeTypeC/FTFace.hpp>
#include <FreeTypeC/FTGlyphCollection.hpp>


FT_REF_INTERFACE_IMPL(FTLibrary)
FT_REF_INTERFACE_IMPL(FTGlyphCollection)


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


FTGlyphCollection* FTLibrary::exportGlyphs(const char* nonnull path, long faceIndex, long faceInstanceIndex, long width, long height, CommonError* nullable error, FTProgressCallback nullable progressCallback) {
    // Success by default
    resetError(error);
    
    // Open the font face
    auto faceFlags = std::abs(faceIndex) | (std::abs(faceInstanceIndex) << 16);
    FT_Face face;
    auto errorCode = FT_New_Face(_library, path, faceFlags, &face);
    if (errorCode) {
        setFTError(error, errorCode);
        return nullptr;
    }
    
    
    struct converter {
        static float from_26_6(FT_Pos value) {
            constexpr auto denom = 1.0f / 64.0f;
            
            auto x = static_cast<float>(value >> 6);
            auto y = static_cast<float>(value & 0b111111) * denom;
            
            return x + y;
        }
        
        static FT_Long to_26_6(float value) {
            auto x = static_cast<FT_Long>(floorf(value));
            auto y = static_cast<FT_Long>(64.0f * (value - static_cast<float>(x)));
            return (x << 6) | y;
        }
    };
    
    
    FT_UInt agindex = 0;
        FT_ULong code = FT_Get_First_Char(face, &agindex);
        long numCharacters = 0;
    while (agindex != 0) {
        numCharacters += 1;
        code = FT_Get_Next_Char(face, code, &agindex);
        
        
        
        
        
        
        
#if 0
        if (FT_Set_Char_Size(ftFace,
                             converter::to_26_6(static_cast<float>(width / 2)),
                             converter::to_26_6(static_cast<float>(height / 2)),
                             144,
                             144)) {
            // Failed to set size
            callback(userInfo, FTLoadGlyphResult::failure(0));
            return;
        }
#elif 0
        FT_Size_RequestRec request = {
            .type = FT_SIZE_REQUEST_TYPE_NOMINAL,
            .width = converter::to_26_6(static_cast<float>(width / 2)),
            .height = converter::to_26_6(static_cast<float>(height / 2)),
            .horiResolution = 144,
            .vertResolution = 144
        };
        
        if (FT_Request_Size(ftFace, &request)) {
            // Failed to set size
            callback(userInfo, FTLoadGlyphResult::failure(0));
            return;
        }
#else
        if (FT_Set_Pixel_Sizes(face, static_cast<FT_UInt>(width), static_cast<FT_UInt>(height))) {
            // Failed to set size
            setFTError(error, -1);
            return;
        }
#endif
        
        //if (FT_Load_Glyph(face, code, FT_LOAD_RENDER)) {
        if (FT_Load_Char(face, code, FT_LOAD_RENDER)) {
            // Failed to render the character
            setFTError(error, -1);
            return;
        }
        
        auto bitmap = face->glyph->bitmap;
        if (bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) {
            // Unsupported pixel mode
            setFTError(error, -1);
            return;
        }
        
        if (bitmap.width < 1 || bitmap.rows < 1) {
            // No glyph for symbol
            setFTError(error, -1);
            return;
        }
        
        auto metrics = face->glyph->metrics;
        
        auto gWidth = converter::from_26_6(metrics.width);
        auto gHeight = converter::from_26_6(metrics.height);
        
        // Check if calculations are correct
        //auto testX = converter::to_26_6(gWidth);
        //auto checkX = converter::from_26_6(testX);
        //auto check2 = converter::to_26_6(checkX);
        
        auto hBearingX = converter::from_26_6(metrics.horiBearingX);
        auto hBearingY = converter::from_26_6(metrics.horiBearingY);
        auto hAdvance = converter::from_26_6(metrics.horiAdvance);
        
        auto vBearingX = converter::from_26_6(metrics.vertBearingX);
        auto vBearingY = converter::from_26_6(metrics.vertBearingY);
        auto vAdvance = converter::from_26_6(metrics.vertAdvance);
        
        
        //#error Finished here. Get glyph information to use it in layout
        
        
        auto dataSize = bitmap.width * bitmap.rows;
        auto data = new char[dataSize];
        memcpy(data, bitmap.buffer, dataSize);
        
        auto characterCode = static_cast<unsigned long>(code);
        auto glyph = new FTGlyphBitmap(characterCode,
                                       data,
                                       bitmap.width, bitmap.rows,
                                       gWidth, gHeight,
                                       hBearingX, hBearingY, hAdvance,
                                       vBearingX, vBearingY, vAdvance);
        
        //callback(userInfo, FTLoadGlyphResult::successTransferred(glyph));
        
        
        
        
        
        
        
    }
    
    
    // Clean up
    FT_Done_Face(face);
    
    return nullptr;
}


// MARK: - FTGlyphCollection

FTGlyphCollection::FTGlyphCollection():
_referenceCounter() {
    
}

FTGlyphCollection::~FTGlyphCollection() {
    //
}
