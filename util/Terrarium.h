#pragma once

#include <array>
#include <functional>

#include <daisy_seed.h>

#include <util/Led.h>

class Terrarium
{
public:
    // Initializes the Daisy Seed hardware and the Terrarium interface.
    // Call this method before using other members of this class.
    void Init(bool boost = false);

    // Start an infinite loop that executes at the given frequency in hertz.
    // Sets the Terrarium knob sample rates to match the loop frequency.
    // Automatically debounces the Terrarium toggle and stomp switches.
    void Loop(float frequency, std::function<void()> callback);

    daisy::DaisySeed seed;

    static constexpr int knob_count = 6;
    static constexpr int toggle_count = 4;
    static constexpr int stomp_count = 2;
    static constexpr int led_count = 2;

    std::array<daisy::AnalogControl, knob_count> knobs;
    std::array<daisy::Switch, toggle_count> toggles;
    std::array<daisy::Switch, stomp_count> stomps;
    std::array<Led, led_count> leds;

private:
    void InitKnobs();
    void InitToggles();
    void InitStomps();
    void InitLeds();
};
