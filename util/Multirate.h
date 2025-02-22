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
        // 0-10000 Hz pass band (3 dB ripple)
        // -60 dB in stop band
        return
            0.00083861 * (buffer1[offset1+0] + buffer1[offset1+42]) +
            -0.00170401 * (buffer1[offset1+2] + buffer1[offset1+40]) +
            0.00332071 * (buffer1[offset1+4] + buffer1[offset1+38]) +
            -0.00583481 * (buffer1[offset1+6] + buffer1[offset1+36]) +
            0.00957465 * (buffer1[offset1+8] + buffer1[offset1+34]) +
            -0.01502259 * (buffer1[offset1+10] + buffer1[offset1+32]) +
            0.02299861 * (buffer1[offset1+12] + buffer1[offset1+30]) +
            -0.03518379 * (buffer1[offset1+14] + buffer1[offset1+28]) +
            0.05592924 * (buffer1[offset1+16] + buffer1[offset1+26]) +
            -0.10130299 * (buffer1[offset1+18] + buffer1[offset1+24]) +
            0.31668232 * (buffer1[offset1+20] + buffer1[offset1+22]) +
            0.5 * buffer1[offset1+21];
    }

    static constexpr std::size_t bsize1 = 64;
    static constexpr std::size_t fsize1 = 43;
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
    // 0-10000 Hz pass band (5 dB ripple)
    // 12000-24000 Hz stop band (-40 dB)
    // Gain=2 in passband

    float filter_a()
    {
        return
            -0.01660065602747542 * (buffer2[offset2+0] + buffer2[offset2+15]) +
            -0.10092145713132415 * (buffer2[offset2+1] + buffer2[offset2+14]) +
            0.024696002957939184 * (buffer2[offset2+2] + buffer2[offset2+13]) +
            -0.00017576862171295502 * (buffer2[offset2+3] + buffer2[offset2+12]) +
            -0.02861407663966942 * (buffer2[offset2+4] + buffer2[offset2+11]) +
            0.07798643163226901 * (buffer2[offset2+5] + buffer2[offset2+10]) +
            -0.18036935076211502 * (buffer2[offset2+6] + buffer2[offset2+9]) +
            0.6256712589010108 * (buffer2[offset2+7] + buffer2[offset2+8]);
    }

    float filter_b()
    {
        return
            -0.06759330637934204 * (buffer2[offset2+0] + buffer2[offset2+14]) +
            -0.0660346567846777 * (buffer2[offset2+1] + buffer2[offset2+13]) +
            0.06516031623939957 * (buffer2[offset2+2] + buffer2[offset2+12]) +
            -0.07511759733734813 * (buffer2[offset2+3] + buffer2[offset2+11]) +
            0.0888378327759846 * (buffer2[offset2+4] + buffer2[offset2+10]) +
            -0.10142513890648383 * (buffer2[offset2+5] + buffer2[offset2+9]) +
            0.1101721921260483 * (buffer2[offset2+6] + buffer2[offset2+8]) +
            0.8868271703659392 * buffer2[offset2+7];
    }

    static constexpr std::size_t bsize2 = 16;
    static constexpr std::size_t fsize2 = 13;
    static constexpr std::size_t offset2 = bsize2 - fsize2;

    cycfi::q::ring_buffer<float> buffer2{bsize2};
};
