#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/typing.hpp"
#endif

#ifdef RAWR_MODULE
    export module rawr.lib.typing;
    import rawr.lib.intrin.base;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/intrin/base.hpp"

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/compiler.pp"

RAWR_EXPORT namespace rawr::inline lib::inline typing::inline base
{
    using intrin::declval;

    using intrin::Is;
    using intrin::bare;

    template <typename T> concept Bare = Is<T, bare<T>>;
    using intrin::Enum;
    using intrin::Class;
    using intrin::Empty;
    using intrin::Union;

    using intrin::DefaultConstructible;
    using intrin::CopyConstructible;
    using intrin::MoveConstructible;
    using intrin::CopyAssignable;
    using intrin::MoveAssignable;
    using intrin::Destructible;
    using intrin::TriviallyDefaultConstructible;
    using intrin::TriviallyCopyConstructible;
    using intrin::TriviallyMoveConstructible;
    using intrin::TriviallyCopyAssignable;
    using intrin::TriviallyMoveAssignable;
    using intrin::TriviallyDestructible;
    using intrin::NoThrowDefaultConstructible;
    using intrin::NoThrowCopyConstructible;
    using intrin::NoThrowMoveConstructible;
    using intrin::NoThrowCopyAssignable;
    using intrin::NoThrowMoveAssignable;
    using intrin::NoThrowDestructible;
    using intrin::TriviallyCopyable;
    using intrin::StandardLayout;
    using intrin::ConvertibleTo;

    namespace trait
    {
        template <typename F, typename Signature>
        struct callable;
        template <typename F, typename R, typename... Args>
        struct callable<F, R(Args...)>
        {
            static constexpr bool value = requires(F&& f, Args&&... args) {
                requires Is< decltype(static_cast<F&&>(f)(static_cast<Args&&>(args)...)), R >;
            };
        };
    }

    template <typename F, typename... Args>
    concept Invocable = requires(F&& f, Args&&... args) {
        static_cast<F&&>(f)(static_cast<Args&&>(args)...);
    };
    template <typename F, typename Signature>
    concept Callable = trait::callable<F, Signature>::value;
}

RAWR_EXPORT namespace rawr::inline lib::inline typing::inline category
{
    // -------------------------------------------------------------------------
    // in<T>: small objects are stored by value; larger objects are borrowed.
    // The observable interface is identical in either case.
    // -------------------------------------------------------------------------

    namespace trait
    {
        template <typename T>
        struct is_in_value_directly_embeddable
        { static constexpr bool value = sizeof(T) < 4; };
    }
    template <Bare T> class in;

    template <Bare T>
    requires (trait::is_in_value_directly_embeddable<T>::value == true)
    class in<T>
    {
        T value_;

    public:
        constexpr in(T const& value) noexcept : value_{value} {}
        constexpr auto operator*() const noexcept -> T const& { return value_; }
        constexpr auto operator->() const noexcept -> T const* { return &value_; }

        static constexpr auto is_in()           { return true; }
        static constexpr auto is_inout()        { return false; }
        static constexpr auto is_relinquished() { return false; }
    };

    template <Bare T>
    requires (trait::is_in_value_directly_embeddable<T>::value == false)
    class in<T>
    {
        T const& value_;

    public:
        constexpr in(T const& value) noexcept : value_{value} {}
        constexpr auto operator*() const noexcept -> T const& { return value_; }
        constexpr auto operator->() const noexcept -> T const* { return &value_; }

        static constexpr auto is_in()           { return true; }
        static constexpr auto is_inout()        { return false; }
        static constexpr auto is_relinquished() { return false; }
    };

    namespace trait
    {
        template <typename T> struct is_in        { static constexpr auto value = false; };
        template <typename T> struct is_in<in<T>> { static constexpr auto value = true;  };
    }
    template <typename T> concept In = trait::is_in<T>::value;

    // -------------------------------------------------------------------------
    // inout<T>: always a mutable borrow.
    // -------------------------------------------------------------------------

    template <Bare T>
    class inout
    {
        T& value_;

    public:
        constexpr inout(T& value) noexcept : value_{value} {}
        constexpr auto operator*() const noexcept -> T& { return value_; }
        constexpr auto operator->() const noexcept -> T* { return &value_; }

        static constexpr auto is_in()           { return false; }
        static constexpr auto is_inout()        { return true; }
        static constexpr auto is_relinquished() { return false; }
    };

    namespace trait
    {
        template <typename T> struct is_inout           { static constexpr auto value = false; };
        template <typename T> struct is_inout<inout<T>> { static constexpr auto value = true;  };
    }
    template <typename T> concept InOut = trait::is_inout<T>::value;


    // -------------------------------------------------------------------------
    // relinquished.
    // -------------------------------------------------------------------------
    // TODO: rename to relinquish to consumable/linear/affine?

    template <Bare T> struct relinquishable;

    template <Bare T>
    class RAWR_CLANG([[clang::trivial_abi]] [[clang::consumable(unconsumed)]]) relinquish_token {
        // Mutable because a const token reference is deliberately consumable.
        mutable T* ptr_;

        explicit constexpr relinquish_token(
            T* p
            RAWR_CLANG([[clang::lifetimebound]])
            RAWR_MSVC([[msvc::lifetimebound]])
            [[gnu::nonnull]]
        ) noexcept : ptr_{p} {}
        friend class relinquishable<T>; // only creator

    public:
        relinquish_token(relinquish_token const&) = delete;
        relinquish_token(relinquish_token&&)      = delete;
        relinquish_token& operator=(relinquish_token const&) = delete;
        relinquish_token& operator=(relinquish_token&&)      = delete;

        RAWR_CLANG([[clang::callable_when(unconsumed), clang::set_typestate(consumed)]])
        [[nodiscard]] constexpr auto consume() const noexcept -> T&& {
            RAWR_ASSERTION(ptr_ != nullptr);
            auto ptr = ptr_;
            ptr_ = nullptr;
            return static_cast<T&&>(*ptr);
        }

        ~relinquish_token() {
            RAWR_ASSERTION(ptr_ == nullptr);
        }

        static constexpr auto is_in()           { return false; }
        static constexpr auto is_inout()        { return false; }
        static constexpr auto is_relinquished() { return true; }
    };

    template <Bare T>
    using relinquished = relinquish_token<T> const&;

    namespace trait
    {
        template <typename T> struct is_relinquished                  { static constexpr auto value = false; };
        template <typename T> struct is_relinquished<relinquished<T>> { static constexpr auto value = true;  };
    }
    template <typename T> concept Relinquished = trait::is_relinquished<T>::value;

    template <Bare T>
    struct relinquishable
    {
        using relinquished = typing::relinquished<T>;

        [[nodiscard]]
        constexpr auto relinquish() & noexcept -> relinquished
        { return relinquish_token<T>{ static_cast<T*>(this) }; }

        auto relinquish() const & = delete;
        auto relinquish() &&      = delete;
    };

    // -------------------------------------------------------------------------
    // Forwarding. Is just shorthand for "any category".
    // -------------------------------------------------------------------------

    template <typename T> concept Forwarding = In<T> || InOut<T> || Relinquished<T>;
}
