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
        return filter1();
    }

private:
    float filter1()
    {
        // Half-band filter
        // 48000 Hz sample rate
        // 0-2000 Hz pass band
        return
            -0.03290583 * (buffer1[offset1+0] + buffer1[offset1+6]) +
            0.28285046 * (buffer1[offset1+2] + buffer1[offset1+4]) +
            0.5 * buffer1[offset1+3];
    }

    static constexpr std::size_t bsize1 = 8;
    static constexpr std::size_t fsize1 = 7;
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
        output[0] = filter2a();
        output[1] = filter2b();

        return output;
    }

private:
    // Filter 2
    // 48000 Hz sample rate
    // 0-3600 Hz pass band (3 dB ripple)
    // 12000-24000 Hz stop band (-79 dB)
    // Gain=2 in passband

    float filter2a()
    {
        return
            -0.0019270988947841026 * (buffer2[offset2+0] + buffer2[offset2+8]) +
            -0.018056143186882867 * (buffer2[offset2+1] + buffer2[offset2+7]) +
            0.03355581609645265 * (buffer2[offset2+2] + buffer2[offset2+6]) +
            0.300268057392673 * (buffer2[offset2+3] + buffer2[offset2+5]) +
            0.5012732307948162 * buffer2[offset2+4];
    }

    float filter2b()
    {
        return
            -0.008704460313655491 * (buffer2[offset2+0] + buffer2[offset2+7]) +
            -0.013602379992118592 * (buffer2[offset2+1] + buffer2[offset2+6]) +
            0.1437455836391944 * (buffer2[offset2+2] + buffer2[offset2+5]) +
            0.4430919218647344 * (buffer2[offset2+3] + buffer2[offset2+4]);
    }

    static constexpr std::size_t bsize2 = 16;
    static constexpr std::size_t fsize2 = 9;
    static constexpr std::size_t offset2 = bsize2 - fsize2;

    cycfi::q::ring_buffer<float> buffer2{bsize2};
};
