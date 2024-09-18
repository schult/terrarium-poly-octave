#pragma once

#include <array>
#include <span>

#include <q/utility/ring_buffer.hpp>

constexpr size_t resample_factor = 2;

//=============================================================================
class Decimator
{
public:
    float operator()(std::span<const float, resample_factor> s)
    {
        buffer1.push(s[0]);
        buffer1.push(s[1]);
        return filter();
    }

private:
    float filter()
    {
        // Half-band filter
        // 48000 Hz sample rate
        // 0-8000 Hz pass band (3 dB ripple)
        // -60 dB in stop band
        return
            -0.0014897 * (buffer1[offset1+0] + buffer1[offset1+22]) +
            0.00598166 * (buffer1[offset1+2] + buffer1[offset1+20]) +
            -0.01663127 * (buffer1[offset1+4] + buffer1[offset1+18]) +
            0.03891337 * (buffer1[offset1+6] + buffer1[offset1+16]) +
            -0.08926107 * (buffer1[offset1+8] + buffer1[offset1+14]) +
            0.31232524 * (buffer1[offset1+10] + buffer1[offset1+12]) +
            0.5 * buffer1[offset1+11];
    }

    static constexpr std::size_t bsize1 = 32;
    static constexpr std::size_t fsize1 = 23;
    static constexpr std::size_t offset1 = bsize1 - fsize1;

    cycfi::q::ring_buffer<float> buffer1{bsize1};
};


//=============================================================================
class Interpolator
{
public:
    std::array<float, resample_factor> operator()(float s)
    {
        std::array<float, resample_factor> output;

        buffer2.push(s);
        output[0] = filter_a();
        output[1] = filter_b();

        return output;
    }

private:
    // 48000 Hz sample rate
    // 0-8000 Hz pass band (3 dB ripple)
    // 12000-24000 Hz stop band (-60 dB)
    // Gain=2 in passband

    float filter_a()
    {
        return
            -0.005862192139473832 * (buffer2[offset2+0] + buffer2[offset2+12]) +
            -0.05517156594209455 * (buffer2[offset2+1] + buffer2[offset2+11]) +
            -0.009719556794562321 * (buffer2[offset2+2] + buffer2[offset2+10]) +
            0.0687893911088757 * (buffer2[offset2+3] + buffer2[offset2+9]) +
            -0.14410290445787172 * (buffer2[offset2+4] + buffer2[offset2+8]) +
            0.20899356763944632 * (buffer2[offset2+5] + buffer2[offset2+7]) +
            0.7653977205510119 * buffer2[offset2+6];
    }

    float filter_b()
    {
        return
            -0.026907019867822866 * (buffer2[offset2+0] + buffer2[offset2+11]) +
            -0.058743370771268094 * (buffer2[offset2+1] + buffer2[offset2+10]) +
            0.06113487074569069 * (buffer2[offset2+2] + buffer2[offset2+9]) +
            -0.03083050178098881 * (buffer2[offset2+3] + buffer2[offset2+8]) +
            -0.08966376487780182 * (buffer2[offset2+4] + buffer2[offset2+7]) +
            0.5902325721646465 * (buffer2[offset2+5] + buffer2[offset2+6]);
    }

    static constexpr std::size_t bsize2 = 16;
    static constexpr std::size_t fsize2 = 13;
    static constexpr std::size_t offset2 = bsize2 - fsize2;

    cycfi::q::ring_buffer<float> buffer2{bsize2};
};
