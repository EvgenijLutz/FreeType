//
//  FTFace.hpp
//  FreeType
//
//  Created by Evgenij Lutz on 08.09.25.
//

#pragma once

#include <FreeTypeC/Common.hpp>
#include <FreeTypeC/CommonError.hpp>


class FTLibrary;


typedef long FTGlyphIndex;

/// 64-bit character code used by FreeType.
typedef unsigned long FTCharacterCode;


/// FreeType face context.
class FTFace final {
private:
    std::atomic<size_t> _referenceCounter;
    
    FT_Face_impl fn_nonnull _face;
    
    friend class FTLibrary;
    FN_FRIEND_SWIFT_INTERFACE(FTFace);
    
    FTFace(FT_Face_impl fn_nonnull face);
    ~FTFace();
    
public:
    long getNumCharacters() SWIFT_COMPUTED_PROPERTY;
    
    std::vector<FTCharacterCode> listCharacterCodes() SWIFT_NAME(__listCharacterCodesUnsafe());
    
    /// Return the glyph index of a given character code.
    ///
    /// The glyph index. 0 means `undefined character code`.
    FTGlyphIndex getGlyphIndex(FTCharacterCode characterCode);
    
    
    void* fn_nullable loadGlyph(FTGlyphIndex glyphIndex, CommonError* fn_nullable error) SWIFT_NAME(__loadGlyphUnsafe(_:_:));
    
    FT_Face_impl fn_nonnull _getFace() { return _face; }
    
} FN_SWIFT_INTERFACE(FTFace);


FN_DEFINE_SWIFT_INTERFACE(FTFace)
