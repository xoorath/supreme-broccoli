#pragma once

#define xoUnused(x) (void)(x)

//////////////////////////////////////////////////////////////////////////
// Some thoughts on the PIMPL pattern using these macros.
//////////////////////////////////////////////////////////////////////////
// I'm assuming in this comment that the reader is already familiar with 
// the PIMPL pattern and its general pros and cons. I'm not arguing for
// or against the pattern in general here.
//
// About the macros below: xoPimpl and xoPimplImpl are used to create a
// buffer of memory in the owning class, and to use a placement new to
// allocate the opaque class in the same memory space as the owning class.
//
// Advantage to implanting with the macros below:
// 1) Puts the opaque class in the same memory space as the rest of the 
// class.
// 2) A static assert can be used in combination with a template struct
// to give a reasonably user friendly error message to update the header.
// 3) The developers looking at a class get a clear indication of the size
// of the opaque class at a glance.
//
// Disadvantage to implementing with the macros below:
// 1) A bit annoying to update the header every time the opaque class
// changes its memory footprint.
// 2) Some compile time advantages are lost by requiring a header update
// for memory footprint updates. Not all advantages are lost, but enough
// that it makes me question if this is even a worthwhile practice.
//
//////////////////////////////////////////////////////////////////////////

// Usage example:
//      xoPimpl(CoolClass, Impl, 42);
#define xoPimpl(type, name, size) \
    static constexpr uint32 type##_Size = size; \
    __declspec(align(16)) char type##_Buffer[type##_Size]; \
    class type* name;


// Usage example:
//      xoPimplImpl(CoolClass, Impl)(ConstructorParams);
#if !defined(GAME_FINAL)
namespace XO {
template<int s, int t> struct PimplSizeCheck {
    static_assert(s == t, "Pimpl size out of date. Update the header size indicated in xoPimpl macro.");
};
}
#define xoPimplImpl(type, name) \
    { XO::PimplSizeCheck<type##_Size, sizeof(type)> implcheck; \
    xoUnused(implcheck); } \
    name = new(type##_Buffer) type
#else
#define xoPimplImpl(type, name) \
    static_assert(type##_Size == sizeof(type), "Pimpl size out of date. Update the header size indicated in xoPimpl macro.");\
    name = new(type##_Buffer) type
#endif
