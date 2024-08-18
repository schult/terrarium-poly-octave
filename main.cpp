#include <q/support/literals.hpp>

#include <util/Multirate.h>
#include <util/Terrarium.h>

namespace q = cycfi::q;
using namespace q::literals;

Terrarium terrarium;
bool enable_effect = false;

//=============================================================================
void processAudioBlock(
    daisy::AudioHandle::InputBuffer in,
    daisy::AudioHandle::OutputBuffer out,
    size_t size)
{
    static Decimator decimate;
    static Interpolator interpolate;

    constexpr size_t chunk_size = 4;
    for (size_t i = 0; i <= (size - chunk_size); i += chunk_size)
    {
        std::span<const float, chunk_size> in_chunk(&(in[0][i]), chunk_size);
        const auto s = decimate(in_chunk);

        const auto mix = s;  // TODO

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

    auto& stomp_bypass = terrarium.stomps[0];

    auto& led_enable = terrarium.leds[0];


    terrarium.seed.StartAudio(processAudioBlock);

    terrarium.Loop(100, [&](){
        if (stomp_bypass.RisingEdge())
        {
            enable_effect = !enable_effect;
        }

        led_enable.Set(enable_effect ? 1 : 0);
    });
}
