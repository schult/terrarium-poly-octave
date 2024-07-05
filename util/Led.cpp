#include "Led.h"

void Led::Init(daisy::DacHandle::Channel channel)
{
    _channel = channel;
    daisy::DacHandle::Config config;
    config.target_samplerate = 0;
    config.chn = _channel;
    config.mode = daisy::DacHandle::Mode::POLLING;
    config.bitdepth = daisy::DacHandle::BitDepth::BITS_12;
    config.buff_state = daisy::DacHandle::BufferState::ENABLED;
    _dac.Init(config);
}

void Led::Set(float brightness)
{
    if (brightness > 0)
    {
        brightness += 1;
        brightness /= 2;
    }
    _dac.WriteValue(_channel, static_cast<uint16_t>(brightness * 4095));
}
