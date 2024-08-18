#include <cassert>

#include <q/support/literals.hpp>

#include <util/BandShifter.h>
#include <util/Multirate.h>
#include <util/Terrarium.h>

namespace q = cycfi::q;
using namespace q::literals;

Terrarium terrarium;
std::vector<BandShifter> shifters;
bool enable_effect = false;

//=============================================================================
void processAudioBlock(
    daisy::AudioHandle::InputBuffer in,
    daisy::AudioHandle::OutputBuffer out,
    size_t size)
{
    static Decimator decimate;
    static Interpolator interpolate;

    static unsigned int idx = 0;

    constexpr size_t chunk_size = 4;
    for (size_t i = 0; i <= (size - chunk_size); i += chunk_size)
    {
        std::span<const float, chunk_size> in_chunk(&(in[0][i]), chunk_size);
        const auto s = decimate(in_chunk);

        float mix = 0;
        for (auto& shifter : shifters)
        {
            shifter.update(s);
            mix += shifter.up1();
            mix += shifter.down1((float)idx);
            mix += shifter.down2((float)idx);
        }
        ++idx;

        const auto out_chunk = interpolate(mix);
        for (size_t j = 0; j < out_chunk.size(); ++j)
        {
            const auto dry_signal = in[0][i+j];
            out[0][i+j] = enable_effect ? out_chunk[j] : dry_signal;
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
    assert(terrarium.seed.AudioBlockSize() % 4 == 0);

    auto& stomp_bypass = terrarium.stomps[0];

    auto& led_enable = terrarium.leds[0];


    const auto sample_rate = terrarium.seed.AudioSampleRate() / 4;
    for (int i = 0; i < 80; ++i)
    {
        const auto center = centerFreq(i);
        const auto bw = bandwidth(i);
        shifters.emplace_back(BandShifter(center, sample_rate, bw));
    }


    terrarium.seed.StartAudio(processAudioBlock);

    terrarium.Loop(100, [&](){
        if (stomp_bypass.RisingEdge())
        {
            enable_effect = !enable_effect;
        }

        led_enable.Set(enable_effect ? 1 : 0);
    });
}
