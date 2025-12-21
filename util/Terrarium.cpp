#include <util/Terrarium.h>
#include <hw_config.h>
#include <cmath>
#include <cstring>
#include <vector>
#include <sstream>

extern I2S_HandleTypeDef hi2s2;
extern int16_t txBuf[];
extern int16_t rxBuf[];

// Global pointer to the user's audio callback
static void (*UserAudioCallback)(float**, float**, size_t) = nullptr;

// Interleaved buffers for the callback
static float in_interleaved[AUDIO_BLOCK_SIZE * 2];
static float out_interleaved[AUDIO_BLOCK_SIZE * 2];

// Pointers for the callback (De-interleaved format expected by Daisy/Terrarium logic)
static float in_ch0[AUDIO_BLOCK_SIZE];
static float in_ch1[AUDIO_BLOCK_SIZE];
static float out_ch0[AUDIO_BLOCK_SIZE];
static float out_ch1[AUDIO_BLOCK_SIZE];
static float* in_ptr[2] = {in_ch0, in_ch1};
static float* out_ptr[2] = {out_ch0, out_ch1};

//-----------------------------------------------------------------------------
// The actual DMA Half/Full Transfer callbacks call this
void Audio_Update(int16_t* rx, int16_t* tx, size_t size_half_words)
{
    if (!UserAudioCallback) return;

    size_t num_samples = size_half_words / 2; // Stereo samples

    // Convert Int16 to Float and De-interleave
    for (size_t i = 0; i < num_samples; ++i)
    {
        in_ch0[i] = (float)rx[i*2] / 32768.0f;
        in_ch1[i] = (float)rx[i*2+1] / 32768.0f;
    }

    // Call the processing logic
    UserAudioCallback(in_ptr, out_ptr, num_samples);

    // Interleave and Convert Float to Int16
    for (size_t i = 0; i < num_samples; ++i)
    {
        // Clipping
        float l = out_ch0[i];
        float r = out_ch1[i];
        if(l > 1.0f) l = 1.0f; else if(l < -1.0f) l = -1.0f;
        if(r > 1.0f) r = 1.0f; else if(r < -1.0f) r = -1.0f;

        tx[i*2]   = (int16_t)(l * 32767.0f);
        tx[i*2+1] = (int16_t)(r * 32767.0f);
    }
}

// STM32 HAL Callbacks
extern "C" {
    // With HAL_I2SEx_TransmitReceive_DMA, the callback is usually TxCpltCallback
    // when the transaction is complete (RX and TX move together).

    void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
        if (hi2s->Instance == SPI2) {
            Audio_Update(&rxBuf[0], &txBuf[0], DMA_BUFFER_SIZE);
        }
    }
    void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
        if (hi2s->Instance == SPI2) {
            Audio_Update(&rxBuf[DMA_BUFFER_SIZE], &txBuf[DMA_BUFFER_SIZE], DMA_BUFFER_SIZE);
        }
    }
}

//-----------------------------------------------------------------------------
void Terrarium::Init(bool boost)
{
    (void)boost;

    // Set default values
    for(auto& k : knobs) k.Set(0.5f);
    for(auto& s : toggles) s.Set(false);
    for(auto& s : stomps) s.Set(false);
}

void Terrarium::Seed::StartAudio(void (*cb)(float**, float**, size_t))
{
    UserAudioCallback = cb;

    // Start DMA using the Extended Full Duplex function
    // This allows simultaneous TX and RX on the I2S bus.
    HAL_I2SEx_TransmitReceive_DMA(&hi2s2, (uint16_t*)txBuf, (uint16_t*)rxBuf, DMA_BUFFER_SIZE * 2);
}

void Terrarium::Loop(float frequency, std::function<void()> callback)
{
    uint32_t period_ms = (uint32_t)(1000.0f / frequency);
    while(1)
    {
        callback();
        HAL_Delay(period_ms);
    }
}

void Terrarium::ProcessCommand(const std::string& cmd)
{
    std::stringstream ss(cmd);
    std::string type;
    int index;
    float value;

    ss >> type >> index >> value;

    if (type == "knob" && index >= 0 && index < knob_count) {
        knobs[index].Set(value);
    } else if (type == "stomp" && index >= 0 && index < stomp_count) {
        stomps[index].Set(value > 0.5f);
    } else if (type == "toggle" && index >= 0 && index < toggle_count) {
        toggles[index].Set(value > 0.5f);
    }
}
