#ifndef GENERATOR_H
#define GENERATOR_H

#include <cstdint>

/// 'constexpr' allows compile-time calculation if inputs are constant
inline constexpr std::uint32_t GetTileTextureIndex(const int tileNumber, const std::uint32_t seed, const int maxNumber) {
    std::uint32_t mangled = static_cast<std::uint32_t>(tileNumber);

    mangled *= 0xb5297a4d;
    mangled += seed;
    mangled ^= (mangled >> 8);
    mangled += 0x68e31da4;
    mangled ^= (mangled << 8);
    mangled *= 0x1b56c4e9;
    mangled ^= (mangled >> 8);

    return mangled & maxNumber;
}

#endif