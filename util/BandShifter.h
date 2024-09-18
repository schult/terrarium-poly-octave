#pragma once

#include <cmath>
#include <complex>
#include <numbers>

#include <util/FastMath.h>

//=============================================================================
class BandShifter
{
public:
    BandShifter() = default;

    BandShifter(float center, float sample_rate, float bw)
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

    void setScale(float scale)
    {
        _scale = scale;
        _offset_step = pi2 * _scale;
    }

    float operator()(float sample)
    {
        update_filter(sample);
        update_shifted();
        return _shifted;
    }

private:
    // gist.github.com/volkansalma/2972237#gistcomment-3872525
    static constexpr float fastAtan2(float y, float x)
    {
        constexpr float half_pi = pi / 2.0f;
        constexpr float quarter_pi = pi / 4.0f;

        const float ay = std::abs(y) + std::numeric_limits<float>::epsilon();
        const float ax = std::abs(x);
        const float r = (x - std::copysign(ay, x)) / (ay + ax);
        const float th = half_pi - std::copysign(quarter_pi, x) - (quarter_pi * r);
        return std::copysign(th, y);
    }

    // bmtechjournal.wordpress.com/2020/05/27/super-fast-quadratic-sinusoid-approximation/
    static constexpr float fastSine(float x)
    {
        const auto z = 2 * (x - std::floor(x) - 0.5f);
        return 4 * z * (1 - std::abs(z));
    }

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

    // Pitch shifts are performed via phase scaling, described in "Real-Time
    // Polyphonic Octave Doubling for the Guitar" by Etienne Thuillier
    // https://core.ac.uk/download/pdf/80719011.pdf
    //
    // In order to support an arbitrary scale factor, we have to actually
    // determine the phase and scale it, rather than using the shortcut
    // identified in the paper.
    void update_shifted()
    {
        const auto mag = fastSqrt(std::norm(_y));
        const auto phase_in = fastAtan2(_y.imag(), _y.real());
        const auto phase_out = _scale * phase_in + _phase_offset;
        _shifted = mag * fastSine(phase_out);
    }

    static constexpr float pi = 0.5f; // To be compatible with fastSine
    static constexpr float pi2 = 2 * pi;
    float _scale = 0.749153538438341f;
    float _offset_step = pi2 * _scale;

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
