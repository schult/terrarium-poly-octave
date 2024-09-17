#pragma once

#include <util/BandShifter.h>

#include <gcem.hpp>

//=============================================================================
class PitchShifter
{
public:
    PitchShifter(float sample_rate)
    {
        for (int i = 0; i < 110; ++i)
        {
            const auto center = centerFreq(i);
            const auto bw = bandwidth(i);
            _shifters.emplace_back(center, sample_rate, bw);
        }
    }

    void setScale(float scale)
    {
        for (auto& shifter : _shifters)
        {
            shifter.setScale(scale);
        }
    }

    float operator()(float sample)
    {
        float shifted = 0;
        for (auto& shifter : _shifters)
        {
            shifted += shifter(sample);
        }
        return shifted;
    }

private:
    static constexpr float centerFreq(const int n)
    {
        return 480 * gcem::pow(2.0f, (n / 27.0f)) - 420;
    }

    static constexpr float bandwidth(const int n)
    {
        const float f0 = centerFreq(n-1);
        const float f1 = centerFreq(n);
        const float f2 = centerFreq(n+1);
        const float a = (f2 - f1);
        const float b = (f1 - f0);
        return 2.0f * (a*b) / (a+b);
    }

    std::vector<BandShifter> _shifters;
};
