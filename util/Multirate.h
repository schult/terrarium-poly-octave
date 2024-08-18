#pragma once

#include <array>
#include <span>

#include <q/utility/ring_buffer.hpp>

//=============================================================================
class Decimator
{
public:
    float operator()(std::span<const float, 4> s)
    {
        buffer1.push(s[0]);
        buffer1.push(s[1]);
        buffer2.push(filter1());

        buffer1.push(s[2]);
        buffer1.push(s[3]);
        buffer2.push(filter1());

        return filter2();
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

    float filter2()
    {
        // Half-band filter
        // 24000 Hz sample rate
        // 0-2000 Hz pass band
        return
            0.00829857 * (buffer2[offset2+0] + buffer2[offset2+10]) +
            -0.05514833 * (buffer2[offset2+2] + buffer2[offset2+8]) +
            0.29690729 * (buffer2[offset2+4] + buffer2[offset2+6]) +
            0.5 * buffer2[offset2+5];
    }

    static constexpr std::size_t bsize1 = 8;
    static constexpr std::size_t fsize1 = 7;
    static constexpr std::size_t offset1 = bsize1 - fsize1;

    static constexpr std::size_t bsize2 = 16;
    static constexpr std::size_t fsize2 = 11;
    static constexpr std::size_t offset2 = bsize2 - fsize2;

    cycfi::q::ring_buffer<float> buffer1{bsize1};
    cycfi::q::ring_buffer<float> buffer2{bsize2};
};


//=============================================================================
class Interpolator
{
public:
    std::array<float, 4> operator()(float s)
    {
        std::array<float, 4> output;

        buffer1.push(s);

        buffer2.push(filter1a());
        output[0] = filter2a();
        output[1] = filter2b();

        buffer2.push(filter1b());
        output[2] = filter2a();
        output[3] = filter2b();

        return output;
    }

private:
    // Filter 1
    // 24000 Hz sample rate
    // 0-3600 Hz pass band (3 dB ripple)
    // 6000-12000 Hz stop band (-80 dB)
    // Gain=2 in passband

    float filter1a()
    {
        return
            -0.0016891753063320908 * (buffer1[offset1+0] + buffer1[offset1+13]) +
            -0.015774477867287424 * (buffer1[offset1+1] + buffer1[offset1+12]) +
            0.01473901938887895 * (buffer1[offset1+2] + buffer1[offset1+11]) +
            0.08691273324484114 * (buffer1[offset1+3] + buffer1[offset1+10]) +
            -0.06596088946209601 * (buffer1[offset1+4] + buffer1[offset1+9]) +
            -0.030808421970123564 * (buffer1[offset1+5] + buffer1[offset1+8]) +
            0.5678435543634612 * (buffer1[offset1+6] + buffer1[offset1+7]);
    }

    float filter1b()
    {
        return
            -0.007650263894572795 * (buffer1[offset1+0] + buffer1[offset1+12]) +
            -0.013417132848789577 * (buffer1[offset1+1] + buffer1[offset1+11]) +
            0.06233747246936775 * (buffer1[offset1+2] + buffer1[offset1+10]) +
            0.040285746814162506 * (buffer1[offset1+3] + buffer1[offset1+9]) +
            -0.13062326696973783 * (buffer1[offset1+4] + buffer1[offset1+8]) +
            0.25076255723029434 * (buffer1[offset1+5] + buffer1[offset1+7]) +
            0.7072160655507663 * buffer1[offset1+6];
    }

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

    static constexpr std::size_t bsize1 = 16;
    static constexpr std::size_t fsize1 = 14;
    static constexpr std::size_t offset1 = bsize1 - fsize1;

    static constexpr std::size_t bsize2 = 16;
    static constexpr std::size_t fsize2 = 9;
    static constexpr std::size_t offset2 = bsize2 - fsize2;

    cycfi::q::ring_buffer<float> buffer1{bsize1};
    cycfi::q::ring_buffer<float> buffer2{bsize2};
};
