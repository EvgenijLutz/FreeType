//
//  FTFace.cpp
//  FreeType
//
//  Created by Evgenij Lutz on 08.09.25.
//

#include <freetype/freetype.h>
#include <FreeTypeC/FTFace.hpp>


FT_REF_INTERFACE_IMPL(FTFace)


FTFace::FTFace(FT_Face_impl nonnull face):
_referenceCounter(1),
_face(face) {
    //
}


FTFace::~FTFace() {
    FT_Done_Face(_face);
}


long FTFace::getNumCharacters() {
    return static_cast<long>(_face->num_charmaps);
}


std::vector<FTCharacterCode> FTFace::listCharacterCodes() {
    std::vector<FTCharacterCode> characters;
    
    FT_UInt agindex = 0;
    FT_ULong code = FT_Get_First_Char(_face, &agindex);
    while (agindex != 0) {
        code = FT_Get_Next_Char(_face, code, &agindex);
        characters.push_back(static_cast<FTCharacterCode>(code));
    }
    
    FT_Get_First_Char(_face, &agindex);
    
    return characters;
}


FTGlyphIndex FTFace::getGlyphIndex(FTCharacterCode characterCode) {
    auto charcode = static_cast<FT_ULong>(characterCode);
    auto glyphIndex = FT_Get_Char_Index(_face, charcode);
    return static_cast<FTGlyphIndex>(glyphIndex);
}


void* nullable FTFace::loadGlyph(FTGlyphIndex glyphIndex, CommonError* nullable error) {
    resetError(error);
    
    auto ftGlyphIndex = static_cast<FT_UInt>(glyphIndex);
    
    FT_Int32 ftLoadArgs = FT_LOAD_RENDER | FT_LOAD_COLOR;
    
    //auto errorCode = FT_Load_Char(_face, ftCharacterCode, ftLoadArgs);
    
    auto errorCode = FT_Load_Glyph(_face, ftGlyphIndex, ftLoadArgs);
    if (errorCode) {
        setFTError(error, errorCode);
        return nullptr;
    }
    
    return nullptr;
}
