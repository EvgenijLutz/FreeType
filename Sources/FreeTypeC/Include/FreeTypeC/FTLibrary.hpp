//
//  FTLibrary.hpp
//  FreeType
//
//  Created by Evgenij Lutz on 08.09.25.
//

#pragma once

#include "Common.hpp"
#include "CommonError.hpp"


class FTLibrary;
class FTFace;
class FTGlyphCollection;
class FTGlyphBitmap;


typedef bool (* FTProgressCallback)(void* nullable userInfo, float progress);


/// FreeType library that uses ``FT_Library`` under the hood.
///
/// - Note: After some experience, it looks like FreeType is thread-safe rather when it's used correctly. There are some mutex activities mentioned on the Internet, especially between ``FT_New_Face`` and ``FT_Done_Face`` function calls that lock ``FT_Library``'s access during the ``FT_Face`` usage between these methods. Therefore, it's recommended to use one ``FTLibrary`` and its objects per thread, i.e. work in a single-threaded environment.
class FTLibrary final {
private:
    std::atomic<size_t> _referenceCounter;
    FT_Library_impl nonnull _library;
    
    FT_REF_FRIEND_INTERFACE(FTLibrary);
    
    FTLibrary(FT_Library_impl nonnull library);
    ~FTLibrary();
    
public:
    static FTLibrary* nullable create(CommonError* nullable error = nullptr)
    SWIFT_NAME(__createUnsafe(_:))
    SWIFT_RETURNS_RETAINED;
    
    
    long readNumFaces(const char* nonnull path, CommonError* nullable error = nullptr)
    SWIFT_NAME(__readNumFacesUnsafe(_:_:));
    
    
    long readNumFaceInstances(const char* nonnull path, long faceIndex, CommonError* nullable error = nullptr)
    SWIFT_NAME(__readNumFaceInstancesUnsafe(_:_:_:));
    
    
    FTFace* nullable openFace(const char* nonnull path, long faceIndex = 0, long faceInstanceIndex = 0, CommonError* nullable error = nullptr)
    SWIFT_NAME(__openFaceUnsafe(_:_:_:_:))
    SWIFT_RETURNS_RETAINED;
    
    
    FTGlyphCollection* nullable exportGlyphs(const char* nonnull path, long faceIndex = 0, long faceInstanceIndex = 0, long width = 64, long height = 64, CommonError* nullable error = nullptr, FTProgressCallback nullable progressCallback = nullptr) SWIFT_RETURNS_RETAINED;
    
} FT_REF_INTERFACE(FTLibrary);
