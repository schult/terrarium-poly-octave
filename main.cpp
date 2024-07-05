#include <q/support/literals.hpp>

#include <util/Terrarium.h>

namespace q = cycfi::q;
using namespace q::literals;

Terrarium terrarium;
bool enable_effect = false;

void processAudioBlock(
    daisy::AudioHandle::InputBuffer in,
    daisy::AudioHandle::OutputBuffer out,
    size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        const auto dry_signal = in[0][i];

        const auto mix = 0;  // TODO
        out[0][i] = enable_effect ? mix : dry_signal;
        out[1][i] = 0;
    }
}

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
