#pragma once

#include <bit>
#include <cmath>
#include <complex>
#include <cstdint>
#include <limits>
#include <numbers>

#include <gcem.hpp>
#include <q/detail/fast_math.hpp>

//=============================================================================
constexpr float centerFreq(const int n)
{
    return 480 * gcem::pow(2.0f, (0.027f * n)) - 420;
}

//=============================================================================
constexpr float bandwidth(const int n)
{
    const float f0 = centerFreq(n-1);
    const float f1 = centerFreq(n);
    const float f2 = centerFreq(n+1);
    const float a = (f2 - f1);
    const float b = (f1 - f0);
    return (4/3.0f) * (a*b) / (a+b);
}

//=============================================================================
// Prototype filter is LPF from "Cookbook formulae for audio EQ biquad filter
// coefficients", a.k.a. "Audio EQ Cookbook", by Robert Bristow-Johnson
// https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html
//
// Transformed as described in Section 3.1 of "Complex Band-Pass Filters for
// Analytic Signal Generation and Their Application" by Andrew J. Noga
// https://apps.dtic.mil/sti/tr/pdf/ADA395963.pdf
class BandShifter
{
public:
    BandShifter() = default;

    BandShifter(float center, float sample_rate, float bw)
        : _down1_w(center * -0.5f * (2 * pi_f / sample_rate))
        , _down2_w(center * -0.75f * (2 * pi_f / sample_rate))
    {
        constexpr auto pi_d = std::numbers::pi_v<double>;
        constexpr auto j = std::complex<double>(0, 1);

        const auto w0 = pi_d * bw / sample_rate;
        const auto cos_w0 = std::cos(w0);
        const auto sin_w0 = std::sin(w0);
        const auto sqrt_2 = std::sqrt(2.0);
        const auto a0 = (1 + sqrt_2 * sin_w0 / 2);
        const auto g = (1 - cos_w0) / (2 * a0);

        const auto w1 = 2 * pi_d * center / sample_rate;
        const auto e1 = std::exp(j * w1);
        const auto e2 = std::exp(j * w1 * 2.0);

        const auto d0 = g;
        const auto d1 = e1 * 2.0 * g;
        const auto d2 = e2 * g;
        const auto c1 = e1 * (-2 * cos_w0) / a0;
        const auto c2 = e2 * (1 - sqrt_2 * sin_w0 / 2) / a0;

        _d0 = d0;
        _d1 = std::complex<float>(d1.real(), d1.imag());
        _d2 = std::complex<float>(d2.real(), d2.imag());
        _c1 = std::complex<float>(c1.real(), c1.imag());
        _c2 = std::complex<float>(c2.real(), c2.imag());
    }

    void update(float sample)
    {
        _y = _s2 + _d0*sample;
        _s2 = _s1 + _d1*sample - _c1*_y;
        _s1 = _d2*sample - _c2*_y;
    }

    float up1() const {
        const auto a = _y.real();
        const auto b = _y.imag();
        return (a*a - b*b) * invSqrt(a*a + b*b);
    }

    float down1(float idx) const {
        // (_y * std::exp(j * _down1_w * idx)).real()
        const auto x = _down1_w * idx;
        const auto a = _y.real();
        const auto b = _y.imag();
        const auto c = fastercosfull(x);
        const auto d = fastersinfull(x);
        return (a*c) - (b*d);
    }

    float down2(float idx) const {
        // (_y * std::exp(j * _down2_w * idx)).real()
        const auto x = _down2_w * idx;
        const auto a = _y.real();
        const auto b = _y.imag();
        const auto c = fastercosfull(x);
        const auto d = fastersinfull(x);
        return (a*c) - (b*d);
    }

private:
    // https://en.wikipedia.org/wiki/Fast_inverse_square_root
    static constexpr float invSqrt(float x) noexcept
    {
        static_assert(std::numeric_limits<float>::is_iec559);
        float const y = std::bit_cast<float>(
                0x5F1FFFF9 - (std::bit_cast<std::uint32_t>(x) >> 1));
        return y * (0.703952253f * (2.38924456f - (x * y * y)));
    }

    static constexpr auto pi_f = std::numbers::pi_v<float>;

    const float _down1_w;
    const float _down2_w;

    float _d0 = 0;
    std::complex<float> _d1;
    std::complex<float> _d2;
    std::complex<float> _c1;
    std::complex<float> _c2;

    std::complex<float> _s1;
    std::complex<float> _s2;

    std::complex<float> _y;
};
