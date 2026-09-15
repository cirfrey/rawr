#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/intrin/construct_at.hpp"
#endif
// In this file you will witness evil, you will also witness great hacky engineering,
// magic enchantations, forbidden runes and funny comments.
// I don't think I need to tell you this whole file is UB by standard, but works
// on a per-compiler basis.
// Behold.

#ifdef RAWR_MODULE
    export module rawr.lib.intrin.construct_at;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/detection.pp"
#include "rawr/lib/compiler.pp"

namespace rawr::inline lib::construct_at_detail
{
    // What this is about to be used for defies all that is good in the world, therefore
    // you must explictly request it's invocation unto this plane.
    struct unholy_override_tag { explicit constexpr unholy_override_tag() = default; };
}

// Clang treats anything that kinda smells like std::construct_at with an internal
// constexpr blessing. std::rawr::construct_at apparently applies for old clang.
// This is all sorts of nasty but i kinda like it.
#if RAWR_COMPILER_CLANG
    void* operator new(decltype(sizeof(0)), void* p) noexcept;

    namespace std::rawr
    {
        template<class T, class... Args>
        constexpr T* construct_at(T* p, Args&&... args)
        {
            return ::new (const_cast<void*>(static_cast<const volatile void*>(p)))
                T(static_cast<Args&&>(args)...);
        }
    }
#endif

// GCC is a little stricter, it needs to be spelled exactly std::construct_at.
// ...
// They don't check the args.
// ...
// This is even nastier, we are overloading the std::construct_at with a tag-dispatched
// version. There is no God here.
//
// Don't think this is the last you'll see of this technique, old foes usually come
// back in stranger and more bizarre forms.
#if RAWR_COMPILER_GCC
    void* operator new(decltype(sizeof(0)), void* p) noexcept;

    namespace std
    {
        template <class T, class... Args>
        constexpr T* construct_at(
            [[maybe_unused]] rawr::lib::construct_at_detail::unholy_override_tag Tag,
            T* ptr,
            Args&&... args
        )
        { return ::new (const_cast<void*>(static_cast<const volatile void*>(ptr))) T(static_cast<Args&&>(args)...); }
    }
#endif

// You know the drill by now.
// ...
// Except not quite
// ...
// GCC and Clang don't need the tag in operator new since placement new is also blessed there.
// Plus we don't actually define operator new, just declare it - so no ODR issues.
//
// Since we need to define operator new in MSVC with the blessing, we inject the tag
// to avoid confusion with the one declared in <new>.
//
// These days MSVC's construct_at is just a plain old function that calls it with the holy [[msvc::constexpr]]
// attribute bestowed upon us mortals in MSVC 19.33 (VS17.3).
#if RAWR_COMPILER_MSVC && _MSC_VER >= 1933
    [[nodiscard]]
    [[msvc::constexpr]]
    inline void* __cdecl operator new(
        decltype(sizeof(void*)),
        rawr::lib::construct_at_detail::unholy_override_tag,
        void* p
    ) noexcept { return p; }
#endif

// And just when you've though you've seen it all
// ...
// Older MSVC can *also* be supported, but it requires yet *another* hack, as MSVCs old blessing
// mechanism properly checks namespaces, function names and also checks parameters.
// ...
// I'm sure you can guess what's coming next: Tag dispatching via template parameters!
#if RAWR_COMPILER_MSVC && _MSC_VER < 1933
    namespace std
    {
        template <
            rawr::lib::construct_at_detail::unholy_override_tag Tag,
            typename T,
            typename... Args
        >
        constexpr T* construct_at(T* const ptr, Args&&... args)
        { return ::new (const_cast<void*>(static_cast<const volatile void*>(ptr))) T(args...); }
    }
#endif

RAWR_EXPORT namespace rawr::inline lib::intrin
{
    template <typename T, typename... Args>
    constexpr T* construct_at(T* ptr, Args&&... args)
        noexcept(noexcept(T(static_cast<Args&&>(args)...)))
    {
        #if RAWR_COMPILER_GCC
            return std::construct_at(
                rawr::lib::construct_at_detail::unholy_override_tag{},
                ptr,
                static_cast<Args&&>(args)...
            );
        #elif RAWR_COMPILER_CLANG
            return std::rawr::construct_at(ptr, static_cast<Args&&>(args)...);
        #elif RAWR_COMPILER_MSVC && _MSC_VER >= 1933
            [[msvc::constexpr]] return ::new (
                rawr::lib::construct_at_detail::unholy_override_tag{},
                static_cast<void*>(ptr)
            ) T(static_cast<Args&&>(args)...);
        #elif RAWR_COMPILER_MSVC && _MSC_VER < 1933
            return std::construct_at<
                rawr::lib::construct_at_detail::unholy_override_tag{}
            >(
                ptr,
                static_cast<Args&&>(args)...
            );
        #endif
    }
}
