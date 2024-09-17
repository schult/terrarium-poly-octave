#pragma once

#include <array>
#include <span>

#include <q/utility/ring_buffer.hpp>

constexpr size_t resample_factor = 3;

//=============================================================================
class Decimator
{
public:
    float operator()(std::span<const float, resample_factor> s)
    {
        buffer1.push(s[0]);
        buffer1.push(s[1]);
        buffer1.push(s[2]);
        return filter1();
    }

private:
    float filter1()
    {
        // 48000 Hz sample rate
        // 0-1800 Hz pass band (3 dB ripple)
        // 8000-24000 Hz stop band (-80 dB)
        return
            0.000066177472224418f * (buffer1[offset1+0] + buffer1[offset1+20]) +
            0.0009613901552378511f * (buffer1[offset1+1] + buffer1[offset1+19]) +
            0.003835090815380887f * (buffer1[offset1+2] + buffer1[offset1+18]) +
            0.010496532623165526f * (buffer1[offset1+3] + buffer1[offset1+17]) +
            0.02272703591356282f * (buffer1[offset1+4] + buffer1[offset1+16]) +
            0.041464390530886956f * (buffer1[offset1+5] + buffer1[offset1+15]) +
            0.06591039391505207f * (buffer1[offset1+6] + buffer1[offset1+14]) +
            0.09309984953947406f * (buffer1[offset1+7] + buffer1[offset1+13]) +
            0.11829177835273737f * (buffer1[offset1+8] + buffer1[offset1+12]) +
            0.13620590247679107f * (buffer1[offset1+9] + buffer1[offset1+11]) +
            0.14270010010002276f * buffer1[offset1+10];
    }

    static constexpr std::size_t bsize1 = 32;
    static constexpr std::size_t fsize1 = 21;
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
        output[2] = filter2c();

        return output;
    }

private:
    // Filter 2
    // 48000 Hz sample rate
    // 0-3600 Hz pass band (3 dB ripple)
    // 8000-24000 Hz stop band (-80 dB)
    // Gain=3 in passband

    float filter2a()
    {
        return
            0.00036440608905813593f * buffer2[offset2+0] +
            0.0005821260464558225f * buffer2[offset2+1] +
            -0.043244023722481956f * buffer2[offset2+2] +
            -0.10310036386076359f * buffer2[offset2+3] +
            0.13604229993913602f * buffer2[offset2+4] +
            0.5503466630244301f * buffer2[offset2+5] +
            0.4407091552750118f * buffer2[offset2+6] +
            0.009420000864297772f * buffer2[offset2+7] +
            -0.09801301258361905f * buffer2[offset2+8] +
            -0.019627176246818184f * buffer2[offset2+9] +
            0.001762424830497545f * buffer2[offset2+10];
    }

    float filter2b()
    {
        return
            0.001112114188613258f * (buffer2[offset2+0] + buffer2[offset2+10]) +
            -0.005449383064836152f * (buffer2[offset2+1] + buffer2[offset2+9]) +
            -0.07276547446584428f * (buffer2[offset2+2] + buffer2[offset2+8]) +
            -0.0709695783332148f * (buffer2[offset2+3] + buffer2[offset2+7]) +
            0.2904591843823435f * (buffer2[offset2+4] + buffer2[offset2+6]) +
            0.590541634315722f * buffer2[offset2+5];
    }

    float filter2c()
    {
        return
            0.001762424830497545f * buffer2[offset2+0] +
            -0.019627176246818184f * buffer2[offset2+1] +
            -0.09801301258361905f * buffer2[offset2+2] +
            0.009420000864297772f * buffer2[offset2+3] +
            0.4407091552750118f * buffer2[offset2+4] +
            0.5503466630244301f * buffer2[offset2+5] +
            0.13604229993913602f * buffer2[offset2+6] +
            -0.10310036386076359f * buffer2[offset2+7] +
            -0.043244023722481956f * buffer2[offset2+8] +
            0.0005821260464558225f * buffer2[offset2+9] +
            0.00036440608905813593f * buffer2[offset2+10];
    }

    static constexpr std::size_t bsize2 = 16;
    static constexpr std::size_t fsize2 = 11;
    static constexpr std::size_t offset2 = bsize2 - fsize2;

    cycfi::q::ring_buffer<float> buffer2{bsize2};
};
