#ifndef _OBJECT_H
#define _OBJECT_H

#include "Common.h"
#include "DataMap.h"
#include "G3D/Vector3.h"
#include "GridDefines.h"
#include "GridReference.h"
#include "Map.h"
#include "ModelIgnoreFlags.h"
#include "ObjectDefines.h"
#include "Optional.h"
#include "Position.h"
#include "UpdateData.h"
#include "UpdateMask.h"
#include <set>
#include <sstream>
#include <string>

template<class T>
class GridObject
{
public:
    [[nodiscard]] bool IsInGrid() const { return _gridRef.isValid(); }
    //void AddToGrid(GridRefMgr<T>& m) { ASSERT(!IsInGrid()); _gridRef.link(&m, (T*)this); }
    void AddToGrid(GridRefMgr<T>& m) { _gridRef.link(&m, (T*)this); }
    //void RemoveFromGrid() { ASSERT(IsInGrid()); _gridRef.unlink(); }
    void RemoveFromGrid() { _gridRef.unlink(); }
private:
    GridReference<T> _gridRef;
};

class Object
{
public:
    virtual ~Object() {}
    Object() {};

union
{
	int32* m_int32Values;
	uint32* m_uint32Values;
	float* m_floatValues;
};

protected:
    uint16 m_objectType;

    uint16 m_updateFlag;

    uint16 _fieldNotifyFlags;

    bool m_objectUpdated;

private:
    bool m_inWorld;
};

class WorldObject : public Object, public WorldLocation
{
protected:
    //explicit WorldObject(bool isWorldObject); //note: here it means if it is in grid object list or world object list
public:
    //~WorldObject() override;
    //~WorldObject();
    WorldObject(bool isWorldObject = false) 
        : WorldLocation(), m_isWorldObject(isWorldObject) {}

    virtual ~WorldObject() override {}

	//WorldObject(uint32 mapId, bool isWorldObject = false)
	//	: Object(), WorldLocation(mapId), isWorldObject(isWorldObject) {
	//	std::cout << "WorldObject constructed with mapId " << mapId << " and isWorldObject " << isWorldObject << "\n";
	//}

	[[nodiscard]] uint32 GetUInt32Value(uint16 index) const
	{
		return m_uint32Values[index];
	}

    uint16 m_valuesCount;

	[[nodiscard]] bool HasFlag(uint16 index, uint32 flag) const
	{
		//if (index >= m_valuesCount && !PrintIndexError(index, false))
		if (index >= m_valuesCount)
		{
			return false;
		}

		return (m_uint32Values[index] & flag) != 0;
	}

    //Player* ToPlayer() { if (GetTypeId() == TYPEID_PLAYER) return reinterpret_cast<Player*>(this); else return nullptr; }

    //Unit* ToUnit() { if (GetTypeId() == TYPEID_UNIT || GetTypeId() == TYPEID_PLAYER) return reinterpret_cast<Unit*>(this); else return nullptr; }
    //[[nodiscard]] Unit const* ToUnit() const { if (GetTypeId() == TYPEID_UNIT || GetTypeId() == TYPEID_PLAYER) return (Unit const*)((Unit*)this); else return nullptr; }
    //Creature* ToCreature() { if (GetTypeId() == TYPEID_UNIT) return reinterpret_cast<Creature*>(this); else return nullptr; }
    //[[nodiscard]] Creature const* ToCreature() const { if (GetTypeId() == TYPEID_UNIT) return (Creature const*)((Creature*)this); else return nullptr; }
    Unit* ToUnit() { return reinterpret_cast<Unit*>(this); }
    [[nodiscard]] Unit const* ToUnit() const { return (Unit const*)((Unit*)this); }
    Creature* ToCreature() { return reinterpret_cast<Creature*>(this); }
    [[nodiscard]] Creature const* ToCreature() const { return (Creature const*)((Creature*)this); }

