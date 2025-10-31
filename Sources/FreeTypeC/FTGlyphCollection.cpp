//
//  FTGlyphCollection.cpp
//  FreeType
//
//  Created by Evgenij Lutz on 11.09.25.
//

#include <FreeTypeC/FTGlyphCollection.hpp>


FT_REF_INTERFACE_IMPL(FTGlyphBitmap)


FTGlyphBitmap::FTGlyphBitmap(unsigned long characterCode,
                             const char* nonnull bitmapData,
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
