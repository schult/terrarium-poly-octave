#pragma once

#include <algorithm>

class LinearRamp
{
public:
    LinearRamp(float value, float step) : _value(value), _step(step) {}

    float operator()(float target)
    {
        if (_value > target)
        {
            _value -= _step;
            _value = std::max(_value, target);
        }
        else if (_value < target)
        {
            _value += _step;
            _value = std::min(_value, target);
        }

        return _value;
    }

private:
    float _value;
    float _step;
};
