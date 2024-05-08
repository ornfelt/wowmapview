#ifndef ACORE_MAP_H
#define ACORE_MAP_H

#include "Cell.h"
#include "DataMap.h"
#include "Define.h"
#include "DynamicTree.h"
#include "GameObjectModel.h"
#include "GridDefines.h"
#include "GridRefMgr.h"
#include "MapRefMgr.h"
#include "ObjectDefines.h"
#include "PathGenerator.h"
#include "Position.h"
#include "SharedDefines.h"
#include <bitset>
#include <list>
#include <memory>
#include <mutex>
#include <shared_mutex>

class Unit;
class WorldPacket;
class InstanceScript;
class Group;
class InstanceSave;
class Object;
class WorldObject;
class TempSummon;
class Player;
class CreatureGroup;
struct ScriptInfo;
struct Position;
class Battleground;
class MapInstanced;
class InstanceMap;
class BattlegroundMap;
class Transport;
class StaticTransport;
class MotionTransport;
class PathGenerator;

enum WeatherState : uint32;

namespace VMAP
{
    enum class ModelIgnoreFlags : uint32;
}

namespace Acore
{
    struct ObjectUpdater;
    struct LargeObjectUpdater;
}

struct MapEntry
{
    uint32  MapID;                                          // 0
    //char const*       internalname;                             // 1 unused
    uint32  map_type;                                       // 2
    uint32  Flags;                                          // 3
    // 4 0 or 1 for battlegrounds (not arenas)
    char const*   name[16];                                 // 5-20
    // 21 name flags, unused
    uint32  linked_zone;                                    // 22 common zone for instance and continent map
    //char const*     hordeIntro[16];                       // 23-38 text for PvP Zones
    // 39 intro text flags
    //char const*     allianceIntro[16];                    // 40-55 text for PvP Zones
    // 56 intro text flags
    uint32  multimap_id;                                    // 57
    //float BattlefieldMapIconScale;                        // 58
    int32   entrance_map;                                   // 59 map_id of entrance map
    float   entrance_x;                                     // 60 entrance x coordinate (if exist single entry)
    float   entrance_y;                                     // 61 entrance y coordinate (if exist single entry)
    //uint32 TimeOfDayOverride;                             // 62 -1, 0 and 720
    uint32  addon;                                          // 63 (0-original maps, 1-tbc addon)
    uint32  unk_time;                                       // 64 some kind of time?
    uint32  maxPlayers;                                     // 65 max players, fallback if not present in MapDifficulty.dbc

    // Helpers
    [[nodiscard]] uint32 Expansion() const { return addon; }

    bool GetEntrancePos(int32& mapid, float& x, float& y) const
    {
        if (entrance_map < 0)
            return false;
        mapid = entrance_map;
        x = entrance_x;
        y = entrance_y;
        return true;
    }

    [[nodiscard]] bool IsContinent() const
    {
        return MapID == 0 || MapID == 1 || MapID == 530 || MapID == 571;
    }

    //[[nodiscard]] bool IsDynamicDifficultyMap() const { return Flags & MAP_FLAG_DYNAMIC_DIFFICULTY; }
};

// ******************************************
// Map file format defines
// ******************************************
struct map_fileheader
{
    uint32 mapMagic;
    uint32 versionMagic;
    uint32 buildMagic;
    uint32 areaMapOffset;
    uint32 areaMapSize;
    uint32 heightMapOffset;
    uint32 heightMapSize;
    uint32 liquidMapOffset;
    uint32 liquidMapSize;
    uint32 holesOffset;
    uint32 holesSize;
};

#define MAP_AREA_NO_AREA      0x0001

struct map_areaHeader
{
    uint32 fourcc;
    uint16 flags;
    uint16 gridArea;
};

#define MAP_HEIGHT_NO_HEIGHT            0x0001
#define MAP_HEIGHT_AS_INT16             0x0002
#define MAP_HEIGHT_AS_INT8              0x0004
#define MAP_HEIGHT_HAS_FLIGHT_BOUNDS    0x0008

