// WorldLocation.h
#ifndef WORLDLOCATION_H
#define WORLDLOCATION_H

#include "Position.h"

// For uint32...

// Using typedef (C++98 and later):
#include <cstdint>
//typedef std::uint32_t uint32;
// Using using (C++11 and later):
using uint32 = std::uint32_t;

constexpr uint32 MAPID_INVALID = 0xFFFFFFFF;  // Placeholder for an invalid map ID

class WorldLocation : public Position {
public:
    uint32 m_mapId;

    explicit WorldLocation(uint32 _mapId = MAPID_INVALID, float x = 0.f, float y = 0.f, float z = 0.f, float o = 0.f)
        : Position(x, y, z, o), m_mapId(_mapId) {}

    WorldLocation(uint32 mapId, Position const& position)
        : Position(position), m_mapId(mapId) {}
};

#endif // WORLDLOCATION_H
