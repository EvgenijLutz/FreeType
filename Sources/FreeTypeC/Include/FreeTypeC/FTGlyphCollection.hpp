//
//  FTGlyphCollection.hpp
//  FreeType
//
//  Created by Evgenij Lutz on 11.09.25.
//

#ifndef FTGlyphCollection_hpp
#define FTGlyphCollection_hpp

#include "Common.hpp"

#if defined __cplusplus

class FTLibrary;


class FTGlyphBitmap final {
private:
    std::atomic<size_t> _referenceCounter;
    
    unsigned long _characterCode;
    
    const char* nonnull _bitmapData;
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
    
    FT_REF_FRIEND_INTERFACE(FTGlyphBitmap)
    friend class FTLibrary;
    
    FTGlyphBitmap(unsigned long _characterCode,
                  const char* nonnull bitmapData,
                  float bitmapWidth, float bitmapHeight,
                  float gWidth, float gHeight,
                  float hBearingX, float hBearingY, float hAdvance,
                  float vBearingX, float vBearingY, float vAdvance);
    ~FTGlyphBitmap();
    
public:
    
    const char* nonnull getBitmapData() SWIFT_COMPUTED_PROPERTY { return _bitmapData; }
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
} FT_REF_INTERFACE(FTGlyphBitmap);


class FTGlyphCollection final {
private:
    std::atomic<size_t> _referenceCounter;
    
    std::vector<FTGlyphBitmap* nonnull> _glyphs;
    
    FT_REF_FRIEND_INTERFACE(FTGlyphCollection)
    friend class FTLibrary;
    
    FTGlyphCollection();
    ~FTGlyphCollection();
    
public:
} FT_REF_INTERFACE(FTGlyphCollection);

#endif

#endif // FTGlyphCollection_hpp
