#pragma once

#include <stdint.h>

// Undefine min and max, because they mess up with the required type interface
// UniformRandomBitGenerator
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

class FNV1a
{
public:
    FNV1a() : m_bytes(0) {}

    using result_type = uint32_t;
    static constexpr result_type min() { return 0U; }
    static constexpr result_type max() { return ~0U; }

    result_type operator()();

    FNV1a& consume(uint64_t bytes) { m_bytes = bytes; return *this; }
private:
    uint64_t m_bytes;
};
