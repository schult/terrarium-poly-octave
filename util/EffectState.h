#pragma once

#include <util/Mapping.h>

class EffectState
{
public:
    void setDryRatio(float r) { _dry_ratio = r; }
    void setShiftedRatio(float r) { _shifted_ratio = r; }
    void setPitchRatio(float r) { _pitch_ratio = r; }

    float dryLevel() const { return volume_mapping(_dry_ratio); }
    float shiftedLevel() const { return volume_mapping(_shifted_ratio); }

    float pitchScale() const
    {
        return 1 / std::pow(2.0f, 1.0 - _pitch_ratio);
    }

private:
    static constexpr LogMapping volume_mapping{0, 1, 20};

    static constexpr float ratio_min = 0.0;
    static constexpr float ratio_max = 1.0;

    float _dry_ratio = ratio_min;
    float _shifted_ratio = ratio_min;
    float _pitch_ratio = ratio_min;
};
