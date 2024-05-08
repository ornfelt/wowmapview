#ifndef _DYNTREE_H
#define _DYNTREE_H

#include "Define.h"

namespace G3D
{
    class Ray;
    class Vector3;
}

namespace VMAP
{
    struct AreaAndLiquidData;
    enum class ModelIgnoreFlags : uint32;
}

class GameObjectModel;
struct DynTreeImpl;

class DynamicMapTree
{
    DynTreeImpl* impl;

public:
    DynamicMapTree();
    ~DynamicMapTree();

    [[nodiscard]] bool isInLineOfSight(float x1, float y1, float z1, float x2, float y2, float z2, uint32 phasemask, VMAP::ModelIgnoreFlags ignoreFlags) const;

    bool GetIntersectionTime(uint32 phasemask, const G3D::Ray& ray, const G3D::Vector3& endPos, float& maxDist) const;

    bool GetAreaInfo(float x, float y, float& z, uint32 phasemask, uint32& flags, int32& adtId, int32& rootId, int32& groupId) const;
    void GetAreaAndLiquidData(float x, float y, float z, uint32 phasemask, uint8 reqLiquidType, VMAP::AreaAndLiquidData& data) const;

    bool GetObjectHitPos(uint32 phasemask, const G3D::Vector3& pPos1,
                         const G3D::Vector3& pPos2, G3D::Vector3& pResultHitPos,
                         float pModifyDist) const;

    [[nodiscard]] float getHeight(float x, float y, float z, float maxSearchDist, uint32 phasemask) const;

    void insert(const GameObjectModel&);
    void remove(const GameObjectModel&);
    [[nodiscard]] bool contains(const GameObjectModel&) const;
    [[nodiscard]] int size() const;

    void balance();
    void update(uint32 diff);
};

#endif // _DYNTREE_H
