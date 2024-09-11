#pragma once

#include <cmath>
#include <complex>
#include <numbers>

#include <dsp/fast_math_functions.h>
#include <q/detail/fast_math.hpp>

#include <util/FastSqrt.h>

//=============================================================================
class BandShifter
{
public:
    BandShifter() = default;

    BandShifter(float center, float sample_rate, float bw)
    {
        constexpr auto pi = std::numbers::pi_v<double>;
        constexpr auto j = std::complex<double>(0, 1);

        const auto w0 = pi * bw / sample_rate;
        const auto cos_w0 = std::cos(w0);
        const auto sin_w0 = std::sin(w0);
        const auto sqrt_2 = std::sqrt(2.0);
        const auto a0 = (1 + sqrt_2 * sin_w0 / 2);
        const auto g = (1 - cos_w0) / (2 * a0);

        const auto w1 = 2 * pi * center / sample_rate;
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

    float operator()(float sample)
    {
        update_filter(sample);
        update_shifted();
        return _shifted;
    }

private:
    // Prototype filter is LPF from "Cookbook formulae for audio EQ biquad
    // filter coefficients", a.k.a. "Audio EQ Cookbook",
    // by Robert Bristow-Johnson
    // https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html
    //
    // Transformed as described in Section 3.1 of "Complex Band-Pass Filters
    // for Analytic Signal Generation and Their Application" by Andrew J. Noga
    // https://apps.dtic.mil/sti/tr/pdf/ADA395963.pdf

    void update_filter(float sample)
    {
        const auto prev_imag_sign = std::signbit(_y.imag());

        _y = _s2 + _d0*sample;
        _s2 = _s1 + _d1*sample - _c1*_y;
        _s1 = _d2*sample - _c2*_y;

        if ((_y.real() < 0) && (std::signbit(_y.imag()) != prev_imag_sign))
        {
            _phase_offset += _offset_step;
            while (_phase_offset > pi2)
            {
                _phase_offset -= pi2;
            }
        }
    }

    void update_shifted()
    {
        const auto mag = fastSqrt(std::norm(_y));
        float phase_in;
        arm_atan2_f32(_y.imag(), _y.real(), &phase_in);
        const auto phase_out = _scale * phase_in + _phase_offset;
        _shifted = mag * fastersinfull(phase_out);
    }

    static constexpr float pi2 = 2.0f * std::numbers::pi_v<float>;
    static constexpr float _scale = 0.749153538438341f; // TODO: Make adjustable
    static constexpr float _offset_step = pi2 * _scale;

    float _d0 = 0;
    std::complex<float> _d1;
    std::complex<float> _d2;
    std::complex<float> _c1;
    std::complex<float> _c2;

    std::complex<float> _s1;
    std::complex<float> _s2;

    std::complex<float> _y;
    float _phase_offset = std::numbers::pi_v<float> / 2;
    float _shifted = 0;
};