    void GetNearPoint2D(WorldObject const* searcher, float& x, float& y, float distance, float absAngle, Position const* startPos = nullptr) const;
    void GetNearPoint2D(float& x, float& y, float distance, float absAngle, Position const* startPos = nullptr) const;
    void GetNearPoint(WorldObject const* searcher, float& x, float& y, float& z, float searcher_size, float distance2d, float absAngle, float controlZ = 0, Position const* startPos = nullptr) const;
    void GetVoidClosePoint(float& x, float& y, float& z, float size, float distance2d = 0, float relAngle = 0, float controlZ = 0) const;
    bool GetClosePoint(float& x, float& y, float& z, float size, float distance2d = 0, float angle = 0, WorldObject const* forWho = nullptr, bool force = false) const;
    void MovePosition(Position& pos, float dist, float angle);
    Position GetNearPosition(float dist, float angle);
    void MovePositionToFirstCollision(Position& pos, float dist, float angle) const;
    Position GetFirstCollisionPosition(float startX, float startY, float startZ, float destX, float destY);
    Position GetFirstCollisionPosition(float destX, float destY, float destZ);
    Position GetFirstCollisionPosition(float dist, float angle) const;
    Position GetRandomNearPosition(float radius);

    void GetContactPoint(WorldObject const* obj, float& x, float& y, float& z, float distance2d = CONTACT_DISTANCE) const;
    void GetChargeContactPoint(WorldObject const* obj, float& x, float& y, float& z, float distance2d = CONTACT_DISTANCE) const;

    [[nodiscard]] float GetObjectSize() const { return m_objectSize; }

    [[nodiscard]] virtual float GetCombatReach() const { return 0.0f; } // overridden (only) in Unit
    void UpdateGroundPositionZ(float x, float y, float& z) const;
    //void UpdateAllowedPositionZ(float x, float y, float& z, float* groundZ = nullptr) const;

    // TODO: configurable?
	[[nodiscard]] bool CanFly()  const { return false; }
    [[nodiscard]] float GetHoverHeight() const { return m_hoverHeight; }

    mutable float newPositionZ;
    void SetPositionZNew(float value) const { newPositionZ = value; }
    float GetPositionZNew() const { return newPositionZ; }

	float GetMapHeight(float x, float y, float z, bool vmap = true, float distanceToSearch = DEFAULT_HEIGHT_SEARCH) const
	{
		if (z != MAX_HEIGHT)
			z += std::max(GetCollisionHeight(), Z_OFFSET_FIND_HEIGHT);

		return GetMap()->GetHeight(GetPhaseMask(), x, y, z, vmap, distanceToSearch);
	}

	float GetMapWaterOrGroundLevel(float x, float y, float z, float* ground/* = nullptr*/) const
	{
		//return GetMap()->GetWaterOrGroundLevel(GetPhaseMask(), x, y, z, ground,
		//	isType(TYPEMASK_UNIT) ? !static_cast<Unit const*>(this)->HasAuraType(SPELL_AURA_WATER_WALK) : false,
		//	std::max(GetCollisionHeight(), Z_OFFSET_FIND_HEIGHT));
		return GetMap()->GetWaterOrGroundLevel(GetPhaseMask(), x, y, z, ground,
			false,
			std::max(GetCollisionHeight(), Z_OFFSET_FIND_HEIGHT));
	}

    //void UpdateAllowedPositionZ(float x, float y, float& z, float* groundZ = nullptr) const
    //{
	//	float ground_z = GetMapHeight(x, y, z);
	//	if (ground_z > INVALID_HEIGHT)
	//		z = ground_z;

	//	if (groundZ)
	//		*groundZ = ground_z;
	//}

