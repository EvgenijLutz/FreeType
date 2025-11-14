//
//  Common.hpp
//  CommonHeaders
//
//  Created by Evgenij Lutz on 11.11.25.
//

#pragma once

#include <stdio.h>
#include <atomic>
#include <span>
#include <vector>
#include <string>

#if __has_include(<swift/bridging>)
#  include <swift/bridging>
#else
// from <swift/bridging>
#  define SWIFT_SELF_CONTAINED
#  define SWIFT_RETURNS_INDEPENDENT_VALUE
#  define SWIFT_SHARED_REFERENCE(_retain, _release)
#  define SWIFT_IMMORTAL_REFERENCE
#  define SWIFT_UNSAFE_REFERENCE
#  define SWIFT_NAME(_name)
#  define SWIFT_CONFORMS_TO_PROTOCOL(_moduleName_protocolName)
#  define SWIFT_COMPUTED_PROPERTY
#  define SWIFT_MUTATING
#  define SWIFT_UNCHECKED_SENDABLE
#  define SWIFT_NONCOPYABLE
#  define SWIFT_NONESCAPABLE
#  define SWIFT_ESCAPABLE
#  define SWIFT_ESCAPABLE_IF(...)
#  define SWIFT_RETURNS_RETAINED
#  define SWIFT_RETURNS_UNRETAINED
#  define SWIFT_PRIVATE_FILEID(_fileID)
#endif


#ifndef fn_nullable
#define fn_nullable __nullable
#endif

#ifndef fn_nonnull
#define fn_nonnull __nonnull
#endif

#ifndef fn_noescape
#define fn_noescape _LIBCPP_NOESCAPE
#endif

#ifndef fn_lifetimebound
#define fn_lifetimebound _LIBCPP_LIFETIMEBOUND
#endif


#ifndef FN_FRIEND_SWIFT_INTERFACE
#define FN_FRIEND_SWIFT_INTERFACE(name) \
friend name* fn_nullable name##Retain(name* fn_nullable obj) SWIFT_RETURNS_UNRETAINED; \
friend void name##Release(name* fn_nullable obj);
#endif


#ifndef FN_SWIFT_INTERFACE
#define FN_SWIFT_INTERFACE(name) SWIFT_SHARED_REFERENCE(name##Retain, name##Release)
#endif


#ifndef FN_DEFINE_SWIFT_INTERFACE
#define FN_DEFINE_SWIFT_INTERFACE(name) \
name* fn_nullable name##Retain(name* fn_nullable obj) SWIFT_RETURNS_UNRETAINED; \
void name##Release(name* fn_nullable obj);
#endif


#ifndef FN_IMPLEMENT_SWIFT_INTERFACE1
#define FN_IMPLEMENT_SWIFT_INTERFACE1(name) \
name* fn_nullable name##Retain(name* fn_nullable obj) { \
    if (obj == nullptr) { \
        return nullptr; \
    } \
    obj->_referenceCounter.fetch_add(1); \
    return obj; \
} \
void name##Release(name* fn_nullable obj) { \
    if (obj == nullptr) { \
        return; \
    } \
    if (obj->_referenceCounter.fetch_sub(1) == 1) { \
        delete obj; \
    } \
}
#endif
