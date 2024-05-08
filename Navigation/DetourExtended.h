#ifndef _ACORE_DETOUR_EXTENDED_H
#define _ACORE_DETOUR_EXTENDED_H

#include "DetourNavMeshQuery.h"

class dtQueryFilterExt: public dtQueryFilter
{
public:
    float getCost(const float* pa, const float* pb,
        const dtPolyRef prevRef, const dtMeshTile* prevTile, const dtPoly* prevPoly,
        const dtPolyRef curRef, const dtMeshTile* curTile, const dtPoly* curPoly,
        const dtPolyRef nextRef, const dtMeshTile* nextTile, const dtPoly* nextPoly) const override;
};

#endif // _ACORE_DETOUR_EXTENDED_H