struct map_heightHeader
{
    uint32 fourcc;
    uint32 flags;
    float  gridHeight;
    float  gridMaxHeight;
};

#define MAP_LIQUID_NO_TYPE    0x0001
#define MAP_LIQUID_NO_HEIGHT  0x0002

struct map_liquidHeader
{
    uint32 fourcc;
    uint8 flags;
    uint8 liquidFlags;
    uint16 liquidType;
    uint8  offsetX;
    uint8  offsetY;
    uint8  width;
    uint8  height;
    float  liquidLevel;
};

enum LiquidStatus
{
    LIQUID_MAP_NO_WATER     = 0x00000000,
    LIQUID_MAP_ABOVE_WATER  = 0x00000001,
    LIQUID_MAP_WATER_WALK   = 0x00000002,
    LIQUID_MAP_IN_WATER     = 0x00000004,
    LIQUID_MAP_UNDER_WATER  = 0x00000008
};

#define MAP_LIQUID_STATUS_SWIMMING (LIQUID_MAP_IN_WATER | LIQUID_MAP_UNDER_WATER)
#define MAP_LIQUID_STATUS_IN_CONTACT (MAP_LIQUID_STATUS_SWIMMING | LIQUID_MAP_WATER_WALK)

#define MAP_LIQUID_TYPE_NO_WATER    0x00
#define MAP_LIQUID_TYPE_WATER       0x01
#define MAP_LIQUID_TYPE_OCEAN       0x02
#define MAP_LIQUID_TYPE_MAGMA       0x04
#define MAP_LIQUID_TYPE_SLIME       0x08

#define MAP_ALL_LIQUIDS   (MAP_LIQUID_TYPE_WATER | MAP_LIQUID_TYPE_OCEAN | MAP_LIQUID_TYPE_MAGMA | MAP_LIQUID_TYPE_SLIME)

#define MAP_LIQUID_TYPE_DARK_WATER  0x10

#define MAX_HEIGHT            100000.0f                     // can be use for find ground height at surface
#define INVALID_HEIGHT       -100000.0f                     // for check, must be equal to VMAP_INVALID_HEIGHT, real value for unknown height is VMAP_INVALID_HEIGHT_VALUE
#define MAX_FALL_DISTANCE     250000.0f                     // "unlimited fall" to find VMap ground if it is available, just larger than MAX_HEIGHT - INVALID_HEIGHT
#define DEFAULT_HEIGHT_SEARCH     50.0f                     // default search distance to find height at nearby locations
#define MIN_UNLOAD_DELAY      1                             // immediate unload

struct LiquidData
{
    LiquidData()  = default;

    uint32 Entry{0};
    uint32 Flags{0};
    float  Level{INVALID_HEIGHT};
    float  DepthLevel{INVALID_HEIGHT};
    LiquidStatus Status{LIQUID_MAP_NO_WATER};
};

struct PositionFullTerrainStatus
{
    PositionFullTerrainStatus()  = default;
    uint32 areaId{0};
    float floorZ{INVALID_HEIGHT};
    bool outdoors{false};
    LiquidData liquidInfo;
};

enum LineOfSightChecks
{
    LINEOFSIGHT_CHECK_VMAP          = 0x1, // check static floor layout data
    LINEOFSIGHT_CHECK_GOBJECT_WMO   = 0x2, // check dynamic game object data (wmo models)
    LINEOFSIGHT_CHECK_GOBJECT_M2    = 0x4, // check dynamic game object data (m2 models)

    LINEOFSIGHT_CHECK_GOBJECT_ALL   = LINEOFSIGHT_CHECK_GOBJECT_WMO | LINEOFSIGHT_CHECK_GOBJECT_M2,

    LINEOFSIGHT_ALL_CHECKS          = LINEOFSIGHT_CHECK_VMAP | LINEOFSIGHT_CHECK_GOBJECT_ALL
};

class GridMap
{
    uint32  _flags;
    union
    {
        float* m_V9;
        uint16* m_uint16_V9;
        uint8* m_uint8_V9;
    };
    union
    {
        float* m_V8;
        uint16* m_uint16_V8;
        uint8* m_uint8_V8;
    };
    int16* _maxHeight;
    int16* _minHeight;
    // Height level data
    float _gridHeight;
    float _gridIntHeightMultiplier;

