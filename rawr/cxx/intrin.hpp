//// rawr/cxx/intrin.hpp
#pragma region rawr/cxx/intrin.hpp

#ifndef RAWR_UNITY
    #pragma once
    #include "rawr/lib/detection_macros.hpp"
#endif

namespace rawr::cxx::intrin
{
    [[nodiscard]] RAWR_ALWAYS_INLINE constexpr bool is_constexpr() noexcept
    { return __builtin_is_constant_evaluated(); }
}

#pragma endregion rawr/cxx/intrin.hpp
