#include "Hash.hpp"

constexpr uint64_t FNVPrime64   = 0x00000100000001b3ULL;
constexpr uint64_t FNVOffset64  = 0xcbf29ce484222325ULL;

uint32_t FNV1a::operator()()
{
    uint64_t hash = FNVOffset64;
    for (uint32_t i = 0; i < 8; i++)
    {
        hash ^= (m_bytes & 0x00000000000000ffULL);
        hash *= FNVPrime64;
        m_bytes >>= 8;
    }
    return static_cast<uint32_t>((hash >> 32) ^ (hash & 0x00000000ffffffff));
}