	void UpdateAllowedPositionZ(float x, float y, float& z, float* groundZ = nullptr) const
	{
		//if (GetTransport())
		//{
		//	if (groundZ)
		//		*groundZ = z;

		//	return;
		//}

		//if (Unit const* unit = ToUnit())
		//{
		//	if (!unit->CanFly())
			if (!CanFly())
			{
				//Creature const* c = unit->ToCreature();
				//bool canSwim = c ? c->CanSwim() : true;
                bool canSwim = true;
				float ground_z = z;
				float max_z;
				if (canSwim)
					max_z = GetMapWaterOrGroundLevel(x, y, z, &ground_z);
				else
					max_z = ground_z = GetMapHeight(x, y, z);

				if (max_z > INVALID_HEIGHT)
				{
					//if (canSwim && unit->GetMap()->IsInWater(unit->GetPhaseMask(), x, y, max_z - Z_OFFSET_FIND_HEIGHT, unit->GetCollisionHeight()))
					if (canSwim && GetMap()->IsInWater(GetPhaseMask(), x, y, max_z - Z_OFFSET_FIND_HEIGHT, GetCollisionHeight()))
					{
						// do not allow creatures to walk on
						// water level while swimming
						max_z = std::max(max_z - GetMinHeightInWater(), ground_z);
					}
					else
					{
						// hovering units cannot go below their hover height
						//float hoverOffset = unit->GetHoverHeight();
						float hoverOffset = GetHoverHeight();
						max_z += hoverOffset;
						ground_z += hoverOffset;
					}

					if (z > max_z)
						z = max_z;
					else if (z < ground_z)
						z = ground_z;
				}

				if (groundZ)
					*groundZ = ground_z;
			}
			else
			{
				//float ground_z = GetMapHeight(x, y, z) + unit->GetHoverHeight();
				float ground_z = GetMapHeight(x, y, z) + GetHoverHeight();
				if (z < ground_z)
					z = ground_z;

				if (groundZ)
					*groundZ = ground_z;
			}
		//}
		//else
		//{
		//	float ground_z = GetMapHeight(x, y, z);
		//	if (ground_z > INVALID_HEIGHT)
		//		z = ground_z;

		//	if (groundZ)
		//		*groundZ = ground_z;
		//}
	}

	[[nodiscard]] float GetFloatValue(uint16 index) const
	{
		return m_floatValues[index];
	}

    void GetRandomPoint(const Position& srcPos, float distance, float& rand_x, float& rand_y, float& rand_z) const;
    [[nodiscard]] Position GetRandomPoint(const Position& srcPos, float distance) const;

    [[nodiscard]] uint32 GetInstanceId() const { return m_InstanceId; }

    // TODO?
    [[nodiscard]] uint32 GetPhaseMask() const { return m_phaseMask; }
    //[[nodiscard]] uint32 GetPhaseMask() const { return 0; }
    bool InSamePhase(WorldObject const* obj) const { return InSamePhase(obj->GetPhaseMask()); }
    [[nodiscard]] bool InSamePhase(uint32 phasemask) const { return m_useCombinedPhases ? GetPhaseMask() & phasemask : GetPhaseMask() == phasemask; }

    [[nodiscard]] std::string const& GetName() const { return m_name; }
    void SetName(std::string const& newname) { m_name = newname; }

    [[nodiscard]] virtual std::string const& GetNameForLocaleIdx(LocaleConstant /*locale_idx*/) const { return m_name; }

    float GetDistance(WorldObject const* obj) const;
    [[nodiscard]] float GetDistance(const Position& pos) const;
    [[nodiscard]] float GetDistance(float x, float y, float z) const;
    float GetDistance2d(WorldObject const* obj) const;
    [[nodiscard]] float GetDistance2d(float x, float y) const;
    float GetDistanceZ(WorldObject const* obj) const;

    bool IsSelfOrInSameMap(WorldObject const* obj) const;
    bool IsInMap(WorldObject const* obj) const;
    [[nodiscard]] bool IsWithinDist3d(float x, float y, float z, float dist) const;
    bool IsWithinDist3d(const Position* pos, float dist) const;
    [[nodiscard]] bool IsWithinDist2d(float x, float y, float dist) const;
    bool IsWithinDist2d(const Position* pos, float dist) const;
    // use only if you will sure about placing both object at same map
    bool IsWithinDist(WorldObject const* obj, float dist2compare, bool is3D = true, bool useBoundingRadius = true) const;
    bool IsWithinDistInMap(WorldObject const* obj, float dist2compare, bool is3D = true, bool useBoundingRadius = true) const;
    [[nodiscard]] Position GetHitSpherePointFor(Position const& dest, Optional<float> collisionHeight = { }, Optional<float> combatReach = { }) const;
    void GetHitSpherePointFor(Position const& dest, float& x, float& y, float& z, Optional<float> collisionHeight = { }, Optional<float> combatReach = { }) const;
    bool GetDistanceOrder(WorldObject const* obj1, WorldObject const* obj2, bool is3D = true) const;

