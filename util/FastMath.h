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

// https://www.dsprelated.com/showarticle/1052.php
static constexpr float fastAtan2(float y, float x)
{
    constexpr float pi = std::numbers::pi_v<float>;
    constexpr float half_pi = pi / 2.0f;

    if (x == 0) return (y > 0) ? half_pi : -half_pi;

    const float ay = std::abs(y);
    const float ax = std::abs(x);
    const bool invert = ay > ax;
    const float z = invert ? ax/ay : ay/ax;

    float th = (-0.19194795f * z * z + 0.97239411f) * z;

    if (invert) { th = half_pi - th; }
    if (x < 0) { th = pi - th; }
    if (y < 0) { th = -th; }
    return th;
}
