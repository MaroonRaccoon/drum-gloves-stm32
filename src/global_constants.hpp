#pragma once

#include <cstdint>

// ---- status codes ----
namespace status
{
// Invalid channel selected when setting up DAC
const uint32_t dac_invalid_channel = 1;
// V_REFBUF not ready when setting up DAC
const uint32_t dac_not_ready = 2;
const uint32_t ok = 3;
const uint32_t dac_dma_underrun = 4;
}

namespace constants
{
    const uint32_t sample_rate = 32000; // 32 kHz
    const uint32_t clock_frequency = 16'000'000; // 16 MHz
    const int sample_count = 3;
    const int adc_window_length = 128;
}
