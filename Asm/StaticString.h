#pragma once
#include <utility>

constexpr char charToLower(const char c) {
    return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
}

template<std::size_t N>
class Static_string
{
private:
    const char m_str[N + 1];
public:
    template<typename T, T ... nums>
    constexpr Static_string(const char (&str)[N], std::integer_sequence<T, nums ...>) :
        m_str{ charToLower(str[nums])..., 0 }
    {}

    constexpr char operator[] (std::size_t index)
    {
        return m_str[i];
    }

    operator const char*() const
    {
        return m_str;
    }
};

template <std::size_t N>
constexpr Static_string<N> toLower(const char(&str)[N])
{
    return { str, std::make_integer_sequence<unsigned, N>() };
}