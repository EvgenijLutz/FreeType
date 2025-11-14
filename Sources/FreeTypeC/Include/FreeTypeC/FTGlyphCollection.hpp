//
//  FTGlyphCollection.hpp
//  FreeType
//
//  Created by Evgenij Lutz on 11.09.25.
//

#pragma once

#include "Common.hpp"


class FTLibrary;


class FTGlyphBitmap final {
private:
    std::atomic<size_t> _referenceCounter;
    
    unsigned long _characterCode;
    
    const char* fn_nonnull _bitmapData;
    float _bitmapWidth;
    float _bitmapHeight;
    
    float _gWidth;
    float _gHeight;
    
    float _hBearingX;
    float _hBearingY;
    float _hAdvance;
    
    float _vBearingX;
    float _vBearingY;
    float _vAdvance;
    
    friend class FTLibrary;
    FN_FRIEND_SWIFT_INTERFACE(FTGlyphBitmap)
    
    FTGlyphBitmap(unsigned long _characterCode,
                  const char* fn_nonnull bitmapData,
                  float bitmapWidth, float bitmapHeight,
                  float gWidth, float gHeight,
                  float hBearingX, float hBearingY, float hAdvance,
                  float vBearingX, float vBearingY, float vAdvance);
    ~FTGlyphBitmap();
    
public:
    const char* fn_nonnull getBitmapData() SWIFT_COMPUTED_PROPERTY { return _bitmapData; }
    float getBitmapWidth() SWIFT_COMPUTED_PROPERTY { return _bitmapWidth; }
    float getBitmapHeight() SWIFT_COMPUTED_PROPERTY { return _bitmapHeight; }
    
    float getWidth() SWIFT_COMPUTED_PROPERTY { return _gWidth; }
    float getHeight() SWIFT_COMPUTED_PROPERTY { return _gHeight; }
    
    float getHorizontalBearingX() SWIFT_COMPUTED_PROPERTY { return _hBearingX; }
    float getHorizontalBearingY() SWIFT_COMPUTED_PROPERTY { return _hBearingY; }
    float getHorizontalAdvance() SWIFT_COMPUTED_PROPERTY { return _hAdvance; }
    
    float getVerticalBearingX() SWIFT_COMPUTED_PROPERTY { return _vBearingX; }
    float getVerticalBearingY() SWIFT_COMPUTED_PROPERTY { return _vBearingY; }
    float getVerticalAdvance() SWIFT_COMPUTED_PROPERTY { return _vAdvance; }
} FN_SWIFT_INTERFACE(FTGlyphBitmap);


class FTGlyphCollection final {
private:
    std::atomic<size_t> _referenceCounter;
    
    std::vector<FTGlyphBitmap* fn_nonnull> _glyphs;
    
    friend class FTLibrary;
    FN_FRIEND_SWIFT_INTERFACE(FTGlyphCollection)
    
    FTGlyphCollection();
    ~FTGlyphCollection();
    
    void _addGlyph(FTGlyphBitmap* fn_nonnull bitmap);
    
public:
} FN_SWIFT_INTERFACE(FTGlyphCollection);


FN_DEFINE_SWIFT_INTERFACE(FTGlyphBitmap)
FN_DEFINE_SWIFT_INTERFACE(FTGlyphCollection)
