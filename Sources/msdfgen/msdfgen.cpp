//
//  msdfgen.cpp
//  FreeType
//
//  Created by Evgenij Lutz on 14.11.25.
//

#include <libfreetype.h>
#include <msdfgen/msdfgen.hpp>
#include "msdfgen.h"
#include "msdfgen-ext.h"
#include "core/ShapeDistanceFinder.h"


SDFImage::SDFImage(const char* fn_nonnull contents, long numChannels, long width, long height):
_referenceCounter(1),
_contents(contents),
_numChannels(numChannels),
_width(width),
_height(height) { }


SDFImage::~SDFImage() {
    delete [] _contents;
}


SDFImage* fn_nonnull SDFImage::create(const float* fn_nonnull contents, long numChannels, long width, long height) SWIFT_RETURNS_RETAINED {
    auto size = width * height * numChannels * sizeof(float);
    auto buffer = new char[size];
    std::memcpy(buffer, contents, size);
    return new SDFImage(buffer, numChannels, width, height);
}


template <int N>
static void invertColor(const msdfgen::BitmapRef<float, N> &bitmap) {
    const float *end = bitmap.pixels+N*bitmap.width*bitmap.height;
    for (float *p = bitmap.pixels; p < end; ++p)
        *p = 1.f-*p;
}


static void parseColoring(msdfgen::Shape &shape, const char *edgeAssignment) {
    unsigned c = 0, e = 0;
    if (shape.contours.size() < c) return;
    msdfgen::Contour *contour = &shape.contours[c];
    bool change = false;
    bool clear = true;
    for (const char *in = edgeAssignment; *in; ++in) {
        switch (*in) {
            case ',':
                if (change)
                    ++e;
                if (clear)
                    while (e < contour->edges.size()) {
                        contour->edges[e]->color = msdfgen::WHITE;
                        ++e;
                    }
                ++c, e = 0;
                if (shape.contours.size() <= c) return;
                contour = &shape.contours[c];
                change = false;
                clear = true;
                break;
            case '?':
                clear = false;
                break;
            case 'C': case 'M': case 'W': case 'Y': case 'c': case 'm': case 'w': case 'y':
                if (change) {
                    ++e;
                    change = false;
                }
                if (e < contour->edges.size()) {
                    contour->edges[e]->color = msdfgen::EdgeColor(
                                                         (*in == 'C' || *in == 'c')*msdfgen::CYAN|
                                                         (*in == 'M' || *in == 'm')*msdfgen::MAGENTA|
                                                         (*in == 'Y' || *in == 'y')*msdfgen::YELLOW|
                                                         (*in == 'W' || *in == 'w')*msdfgen::WHITE);
                    change = true;
                }
                break;
        }
    }
}


