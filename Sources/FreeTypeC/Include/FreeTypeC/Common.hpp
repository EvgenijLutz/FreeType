//
//  Common.hpp
//  FreeType
//
//  Created by Evgenij Lutz on 08.09.25.
//

#ifndef FreeTypeC_Common_hpp
#define FreeTypeC_Common_hpp

#if defined __cplusplus

#include <swift/bridging>
#include <vector>
#include <string>
#include <atomic>


#if !defined nullable
#define nullable __nullable
#endif

#if !defined nonnull
#define nonnull __nonnull
#endif


#if defined FREETYPE_H_
#define FT_Library_impl FT_Library
#define FT_Face_impl FT_Face
#else
#define FT_Library_impl void*
#define FT_Face_impl void*
#endif // FREETYPE_H_


#define FT_REF_FRIEND_INTERFACE(type) \
friend type* nonnull type##Retain(type* nonnull obj) SWIFT_RETURNS_UNRETAINED; \
friend void type##Release(type* nonnull ojb);

#define FT_REF_INTERFACE(type) \
SWIFT_SHARED_REFERENCE(type##Retain, type##Release)

#define FT_REF_INTERFACE_IMPL(type) \
type* nonnull type##Retain(type* nonnull obj) { \
    if (obj) { \
        obj->_referenceCounter.fetch_add(1); \
    } \
    return obj; \
} \
void type##Release(type* nonnull obj) { \
    if (obj && obj->_referenceCounter.fetch_sub(1) == 1) { \
        delete obj; \
    } \
}


#endif

#endif // FreeTypeC_Common_hpp
