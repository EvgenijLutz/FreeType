//
//  FTFace.hpp
//  FreeType
//
//  Created by Evgenij Lutz on 08.09.25.
//

#ifndef FTFace_hpp
#define FTFace_hpp

#if defined __cplusplus

#include "Common.hpp"
#include <FreeTypeC/CommonError.hpp>


class FTLibrary;


typedef long FTGlyphIndex;

/// 64-bit character code used by FreeType.
typedef unsigned long FTCharacterCode;


/// FreeType face context.
class FTFace final {
private:
    std::atomic<size_t> _referenceCounter;
    
    FT_Face_impl nonnull _face;
    
    friend class FTLibrary;
    FT_REF_FRIEND_INTERFACE(FTFace);
    
    FTFace(FT_Face_impl nonnull face);
    ~FTFace();
    
public:
    long getNumCharacters() SWIFT_COMPUTED_PROPERTY;
    
    std::vector<FTCharacterCode> listCharacterCodes() SWIFT_NAME(__listCharacterCodesUnsafe());
    
    /// Return the glyph index of a given character code.
    ///
    /// The glyph index. 0 means `undefined character code`.
    FTGlyphIndex getGlyphIndex(FTCharacterCode characterCode);
    
    
    void* nullable loadGlyph(FTGlyphIndex glyphIndex, CommonError* nullable error) SWIFT_NAME(__loadGlyphUnsafe(_:_:));
    
} FT_REF_INTERFACE(FTFace);

#endif // __cplusplus

#endif // FTFace_hpp
