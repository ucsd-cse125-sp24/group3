#pragma once

#include "shared/utilities/serialize_macro.hpp"

template <typename T>
class Stat {
public:
    Stat(T min, T max, T base):
        _min(min), _max(max), _base(base), _mod(0), _mult(1)
    {
        if (_base < _min) {
            _base = _min; 
        } else if (_base > _max) {
            _base = _max;
        }
    }
    
    T adjustBase(T delta) {
        return _adjust(_base, delta);
    }

    T adjustMod(T delta) {
        return _adjust(_mod, delta);
    }

    T adjustMult(T delta) {
        return _adjust(_mult, delta);
    }

    T current() const {
        return std::max(_min, std::min(_max ,(_base + _mod) * _mult));
    }

    T min() const { return _min; }
    T max() const { return _max; }
    T base() const { return _base; }
    T mod() const { return _mod; }
    T mult() const { return _mult; }

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & _min & _max & _base & _mod & _mult;
    }

private:
    T _min;
    T _max;

    T _base;
    T _mod;
    T _mult;

    T _adjust(T& val, T delta) {
        val += delta; 
        T overflow = 0;
        if (val > _max) {
            overflow = val - _max;
            val = _max;
        } else if (val < _min) {
            overflow = _min - val;
            val = _min;
        }
        return overflow;
    }
};