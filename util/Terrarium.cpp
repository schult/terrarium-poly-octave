#include "Terrarium.h"

void Terrarium::Init(bool boost)
{
    seed.Init(boost);
    InitKnobs();
    InitToggles();
    InitStomps();
    InitLeds();
}

void Terrarium::Loop(float frequency, std::function<void()> callback)
{
    for (auto& knob : knobs)
    {
        knob.SetSampleRate(frequency);
    }

    const auto interval =
        static_cast<uint32_t>(daisy::System::GetTickFreq() / frequency);
    auto wait_begin = daisy::System::GetTick();
    while (true)
    {
        for (auto& toggle : toggles)
        {
            toggle.Debounce();
        }

        for (auto& stomp : stomps)
        {
            stomp.Debounce();
        }

        callback();

        while ((daisy::System::GetTick() - wait_begin) < interval) {}
        wait_begin += interval;
    }
}

void Terrarium::InitKnobs()
{
    constexpr std::array<daisy::Pin, knob_count> knob_pins{
        daisy::seed::A1,
        daisy::seed::A2,
        daisy::seed::A3,
        daisy::seed::A4,
        daisy::seed::A5,
        daisy::seed::A6,
    };

    std::array<daisy::AdcChannelConfig, knob_count> adc_configs;
    for (int i = 0; i < knob_count; ++i)
    {
        adc_configs[i].InitSingle(knob_pins[i]);
    }

    seed.adc.Init(adc_configs.data(), adc_configs.size());
    seed.adc.Start();

    const auto poll_rate = seed.AudioCallbackRate();
    for (int i = 0; i < knob_count; ++i)
    {
        knobs[i].Init(seed.adc.GetPtr(i), poll_rate);
    }
}

void Terrarium::InitToggles()
{
    constexpr std::array<daisy::Pin, toggle_count> toggle_pins{
        daisy::seed::D10,
        daisy::seed::D9,
        daisy::seed::D8,
        daisy::seed::D7,
    };

    for (int i = 0; i < toggle_count; ++i)
    {
        toggles[i].Init(toggle_pins[i]);
    }
}

void Terrarium::InitStomps()
{
    constexpr std::array<daisy::Pin, stomp_count> stomp_pins{
        daisy::seed::D25,
        daisy::seed::D26,
    };

    for (int i = 0; i < stomp_count; ++i)
    {
        stomps[i].Init(stomp_pins[i]);
    }
}

void Terrarium::InitLeds()
{
    constexpr std::array<daisy::DacHandle::Channel, led_count> led_dacs{
        daisy::DacHandle::Channel::TWO,
        daisy::DacHandle::Channel::ONE,
    };

    for (int i = 0; i < led_count; ++i)
    {
        leds[i].Init(led_dacs[i]);
    }
}
