# Terrarium Poly Octave

This is firmware for a polyphonic octave shift effect pedal. It runs on an
[Electro-Smith Daisy Seed](https://www.electro-smith.com/daisy/daisy) mounted
in a [PedalPCB Terrarium](https://www.pedalpcb.com/product/pcb351/).

## Building

    cmake \
        -GNinja \
        -DTOOLCHAIN_PREFIX=/path/to/toolchain \
        -DCMAKE_TOOLCHAIN_FILE=lib/libDaisy/cmake/toolchains/stm32h750xx.cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -B build .
    cmake --build build
