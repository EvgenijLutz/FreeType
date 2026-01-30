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

#ifndef fn_packed
#define fn_packed __attribute__((packed))
#endif

#ifndef fn_noescape
#define fn_noescape _LIBCPP_NOESCAPE
#endif

#ifndef fn_lifetimebound
#define fn_lifetimebound _LIBCPP_LIFETIMEBOUND
#endif


// MARK: Exposing C++ structs or classes to Swift as Swift classes

#ifndef FN_SWIFT_INTERFACE
/// Exposes C++ `struct` or `class` to Swift as a reference (Switf's `class`) type.
///
/// This macro tells the compiler that there are two functions that Swift will use to manage lifecycle of instances of this class:
/// ```cxx
/// class Foo {
///     // ...
/// } FN_SWIFT_INTERFACE(Foo);
/// ```
/// In this example, the ``FN_SWIFT_INTERFACE`` macro tells that Swift will use `FooRetain` and `FooRelease` functions to manage the lifecycle of instances of the `Foo` class.
///
/// You can implement these functions either by yourself or by using the ``FN_IMPLEMENT_SWIFT_INTERFACE1`` macro if your class complies this macro's requirements:
/// ```cxx
/// // a) Either implement manually both FooRetain and FooRelease functions
/// Foo* fn_nullable FooRetain(Foo* fn_nullable instance) {
///     if (instance) {
///         // Increment the reference count
///     }
///     return instance;
/// }
/// void FooRelease(Foo* fn_nullable instance) {
///     if (instance) {
///         // Decrement the reference count
///         // Destroy the instance if the reference count is 0
///     }
/// }
///
/// // b) Or use a macro
/// FN_IMPLEMENT_SWIFT_INTERFACE1(Foo)
/// ```
///
/// - Seealso: ``FN_IMPLEMENT_SWIFT_INTERFACE1`` macro for more details.
#define FN_SWIFT_INTERFACE(name) SWIFT_SHARED_REFERENCE(name##Retain, name##Release)
#endif



#ifndef FN_IMPLEMENT_SWIFT_INTERFACE1
/// Implements retain/release functions for a C++ class exposed to Swift earlier with the ``FN_DEFINE_SWIFT_INTERFACE`` macro.
///
/// The bridging class thoud contain a reference counter defined as:
/// ```cxx
/// class Foo {
///     // ...
///     std::atomic<size_t> _referenceCounter;
///     // ...
/// } FN_SWIFT_INTERFACE(Foo);
/// ```
///
/// If the `_referenceCounter` variable is a private field, you can make the retain and release functions as friends to this class by including the ``FN_FRIEND_SWIFT_INTERFACE`` macro in class' declaration:
/// ```cxx
/// class Foo {
///     // ...
///     FN_FRIEND_SWIFT_INTERFACE(Foo)
///     // ...
/// } FN_SWIFT_INTERFACE(Foo);
/// ```
///
/// If you want these retain and release functions to be publicly exposed, you can definde them globally somewhere in a header file using the ``FN_DEFINE_SWIFT_INTERFACE`` macro:
/// ```cxx
/// FN_DEFINE_SWIFT_INTERFACE(Foo)
/// ```
///
/// - Seealso: ``FN_FRIEND_SWIFT_INTERFACE`` and ``FN_DEFINE_SWIFT_INTERFACE``.
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



#ifndef FN_FRIEND_SWIFT_INTERFACE
/// Defines friend release and retain functions used by the ``FN_IMPLEMENT_SWIFT_INTERFACE1`` macro:
/// ```cxx
/// class Foo {
///     // ...
///     FN_FRIEND_SWIFT_INTERFACE(Foo)
///     // ...
/// } FN_SWIFT_INTERFACE(Foo);
/// ```
#define FN_FRIEND_SWIFT_INTERFACE(name) \
friend name* fn_nullable name##Retain(name* fn_nullable obj) SWIFT_RETURNS_UNRETAINED; \
friend void name##Release(name* fn_nullable obj);
#endif



#ifndef FN_DEFINE_SWIFT_INTERFACE
/// Exposes retain and release function interfaces to manage object lifetime.
#define FN_DEFINE_SWIFT_INTERFACE(name) \
name* fn_nullable name##Retain(name* fn_nullable obj) SWIFT_RETURNS_UNRETAINED; \
void name##Release(name* fn_nullable obj);
#endif
