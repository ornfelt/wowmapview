#include "MapMgr.h"
#include "GridDefines.h"
#include "ObjectAccessor.h"

MapMgr::MapMgr()
{
    //i_timer[3].SetInterval(sWorld->getIntConfig(CONFIG_INTERVAL_MAPUPDATE));
    mapUpdateStep = 0;
    _nextInstanceId = 0;
}

MapMgr::~MapMgr()
{
}

MapMgr* MapMgr::instance()
{
    static MapMgr instance;
    return &instance;
}

void MapMgr::Initialize()
{
    //int num_threads(sWorld->getIntConfig(CONFIG_NUMTHREADS));
    int num_threads(4);

    // Start mtmaps if needed
    //if (num_threads > 0)
    //    m_updater.activate(num_threads);
}

enum Difficulty : uint8
{
    REGULAR_DIFFICULTY           = 0,

    DUNGEON_DIFFICULTY_NORMAL    = 0,
    DUNGEON_DIFFICULTY_HEROIC    = 1,
    DUNGEON_DIFFICULTY_EPIC      = 2,

    RAID_DIFFICULTY_10MAN_NORMAL = 0,
    RAID_DIFFICULTY_25MAN_NORMAL = 1,
    RAID_DIFFICULTY_10MAN_HEROIC = 2,
    RAID_DIFFICULTY_25MAN_HEROIC = 3,
};

Map* MapMgr::CreateBaseMap(uint32 id)
{
    Map* map = FindBaseMap(id);

    if (!map)
    {
        std::lock_guard<std::mutex> guard(Lock);

        map = FindBaseMap(id);
        if (!map) // pussywizard: check again after acquiring mutex
        {
            //MapEntry const* entry = sMapStore.LookupEntry(id);
            ////ASSERT(entry);

            //if (entry->Instanceable())
            //    map = new MapInstanced(id);
            //else
            //{
                map = new Map(id, 0, REGULAR_DIFFICULTY);
                //map->LoadRespawnTimes();
                //map->LoadCorpseData();
            //}

            i_maps[id] = map;
        }
    }

    //ASSERT(map);
    return map;
}

Map* MapMgr::FindBaseNonInstanceMap(uint32 mapId) const
{
    Map* map = FindBaseMap(mapId);
    //if (map && map->Instanceable())
    //    return nullptr;
    return map;
}

Map* MapMgr::CreateMap(uint32 id, Player* player)
{
    Map* m = CreateBaseMap(id);

    //if (m && m->Instanceable())
    //    m = ((MapInstanced*)m)->CreateInstanceForPlayer(id, player);

    return m;
}

Map* MapMgr::FindMap(uint32 mapid, uint32 instanceId) const
{
    Map* map = FindBaseMap(mapid);
    if (!map)
        return nullptr;

    //if (!map->Instanceable())
    //    return instanceId == 0 ? map : nullptr;

    return ((MapInstanced*)map)->FindInstanceMap(instanceId);
}

bool MapMgr::ExistMapAndVMap(uint32 mapid, float x, float y)
{
    GridCoord p = Acore::ComputeGridCoord(x, y);

    int gx = 63 - p.x_coord;
    int gy = 63 - p.y_coord;

    //return Map::ExistMap(mapid, gx, gy) && Map::ExistVMap(mapid, gx, gy);
    return Map::ExistMap(mapid, gx, gy);
}

bool MapMgr::IsValidMAP(uint32 mapid, bool startUp)
{
    //MapEntry const* mEntry = sMapStore.LookupEntry(mapid);

    //if (startUp)
    //{
    //    return mEntry != nullptr;
    //}
    //else
    //{
    //    return mEntry && (!mEntry->IsDungeon() || sObjectMgr->GetInstanceTemplate(mapid));
    //}
    return true;

    /// @todo: add check for battleground template
}

void MapMgr::UnloadAll()
{
    for (MapMapType::iterator iter = i_maps.begin(); iter != i_maps.end();)
    {
        iter->second->UnloadAll();
        delete iter->second;
        i_maps.erase(iter++);
    }

    //if (m_updater.activated())
    //    m_updater.deactivate();
}

void MapMgr::InitInstanceIds()
{
    _nextInstanceId = 1;

    //QueryResult result = CharacterDatabase.Query("SELECT MAX(id) FROM instance");
    //if (result)
    //{
    //    uint32 maxId = (*result)[0].Get<uint32>();
    //    _instanceIds.resize(maxId + 1);
    //}
}

void MapMgr::RegisterInstanceId(uint32 instanceId)
{
    // Allocation was done in InitInstanceIds()
    _instanceIds[instanceId] = true;

    // Instances are pulled in ascending order from db and _nextInstanceId is initialized with 1,
    // so if the instance id is used, increment
    if (_nextInstanceId == instanceId)
        ++_nextInstanceId;
}
