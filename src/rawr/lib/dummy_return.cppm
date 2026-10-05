export module rawr.lib.dummy_return;

export namespace rawr::inline lib
{
    struct dummy_return {
        template <typename T> operator T() const noexcept;
    };
}
