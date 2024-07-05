#pragma once

#include <daisy_seed.h>

class Led
{
public:
    // Call this method before using other members of this class.
    void Init(daisy::DacHandle::Channel channel);

    // Sets LED brightness.
    // 0.0 = off
    // 1.0 = max brightness
    void Set(float brightness);

private:
    daisy::DacHandle::Channel _channel = daisy::DacHandle::Channel::ONE;
    daisy::DacHandle _dac;
};