    // Area data
    uint16* _areaMap;

    // Liquid data
    float _liquidLevel;
    uint16* _liquidEntry;
    uint8* _liquidFlags;
    float* _liquidMap;
    uint16 _gridArea;
    uint16 _liquidGlobalEntry;
    uint8 _liquidGlobalFlags;
    uint8 _liquidOffX;
    uint8 _liquidOffY;
    uint8 _liquidWidth;
    uint8 _liquidHeight;
    uint16* _holes;

    bool loadAreaData(FILE* in, uint32 offset, uint32 size);
    bool loadHeightData(FILE* in, uint32 offset, uint32 size);
    bool loadLiquidData(FILE* in, uint32 offset, uint32 size);
    bool loadHolesData(FILE* in, uint32 offset, uint32 size);
    [[nodiscard]] bool isHole(int row, int col) const;

    // Get height functions and pointers
    typedef float (GridMap::*GetHeightPtr) (float x, float y) const;
    GetHeightPtr _gridGetHeight;
    [[nodiscard]] float getHeightFromFloat(float x, float y) const;
    [[nodiscard]] float getHeightFromUint16(float x, float y) const;
    [[nodiscard]] float getHeightFromUint8(float x, float y) const;
    [[nodiscard]] float getHeightFromFlat(float x, float y) const;

public:
    GridMap();
    ~GridMap();
    bool loadData(char* filaname);
    void unloadData();

    [[nodiscard]] uint16 getArea(float x, float y) const;
    [[nodiscard]] inline float getHeight(float x, float y) const {return (this->*_gridGetHeight)(x, y);}
    [[nodiscard]] float getMinHeight(float x, float y) const;
    [[nodiscard]] float getLiquidLevel(float x, float y) const;
    [[nodiscard]] LiquidData const GetLiquidData(float x, float y, float z, float collisionHeight, uint8 ReqLiquidType) const;
};

// GCC have alternative #pragma pack(N) syntax and old gcc version not support pack(push, N), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

struct InstanceTemplate
{
    uint32 Parent;
    uint32 ScriptId;
    bool AllowMount;
};

enum LevelRequirementVsMode
{
    LEVELREQUIREMENT_HEROIC = 70
};

struct ZoneDynamicInfo
{
    ZoneDynamicInfo();

    uint32 MusicId;
    WeatherState WeatherId;
    float WeatherGrade;
    uint32 OverrideLightId;
    uint32 LightFadeInTime;
};

#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

enum EncounterCreditType : uint8
{
    ENCOUNTER_CREDIT_KILL_CREATURE  = 0,
    ENCOUNTER_CREDIT_CAST_SPELL     = 1,
};

class Map : public GridRefMgr<NGridType>
{
    friend class MapReference;
public:
    Map(uint32 id, uint32 InstanceId, uint8 SpawnMode, Map* _parent = nullptr);
    Map();
    ~Map() override;

    //[[nodiscard]] MapEntry const* GetEntry() const { return i_mapEntry; }

    // currently unused for normal maps
    bool CanUnload(uint32 diff)
    {
        if (!m_unloadTimer)
            return false;

        if (m_unloadTimer <= diff)
            return true;

        m_unloadTimer -= diff;
        return false;
    }

    [[nodiscard]] bool IsRemovalGrid(float x, float y) const
    {
        GridCoord p = Acore::ComputeGridCoord(x, y);
        return !getNGrid(p.x_coord, p.y_coord);
    }

    [[nodiscard]] bool IsGridLoaded(float x, float y) const
    {
        return IsGridLoaded(Acore::ComputeGridCoord(x, y));
    }

    void LoadGrid(float x, float y);
    void LoadAllCells();
    bool UnloadGrid(NGridType& ngrid);
    virtual void UnloadAll();

    [[nodiscard]] uint32 GetId() const { return m_mapId; }
    [[nodiscard]] void SetId(uint32 mapId) { m_mapId = mapId; }

