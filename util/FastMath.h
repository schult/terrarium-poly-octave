#pragma once

#include <bit>
#include <cstdint>
#include <limits>

// https://en.wikipedia.org/wiki/Fast_inverse_square_root
static constexpr float fastInvSqrt(float x) noexcept
{
    static_assert(std::numeric_limits<float>::is_iec559);
    float const y = std::bit_cast<float>(
            0x5F1FFFF9 - (std::bit_cast<std::uint32_t>(x) >> 1));
    return y * (0.703952253f * (2.38924456f - (x * y * y)));
}

static constexpr float fastSqrt(float x)
{
    return fastInvSqrt(x) * x;
}

// gist.github.com/volkansalma/2972237#gistcomment-3872525
static constexpr float fastAtan2(float y, float x)
{
    constexpr float pi = std::numbers::pi_v<float>;
    constexpr float half_pi = pi / 2.0f;
    constexpr float quarter_pi = pi / 4.0f;

    const float ay = std::abs(y) + std::numeric_limits<float>::epsilon();
    const float ax = std::abs(x);
    const float r = (x - std::copysign(ay, x)) / (ay + ax);
    const float th = half_pi - std::copysign(quarter_pi, x) - (quarter_pi * r);
    return std::copysign(th, y);
}
