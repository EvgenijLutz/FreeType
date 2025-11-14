//
//  FTGlyphCollection.cpp
//  FreeType
//
//  Created by Evgenij Lutz on 11.09.25.
//

#include <FreeTypeC/FTGlyphCollection.hpp>


// MARK: FTGlyphBitmap

FTGlyphBitmap::FTGlyphBitmap(unsigned long characterCode,
                             const char* fn_nonnull bitmapData,
                             float bitmapWidth, float bitmapHeight,
                             float gWidth, float gHeight,
                             float hBearingX, float hBearingY, float hAdvance,
                             float vBearingX, float vBearingY, float vAdvance):
_referenceCounter(1),
_characterCode(characterCode),
_bitmapData(bitmapData),
_bitmapWidth(bitmapWidth),
_bitmapHeight(bitmapHeight),
_gWidth(gWidth),
_gHeight(gHeight),
_hBearingX(hBearingX),
_hBearingY(hBearingY),
_hAdvance(hAdvance),
_vBearingX(vBearingX),
_vBearingY(vBearingY),
_vAdvance(vAdvance) {
    //
}


FTGlyphBitmap::~FTGlyphBitmap() {
    delete [] _bitmapData;
}


// MARK: FTGlyphCollection

FTGlyphCollection::FTGlyphCollection():
_referenceCounter(1) {
    //
}


FTGlyphCollection::~FTGlyphCollection() {
    //
}

void FTGlyphCollection::_addGlyph(FTGlyphBitmap* fn_nonnull bitmap) {
    _glyphs.push_back(FTGlyphBitmapRetain(bitmap));
}


FN_IMPLEMENT_SWIFT_INTERFACE1(FTGlyphBitmap)
FN_IMPLEMENT_SWIFT_INTERFACE1(FTGlyphCollection)