    static bool ExistMap(uint32 mapid, int gx, int gy);
    // TODO?
    static bool ExistVMap(uint32 mapid, int gx, int gy);

    [[nodiscard]] Map const* GetParent() const { return m_parentMap; }

    // pussywizard: movemaps, mmaps
    [[nodiscard]] std::shared_mutex& GetMMapLock() const { return *(const_cast<std::shared_mutex*>(&MMapLock)); }
    // pussywizard:

    // some calls like isInWater should not use vmaps due to processor power
    // can return INVALID_HEIGHT if under z+2 z coord not found height
    [[nodiscard]] float GetHeight(float x, float y, float z, bool checkVMap = true, float maxSearchDist = DEFAULT_HEIGHT_SEARCH) const;
    [[nodiscard]] float GetGridHeight(float x, float y) const;
    [[nodiscard]] float GetMinHeight(float x, float y) const;

    void GetFullTerrainStatusForPosition(uint32 phaseMask, float x, float y, float z, float collisionHeight, PositionFullTerrainStatus& data, uint8 reqLiquidType = MAP_ALL_LIQUIDS);
    // TODO?
    //LiquidData const GetLiquidData(uint32 phaseMask, float x, float y, float z, float collisionHeight, uint8 ReqLiquidType);
	LiquidData const GetLiquidData(uint32_t phaseMask, float x, float y, float z, float collisionHeight, uint8_t ReqLiquidType) const
    {
		LiquidData data;

		data.Level = z + collisionHeight / 2;

		return data;
	}

    [[nodiscard]] bool GetAreaInfo(uint32 phaseMask, float x, float y, float z, uint32& mogpflags, int32& adtId, int32& rootId, int32& groupId) const;
    [[nodiscard]] uint32 GetAreaId(uint32 phaseMask, float x, float y, float z) const;
    [[nodiscard]] uint32 GetZoneId(uint32 phaseMask, float x, float y, float z) const;
    void GetZoneAndAreaId(uint32 phaseMask, uint32& zoneid, uint32& areaid, float x, float y, float z) const;

    [[nodiscard]] float GetWaterLevel(float x, float y) const;
    [[nodiscard]] bool IsInWater(uint32 phaseMask, float x, float y, float z, float collisionHeight) const;
    [[nodiscard]] bool IsUnderWater(uint32 phaseMask, float x, float y, float z, float collisionHeight) const;
    [[nodiscard]] bool HasEnoughWater(WorldObject const* searcher, float x, float y, float z) const;
    [[nodiscard]] bool HasEnoughWater(WorldObject const* searcher, LiquidData const& liquidData) const;

    void MoveAllCreaturesInMoveList();
    void MoveAllGameObjectsInMoveList();

    [[nodiscard]] uint32 GetInstanceId() const { return i_InstanceId; }
    [[nodiscard]] uint8 GetSpawnMode() const { return (i_spawnMode); }

    enum EnterState
    {
        CAN_ENTER = 0,
        CANNOT_ENTER_ALREADY_IN_MAP = 1, // Player is already in the map
        CANNOT_ENTER_NO_ENTRY, // No map entry was found for the target map ID
        CANNOT_ENTER_UNINSTANCED_DUNGEON, // No instance template was found for dungeon map
        CANNOT_ENTER_DIFFICULTY_UNAVAILABLE, // Requested instance difficulty is not available for target map
        CANNOT_ENTER_NOT_IN_RAID, // Target instance is a raid instance and the player is not in a raid group
        CANNOT_ENTER_CORPSE_IN_DIFFERENT_INSTANCE, // Player is dead and their corpse is not in target instance
        CANNOT_ENTER_INSTANCE_BIND_MISMATCH, // Player's permanent instance save is not compatible with their group's current instance bind
        CANNOT_ENTER_TOO_MANY_INSTANCES, // Player has entered too many instances recently
        CANNOT_ENTER_MAX_PLAYERS, // Target map already has the maximum number of players allowed
        CANNOT_ENTER_ZONE_IN_COMBAT, // A boss encounter is currently in progress on the target map
        CANNOT_ENTER_UNSPECIFIED_REASON
    };

