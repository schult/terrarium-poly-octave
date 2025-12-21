#pragma once

#include <array>
#include <functional>
#include <string>

// Replaces daisy::AnalogControl
struct VirtualKnob {
    float value = 0.0f;
    float Process() { return value; }
    void Set(float v) { value = v; }
};

// Replaces daisy::Switch
struct VirtualSwitch {
    bool state = false;
    bool changed = false;

    // Simulate RisingEdge detection
    bool RisingEdge() {
        bool rose = state && changed;
        changed = false; // Reset changed flag after reading
        return rose;
    }

    void Set(bool s) {
        if (state != s) changed = true;
        state = s;
    }
};

// Replaces daisy::Led
struct VirtualLed {
    void Set(float val) { /* Can be sent over USB if needed */ }
};

// Mock Audio Handle structures to match Daisy API signature
namespace daisy {
    struct AudioHandle {
        using InputBuffer = float**;
        using OutputBuffer = float**;
    };
}

class Terrarium
{
public:
    void Init(bool boost = false);
    void Loop(float frequency, std::function<void()> callback);

    // Mocking the 'seed' object from Daisy
    struct Seed {
        float AudioSampleRate() const { return 48000.0f; }
        size_t AudioBlockSize() const { return 48; } // Matching AUDIO_BLOCK_SIZE

        // This registers the callback
        void StartAudio(void (*cb)(float**, float**, size_t));
    } seed;

    static constexpr int knob_count = 6;
    static constexpr int toggle_count = 4;
    static constexpr int stomp_count = 2;
    static constexpr int led_count = 2;

    std::array<VirtualKnob, knob_count> knobs;
    std::array<VirtualSwitch, toggle_count> toggles;
    std::array<VirtualSwitch, stomp_count> stomps;
    std::array<VirtualLed, led_count> leds;

    // Helper for command parsing
    void ProcessCommand(const std::string& cmd);
};
