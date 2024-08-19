#pragma once

#include <util/Mapping.h>

class EffectState
{
public:
    void setDryRatio(float r) { _dry_ratio = r; }
    void setUp1Ratio(float r) { _up1_ratio = r; }
    void setDown1Ratio(float r) { _down1_ratio = r; }
    void setDown2Ratio(float r) { _down2_ratio = r; }

    float dryLevel() const { return volume_mapping(_dry_ratio); }
    float up1Level() const { return volume_mapping(_up1_ratio); }
    float down1Level() const { return volume_mapping(_down1_ratio); }
    float down2Level() const { return volume_mapping(_down2_ratio); }

private:
    static constexpr LogMapping volume_mapping{0, 1, 20};

    static constexpr float ratio_min = 0.0;
    static constexpr float ratio_max = 1.0;

    float _dry_ratio = ratio_min;
    float _up1_ratio = ratio_min;
    float _down1_ratio = ratio_min;
    float _down2_ratio = ratio_min;
};