    virtual EnterState CannotEnter(Player* /*player*/, bool /*loginCheck = false*/) { return CAN_ENTER; }

    [[nodiscard]] const char* GetMapName() const;

    void AddObjectToRemoveList(WorldObject* obj);
    void AddObjectToSwitchList(WorldObject* obj, bool on);

    void resetMarkedCells() { marked_cells.reset(); }
    bool isCellMarked(uint32 pCellId) { return marked_cells.test(pCellId); }
    void markCell(uint32 pCellId) { marked_cells.set(pCellId); }
    void resetMarkedCellsLarge() { marked_cells_large.reset(); }
    bool isCellMarkedLarge(uint32 pCellId) { return marked_cells_large.test(pCellId); }
    void markCellLarge(uint32 pCellId) { marked_cells_large.set(pCellId); }

    [[nodiscard]] bool HavePlayers() const { return !m_mapRefMgr.IsEmpty(); }
    [[nodiscard]] uint32 GetPlayersCountExceptGMs() const;

    void SendToPlayers(WorldPacket const* data) const;

    typedef MapRefMgr PlayerList;
    [[nodiscard]] PlayerList const& GetPlayers() const { return m_mapRefMgr; }

    //per-map script storage
    void ScriptsStart(std::map<uint32, std::multimap<uint32, ScriptInfo> > const& scripts, uint32 id, Object* source, Object* target);
    void ScriptCommandStart(ScriptInfo const& script, uint32 delay, Object* source, Object* target);

    // must called with AddToWorld
    template<class T>
    void AddToActive(T* obj);

    // must called with RemoveFromWorld
    template<class T>
    void RemoveFromActive(T* obj);

    template<class T> void SwitchGridContainers(T* obj, bool on);

    float GetWaterOrGroundLevel(uint32 phasemask, float x, float y, float z, float* ground = nullptr, bool swim = false, float collisionHeight = DEFAULT_COLLISION_HEIGHT) const;
    [[nodiscard]] float GetHeight(uint32 phasemask, float x, float y, float z, bool vmap = true, float maxSearchDist = DEFAULT_HEIGHT_SEARCH) const;
    [[nodiscard]] bool isInLineOfSight(float x1, float y1, float z1, float x2, float y2, float z2, uint32 phasemask, LineOfSightChecks checks, VMAP::ModelIgnoreFlags ignoreFlags) const;
    bool CanReachPositionAndGetValidCoords(WorldObject const* source, PathGenerator *path, float &destX, float &destY, float &destZ, bool failOnCollision = true, bool failOnSlopes = true) const;
    bool CanReachPositionAndGetValidCoords(WorldObject const* source, float &destX, float &destY, float &destZ, bool failOnCollision = true, bool failOnSlopes = true) const;
    bool CanReachPositionAndGetValidCoords(WorldObject const* source, float startX, float startY, float startZ, float &destX, float &destY, float &destZ, bool failOnCollision = true, bool failOnSlopes = true) const;
    bool CheckCollisionAndGetValidCoords(WorldObject const* source, float startX, float startY, float startZ, float &destX, float &destY, float &destZ, bool failOnCollision = true) const;
    void Balance() { _dynamicTree.balance(); }
    void RemoveGameObjectModel(const GameObjectModel& model) { _dynamicTree.remove(model); }
    void InsertGameObjectModel(const GameObjectModel& model) { _dynamicTree.insert(model); }
    [[nodiscard]] bool ContainsGameObjectModel(const GameObjectModel& model) const { return _dynamicTree.contains(model);}
    [[nodiscard]] DynamicMapTree const& GetDynamicMapTree() const { return _dynamicTree; }
    bool GetObjectHitPos(uint32 phasemask, float x1, float y1, float z1, float x2, float y2, float z2, float& rx, float& ry, float& rz, float modifyDist);
    [[nodiscard]] float GetGameObjectFloor(uint32 phasemask, float x, float y, float z, float maxSearchDist = DEFAULT_HEIGHT_SEARCH) const
    {
        return _dynamicTree.getHeight(x, y, z, maxSearchDist, phasemask);
    }