    //[[nodiscard]] Map* GetMap() const { ASSERT(m_currMap); return m_currMap; }
    [[nodiscard]] Map* GetMap() const { return m_currMap; }
    [[nodiscard]] Map* FindMap() const { return m_currMap; }
    //used to check all object's GetMap() calls when object is not in world!
    [[nodiscard]] void SetMap(Map* map) { m_currMap = map; }

#ifdef MAP_BASED_RAND_GEN
    int32 irand(int32 min, int32 max) const     { return int32 (GetMap()->mtRand.randInt(max - min)) + min; }
    uint32 urand(uint32 min, uint32 max) const  { return GetMap()->mtRand.randInt(max - min) + min;}
    int32 rand32() const                        { return GetMap()->mtRand.randInt();}
    double rand_norm() const                    { return GetMap()->mtRand.randExc();}
    double rand_chance() const                  { return GetMap()->mtRand.randExc(100.0);}
#endif

    //[[nodiscard]] virtual float GetStationaryX() const { return GetPositionX(); }
    //[[nodiscard]] virtual float GetStationaryY() const { return GetPositionY(); }
    //[[nodiscard]] virtual float GetStationaryZ() const { return GetPositionZ(); }
    //[[nodiscard]] virtual float GetStationaryO() const { return GetOrientation(); }

    //[[nodiscard]] float GetMapWaterOrGroundLevel(float x, float y, float z, float* ground = nullptr) const;
    //[[nodiscard]] float GetMapHeight(float x, float y, float z, bool vmap = true, float distanceToSearch = 50.0f) const; // DEFAULT_HEIGHT_SEARCH in map.h

    [[nodiscard]] float GetFloorZ() const;
    //[[nodiscard]] float GetMinHeightInWater() const;
    [[nodiscard]] float GetMinHeightInWater() const { return m_minHeightInWater; }

    [[nodiscard]] virtual float GetCollisionHeight() const { return m_collisionHeight; }
    [[nodiscard]] virtual float GetCollisionWidth() const { return GetObjectSize(); }
    [[nodiscard]] virtual float GetCollisionRadius() const { return GetObjectSize() / 2; }

    // Make sure to set these!
    void SetHoverHeight(float value) { m_hoverHeight = value; }
    void SetMinHeightInWater(float value) { m_minHeightInWater = value; }
    void SetObjectSize(float value) { m_objectSize = value; }
    void SetCollisionHeight(float value) { m_collisionHeight = value; }

protected:
    std::string m_name;
    bool m_isActive;
    bool m_isFarVisible;
    Optional<float> m_visibilityDistanceOverride;
    const bool m_isWorldObject;

    uint32 _zoneId;
    uint32 _areaId;
    float _floorZ;
    bool _outdoors;
    //LiquidData _liquidData;
    bool _updatePositionData;

private:
    // TODO: configurable?
	//[[nodiscard]] bool CanFly()  const { return false; }

    //[[nodiscard]] float GetHoverHeight() const { return 0.0f; }
    //[[nodiscard]] float GetMinHeightInWater() const { return 1.0f; } // TODO?
    //[[nodiscard]] float GetObjectSize() const { return 5.0f; }
    //[[nodiscard]] virtual float GetCollisionHeight() const { return 5.0f; }
    float m_minHeightInWater = 0.0f;
    float m_hoverHeight = 0.0f;
    float m_objectSize = 0.0f;
    float m_collisionHeight = 0.0f;

    Map* m_currMap;                                    //current object's Map location

    //uint32 m_mapId;                                     // object at map with map_id
    uint32 m_InstanceId;                                // in map copy with instance id
    uint32 m_phaseMask;                                 // in area phase state
    bool m_useCombinedPhases;                           // true (default): use phaseMask as bit mask combining up to 32 phases
    // false: use phaseMask to represent single phases only (up to 4294967295 phases)

    uint16 m_notifyflags;
    uint16 m_executed_notifies;
};

#endif
