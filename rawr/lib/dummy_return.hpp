#pragma region rawr/lib/dummy_return.hpp

namespace rawr::inline lib
{
    struct dummy_return {
        template <typename T> operator T() const noexcept;
    };
}

#pragma endregion rawr/lib/dummy_return.hpp