    [[nodiscard]] time_t GetInstanceResetPeriod() const { return _instanceResetPeriod; }

    GridMap* GetGrid(float x, float y);
    void EnsureGridCreated(const GridCoord&);

    DataMap CustomData;

private:
    void LoadMapAndVMap(int gx, int gy);
    void LoadVMap(int gx, int gy);
    void LoadMap(int gx, int gy, bool reload = false);

    // Load MMap Data
    void LoadMMap(int gx, int gy);

    template<class T> void InitializeObject(T* obj);
    void AddCreatureToMoveList(Creature* c);
    void RemoveCreatureFromMoveList(Creature* c);
    void AddGameObjectToMoveList(GameObject* go);
    void RemoveGameObjectFromMoveList(GameObject* go);
    void AddDynamicObjectToMoveList(DynamicObject* go);
    void RemoveDynamicObjectFromMoveList(DynamicObject* go);

    std::vector<Creature*> _creaturesToMove;
    std::vector<GameObject*> _gameObjectsToMove;
    std::vector<DynamicObject*> _dynamicObjectsToMove;

    [[nodiscard]] bool IsGridLoaded(const GridCoord&) const;
    void EnsureGridCreated_i(const GridCoord&);

    void buildNGridLinkage(NGridType* pNGridType) { pNGridType->link(this); }

    [[nodiscard]] NGridType* getNGrid(uint32 x, uint32 y) const
    {
        //ASSERT(x < MAX_NUMBER_OF_GRIDS && y < MAX_NUMBER_OF_GRIDS);
        return i_grids[x][y];
    }

    bool EnsureGridLoaded(Cell const&);
    [[nodiscard]] bool isGridObjectDataLoaded(uint32 x, uint32 y) const { return getNGrid(x, y)->isGridObjectDataLoaded(); }
    void setGridObjectDataLoaded(bool pLoaded, uint32 x, uint32 y) { getNGrid(x, y)->setGridObjectDataLoaded(pLoaded); }

    void setNGrid(NGridType* grid, uint32 x, uint32 y);
    void ScriptsProcess();

    void UpdateActiveCells(const float& x, const float& y, const uint32 t_diff);

    void SendObjectUpdates();

protected:
    std::mutex Lock;
    std::mutex GridLock;
    std::shared_mutex MMapLock;

    MapEntry * i_mapEntry;
    uint32 m_mapId;
    uint8 i_spawnMode;
    uint32 i_InstanceId = 0;
    uint32 m_unloadTimer;
    float m_VisibleDistance;
    DynamicMapTree _dynamicTree;
    time_t _instanceResetPeriod; // pussywizard

    MapRefMgr m_mapRefMgr;
    MapRefMgr::iterator m_mapRefIter;

private:
    //used for fast base_map (e.g. MapInstanced class object) search for
    //InstanceMaps and BattlegroundMaps...
    Map* m_parentMap;
    NGridType* i_grids[MAX_NUMBER_OF_GRIDS][MAX_NUMBER_OF_GRIDS];
    GridMap* GridMaps[MAX_NUMBER_OF_GRIDS][MAX_NUMBER_OF_GRIDS];
    std::bitset<TOTAL_NUMBER_OF_CELLS_PER_MAP* TOTAL_NUMBER_OF_CELLS_PER_MAP> marked_cells;
    std::bitset<TOTAL_NUMBER_OF_CELLS_PER_MAP* TOTAL_NUMBER_OF_CELLS_PER_MAP> marked_cells_large;

    // Type specific code for add/remove to/from grid
    template<class T>
    void AddToGrid(T* object, Cell const& cell);

};

enum InstanceResetMethod
{
    INSTANCE_RESET_ALL,                 // reset all option under portrait, resets only normal 5-mans
    INSTANCE_RESET_CHANGE_DIFFICULTY,   // on changing difficulty
    INSTANCE_RESET_GLOBAL,              // global id reset
    INSTANCE_RESET_GROUP_JOIN,          // on joining group
    INSTANCE_RESET_GROUP_LEAVE          // on leaving group
};

#endif