SDFImage* fn_nullable generateSDFImage(FTFace* fn_nonnull face, unsigned long unicodeIndex) SWIFT_RETURNS_RETAINED {
    msdfgen::Shape shape;
    struct FreetypeFontGuard {
        msdfgen::FreetypeHandle *ft;
        msdfgen::FontHandle *font;
        FreetypeFontGuard() : ft(), font() { }
        ~FreetypeFontGuard() {
            if (ft) {
                if (font) {
                    destroyFont(font);
                }
                
                deinitializeFreetype(ft);
            }
        }
    } guard;
    
    // Init library
    guard.ft = msdfgen::initializeFreetype();
    if (guard.ft == nullptr) {
        printf("Failed to initialize FreeType library.\n");
        return nullptr;
    }
    
    // Get character codes
    auto characterCodes = face->listCharacterCodes();
    if (unicodeIndex >= characterCodes.size()) {
        printf("Wrong unicode index: %ld. Number of unicode characters: %ld\n", unicodeIndex, characterCodes.size());
        return nullptr;
    }
    
    // Get font
    auto font = face->_getFace();
    guard.font = msdfgen::adoptFreetypeFont(font);
    if (guard.font == nullptr) {
        printf("Failed to initialize FreeType font.\n");
        return nullptr;
    }
    
    // Load glyph
    auto unicode = static_cast<unsigned int>(characterCodes[unicodeIndex]);
    auto fontCoordinateScaling = msdfgen::FONT_SCALING_LEGACY;
    msdfgen::GlyphIndex glyphIndex;
    getGlyphIndex(glyphIndex, guard.font, unicode);
    double glyphAdvance = 0;
    auto result = msdfgen::loadGlyph(shape, guard.font, glyphIndex, fontCoordinateScaling, &glyphAdvance);
    if (result == false) {
        printf("Failed to load glyph from font file.\n");
        return nullptr;
    }
    
    // Validate shape
    auto shapeValid = shape.validate();
    if (shapeValid == false) {
        printf("The geometry of the loaded shape is invalid.\n");
        return nullptr;
    }
    
    shape.normalize();
    
    auto yFlip = true;
    if (yFlip) {
        shape.inverseYAxis = !shape.inverseYAxis;
    }
    
    msdfgen::Vector2 scale = 1;
    double avgScale = .5*(scale.x+scale.y);
    auto bounds = shape.getBounds();
    
    auto outputDistanceShift = true;
    enum {
        RANGE_UNIT,
        RANGE_PX
    } rangeMode = RANGE_PX;
    msdfgen::Range range(1);
    msdfgen::Range pxRange(2);
    if (outputDistanceShift) {
        auto &rangeRef = rangeMode == RANGE_PX ? pxRange : range;
        double rangeShift = -outputDistanceShift*(rangeRef.upper-rangeRef.lower);
        rangeRef.lower += rangeShift;
        rangeRef.upper += rangeShift;
    }
    
    // Auto-frame
    auto autoFrame = true;
    int width = 64, height = 64;
    bool scaleSpecified = false;
    msdfgen::Vector2 translate;
    if (autoFrame) {
        double l = bounds.l, b = bounds.b, r = bounds.r, t = bounds.t;
        msdfgen::Vector2 frame(width, height);
        if (!scaleSpecified) {
            if (rangeMode == RANGE_UNIT) {
                l += range.lower, b += range.lower, r -= range.lower, t -= range.lower;
            }
            else {
                frame += 2*pxRange.lower;
            }
        }
        if (l >= r || b >= t) {
            l = 0, b = 0, r = 1, t = 1;
        }
        if (frame.x <= 0 || frame.y <= 0) {
            printf("Cannot fit the specified pixel range.\n");
            return nullptr;
        }
        msdfgen::Vector2 dims(r-l, t-b);
        if (scaleSpecified) {
            translate = .5*(frame/scale-dims)-msdfgen::Vector2(l, b);
        }
        else {
            if (dims.x*frame.y < dims.y*frame.x) {
                translate.set(.5*(frame.x/frame.y*dims.y-dims.x)-l, -b);
                scale = avgScale = frame.y/dims.y;
            } else {
                translate.set(-l, .5*(frame.y/frame.x*dims.x-dims.y)-b);
                scale = avgScale = frame.x/dims.x;
            }
        }
        if (rangeMode == RANGE_PX && !scaleSpecified) {
            translate -= pxRange.lower/scale;
        }
    }
    
    if (rangeMode == RANGE_PX) {
        range = pxRange/std::min(scale.x, scale.y);
    }
    
    
    enum {
        SINGLE,
        PERPENDICULAR,
        MULTI,
        MULTI_AND_TRUE,
        METRICS
    } mode = MULTI_AND_TRUE;
    enum {
        NO_PREPROCESS,
        WINDING_PREPROCESS,
        FULL_PREPROCESS
    } geometryPreproc = (
#ifdef MSDFGEN_USE_SKIA
                         FULL_PREPROCESS
#else
                         NO_PREPROCESS
#endif
                         );
    bool scanlinePass = geometryPreproc == NO_PREPROCESS;
    msdfgen::MSDFGeneratorConfig generatorConfig;
    generatorConfig.overlapSupport = geometryPreproc == NO_PREPROCESS;
    bool explicitErrorCorrectionMode = false;
    
    
    // Compute output
    msdfgen::SDFTransformation transformation(msdfgen::Projection(scale, translate), range);
    msdfgen::Bitmap<float, 1> sdf;
    msdfgen::Bitmap<float, 3> msdf;
    msdfgen::Bitmap<float, 4> mtsdf;
    msdfgen::MSDFGeneratorConfig postErrorCorrectionConfig(generatorConfig);
    if (scanlinePass) {
        if (explicitErrorCorrectionMode && generatorConfig.errorCorrection.distanceCheckMode != msdfgen::ErrorCorrectionConfig::DO_NOT_CHECK_DISTANCE) {
            const char *fallbackModeName = "unknown";
            switch (generatorConfig.errorCorrection.mode) {
                case msdfgen::ErrorCorrectionConfig::DISABLED: fallbackModeName = "disabled"; break;
                case msdfgen::ErrorCorrectionConfig::INDISCRIMINATE: fallbackModeName = "distance-fast"; break;
                case msdfgen::ErrorCorrectionConfig::EDGE_PRIORITY: fallbackModeName = "auto-fast"; break;
                case msdfgen::ErrorCorrectionConfig::EDGE_ONLY: fallbackModeName = "edge-fast"; break;
            }
            fprintf(stderr, "Selected error correction mode not compatible with scanline pass, falling back to %s.\n", fallbackModeName);
        }
        generatorConfig.errorCorrection.mode = msdfgen::ErrorCorrectionConfig::DISABLED;
        postErrorCorrectionConfig.errorCorrection.distanceCheckMode = msdfgen::ErrorCorrectionConfig::DO_NOT_CHECK_DISTANCE;
    }
    
    bool legacyMode = false;
    bool skipColoring = false;
    double angleThreshold = 3.;
    unsigned long long coloringSeed = 0;
    void (*edgeColoring)(msdfgen::Shape &, double, unsigned long long) = &msdfgen::edgeColoringSimple;
    const char *edgeAssignment = NULL;
    switch (mode) {
        case SINGLE: {
            sdf = msdfgen::Bitmap<float, 1>(width, height);
            if (legacyMode)
                generateSDF_legacy(sdf, shape, range, scale, translate);
            else
                generateSDF(sdf, shape, transformation, generatorConfig);
            break;
        }
        case PERPENDICULAR: {
            sdf = msdfgen::Bitmap<float, 1>(width, height);
            if (legacyMode)
                generatePSDF_legacy(sdf, shape, range, scale, translate);
            else
                generatePSDF(sdf, shape, transformation, generatorConfig);
            break;
        }
        case MULTI: {
            if (!skipColoring)
                edgeColoring(shape, angleThreshold, coloringSeed);
            if (edgeAssignment)
                parseColoring(shape, edgeAssignment);
            msdf = msdfgen::Bitmap<float, 3>(width, height);
            if (legacyMode)
                generateMSDF_legacy(msdf, shape, range, scale, translate, generatorConfig.errorCorrection);
            else
                generateMSDF(msdf, shape, transformation, generatorConfig);
            break;
        }
        case MULTI_AND_TRUE: {
            if (!skipColoring)
                edgeColoring(shape, angleThreshold, coloringSeed);
            if (edgeAssignment)
                parseColoring(shape, edgeAssignment);
            mtsdf = msdfgen::Bitmap<float, 4>(width, height);
            if (legacyMode)
                generateMTSDF_legacy(mtsdf, shape, range, scale, translate, generatorConfig.errorCorrection);
            else
                generateMTSDF(mtsdf, shape, transformation, generatorConfig);
            break;
        }
        default:;
    }
    
    enum {
        KEEP,
        REVERSE,
        GUESS
    } orientation = KEEP;
    if (orientation == GUESS) {
        // Get sign of signed distance outside bounds
        msdfgen::Point2 p(bounds.l-(bounds.r-bounds.l)-1, bounds.b-(bounds.t-bounds.b)-1);
        double distance = msdfgen::SimpleTrueShapeDistanceFinder::oneShotDistance(shape, p);
        orientation = distance <= 0 ? KEEP : REVERSE;
    }
    if (orientation == REVERSE) {
        switch (mode) {
            case SINGLE:
            case PERPENDICULAR:
                invertColor<1>(sdf);
                break;
            case MULTI:
                invertColor<3>(msdf);
                break;
            case MULTI_AND_TRUE:
                invertColor<4>(mtsdf);
                break;
            default:;
        }
    }
    
    msdfgen::FillRule fillRule = msdfgen::FILL_NONZERO;
    if (scanlinePass) {
        switch (mode) {
            case SINGLE:
            case PERPENDICULAR:
                distanceSignCorrection(sdf, shape, transformation, fillRule);
                break;
            case MULTI:
                distanceSignCorrection(msdf, shape, transformation, fillRule);
                msdfErrorCorrection(msdf, shape, transformation, postErrorCorrectionConfig);
                break;
            case MULTI_AND_TRUE:
                distanceSignCorrection(mtsdf, shape, transformation, fillRule);
                msdfErrorCorrection(mtsdf, shape, transformation, postErrorCorrectionConfig);
                break;
            default:;
        }
    }
    
    
    switch (mode) {
        case SINGLE:
            return SDFImage::create(sdf, 1, sdf.width(), sdf.height());
        
        case PERPENDICULAR:
            return SDFImage::create(sdf, 1, sdf.width(), sdf.height());
            
        case MULTI:
            return SDFImage::create(msdf, 3, msdf.width(), msdf.height());
            
        case MULTI_AND_TRUE:
            return SDFImage::create(mtsdf, 4, mtsdf.width(), mtsdf.height());
            
        default:
            return nullptr;
    }
    
    //return nullptr;
}

FN_IMPLEMENT_SWIFT_INTERFACE1(SDFImage)
