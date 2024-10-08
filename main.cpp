#include <cassert>

#include <q/support/literals.hpp>
#include <q/fx/biquad.hpp>

#include <util/EffectState.h>
#include <util/Multirate.h>
#include <util/OctaveGenerator.h>
#include <util/Terrarium.h>

namespace q = cycfi::q;
using namespace q::literals;

Terrarium terrarium;
EffectState interface_state;
bool enable_effect = false;

//=============================================================================
void processAudioBlock(
    daisy::AudioHandle::InputBuffer in,
    daisy::AudioHandle::OutputBuffer out,
    size_t size)
{
    static const auto sample_rate = terrarium.seed.AudioSampleRate();

    static Decimator decimate;
    static Interpolator interpolate;
    static OctaveGenerator octave(sample_rate / resample_factor);
    static q::highshelf eq1(-11, 140_Hz, sample_rate);
    static q::lowshelf eq2(5, 160_Hz, sample_rate);

    const auto& s = interface_state;

    for (size_t i = 0; i <= (size - resample_factor); i += resample_factor)
    {
        std::span<const float, resample_factor> in_chunk(
            &(in[0][i]), resample_factor);
        const auto sample = decimate(in_chunk);

        float octave_mix = 0;
        octave.update(sample);
        octave_mix += s.up1Level() * octave.up1();
        octave_mix += s.down1Level() * octave.down1();
        octave_mix += s.down2Level() * octave.down2();

        auto out_chunk = interpolate(octave_mix);
        for (size_t j = 0; j < out_chunk.size(); ++j)
        {
            float mix = eq2(eq1(out_chunk[j]));

            const auto dry_signal = in[0][i+j];
            mix += s.dryLevel() * dry_signal;

            out[0][i+j] = enable_effect ? mix : dry_signal;
            out[1][i+j] = 0;
        }
    }
}

//=============================================================================
int main()
{
    terrarium.Init(true);
    // These settings are expected by Decimator/Interpolator
    assert(terrarium.seed.AudioSampleRate() == 48000);
    assert(terrarium.seed.AudioBlockSize() % resample_factor == 0);

    auto& knob_dry = terrarium.knobs[0];
    auto& knob_down2 = terrarium.knobs[3];
    auto& knob_down1 = terrarium.knobs[4];
    auto& knob_up1 = terrarium.knobs[5];

    auto& stomp_bypass = terrarium.stomps[0];

    auto& led_enable = terrarium.leds[0];


    terrarium.seed.StartAudio(processAudioBlock);

    terrarium.Loop(100, [&](){
        interface_state.setDryRatio(knob_dry.Process());
        interface_state.setUp1Ratio(knob_up1.Process());
        interface_state.setDown1Ratio(knob_down1.Process());
        interface_state.setDown2Ratio(knob_down2.Process());

        if (stomp_bypass.RisingEdge())
        {
            enable_effect = !enable_effect;
        }

        led_enable.Set(enable_effect ? 1 : 0);
    });
}
