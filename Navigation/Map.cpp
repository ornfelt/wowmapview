#include "Map.h"
#include "CellImpl.h"
#include "DynamicTree.h"
#include "Geometry.h"
#include "MapInstanced.h"
#include "Object.h"
#include "ObjectAccessor.h"
#include "ObjectGridLoader.h"
#include "VMapFactory.h"
#include "Creature.h"

#include "VMapMgr2.h"

union u_map_magic
{
    char asChar[4];
    uint32 asUInt;
};

u_map_magic MapMagic        = { {'M', 'A', 'P', 'S'} };
uint32 MapVersionMagic      = 9;
u_map_magic MapAreaMagic    = { {'A', 'R', 'E', 'A'} };
u_map_magic MapHeightMagic  = { {'M', 'H', 'G', 'T'} };
u_map_magic MapLiquidMagic  = { {'M', 'L', 'I', 'Q'} };

static uint16 const holetab_h[4] = { 0x1111, 0x2222, 0x4444, 0x8888 };
static uint16 const holetab_v[4] = { 0x000F, 0x00F0, 0x0F00, 0xF000 };

Map::~Map()
{
    // UnloadAll must be called before deleting the map

    //while (!i_worldObjects.empty())
    //{
    //    WorldObject* obj = *i_worldObjects.begin();
    //    //ASSERT(obj->IsWorldObject());
    //    //LOG_DEBUG("maps", "Map::~Map: WorldObject TypeId is not a corpse! ({})", static_cast<uint8>(obj->GetTypeId()));
    //    //ASSERT(obj->GetTypeId() == TYPEID_CORPSE);
    //    //obj->RemoveFromWorld();
    //    obj->ResetMap();
    //}

    //MMAP::MMapFactory::createOrGetMMapMgr()->unloadMap(GetId());
    MMAP::MMapFactory::createOrGetMMapMgr()->unloadMapInstance(GetId(), i_InstanceId);
}

class World {
public:
    std::string GetDataPath() {
        return ".";
    }
};

bool Map::ExistMap(uint32 mapid, int gx, int gy)
{
    // TODO:
    //int len = sWorld->GetDataPath().length() + strlen("maps/%03u%02u%02u.map") + 1;
    int len = std::string(".").length() + strlen("maps/%03u%02u%02u.map") + 1;
    char* tmp = new char[len];
    //snprintf(tmp, len, (char*)(sWorld->GetDataPath() + "maps/%03u%02u%02u.map").c_str(), mapid, gx, gy);
    snprintf(tmp, len, (char*)(std::string(".") + "maps/%03u%02u%02u.map").c_str(), mapid, gx, gy);

    bool ret = false;
    FILE* pf = fopen(tmp, "rb");

    //if (!pf)
    //    LOG_ERROR("maps", "Map file '{}': does not exist!", tmp);
    if (pf)
    {
        map_fileheader header;
        if (fread(&header, sizeof(header), 1, pf) == 1)
        {
            if (header.mapMagic != MapMagic.asUInt || header.versionMagic != MapVersionMagic)
            {
                //LOG_ERROR("maps", "Map file '{}' is from an incompatible map version (%.*u v{}), %.*s v{} is expected. Please pull your source, recompile tools and recreate maps using the updated mapextractor, then replace your old map files with new files.",
                //    tmp, 4, header.mapMagic, header.versionMagic, 4, MapMagic.asChar, MapVersionMagic);
            }

            else
                ret = true;
        }
        fclose(pf);
    }
    delete [] tmp;
    return ret;
}

//bool Map::ExistVMap(uint32 mapid, int gx, int gy)
//{
//    if (VMAP::IVMapMgr* vmgr = VMAP::VMapFactory::createOrGetVMapMgr())
//    {
//        if (vmgr->isMapLoadingEnabled())
//        {
//            VMAP::LoadResult result = vmgr->existsMap((sWorld->GetDataPath() + "vmaps").c_str(), mapid, gx, gy);
//            std::string name = vmgr->getDirFileName(mapid, gx, gy);
//            switch (result)
//            {
//                case VMAP::LoadResult::Success:
//                    break;
//                case VMAP::LoadResult::FileNotFound:
//                    //LOG_ERROR("maps", "VMap file '{}' does not exist", (sWorld->GetDataPath() + "vmaps/" + name));
//                    //LOG_ERROR("maps", "Please place VMAP files (*.vmtree and *.vmtile) in the vmap directory ({}), or correct the DataDir setting in your worldserver.conf file.", (sWorld->GetDataPath() + "vmaps/"));
//                    return false;
//                case VMAP::LoadResult::VersionMismatch:
//                    //LOG_ERROR("maps", "VMap file '{}' couldn't be loaded", (sWorld->GetDataPath() + "vmaps/" + name));
//                    //LOG_ERROR("maps", "This is because the version of the VMap file and the version of this module are different, please re-extract the maps with the tools compiled with this module.");
//                    return false;
//            }
//        }
//    }
//
//    return true;
//}

void Map::LoadMMap(int gx, int gy)
{
    int mmapLoadResult = MMAP::MMapFactory::createOrGetMMapMgr()->loadMap(GetId(), gx, gy);
    switch (mmapLoadResult)
    {
        case MMAP::MMAP_LOAD_RESULT_OK:
            //LOG_DEBUG("maps", "MMAP loaded name:{}, id:{}, x:{}, y:{} (vmap rep.: x:{}, y:{})", GetMapName(), GetId(), gx, gy, gx, gy);
			//std::cout << "MMAP loaded name: " << ""
            //      << ", id: " << GetId()
            //      << ", x: " << gx
            //      << ", y: " << gy
            //      << " (vmap rep.: x: " << gx
            //      << ", y: " << gy << ")" << std::endl;
            break;
        case MMAP::MMAP_LOAD_RESULT_ERROR:
            //LOG_DEBUG("maps", "Could not load MMAP name:{}, id:{}, x:{}, y:{} (vmap rep.: x:{}, y:{})", GetMapName(), GetId(), gx, gy, gx, gy);
			//std::cout << "Could not load MMAP name: " << ""
            //      << ", id: " << GetId()
            //      << ", x: " << gx
            //      << ", y: " << gy
            //      << " (vmap rep.: x: " << gx
            //      << ", y: " << gy << ")" << std::endl;
            break;
        case MMAP::MMAP_LOAD_RESULT_IGNORED:
            //LOG_DEBUG("maps", "Ignored MMAP name:{}, id:{}, x:{}, y:{} (vmap rep.: x:{}, y:{})", GetMapName(), GetId(), gx, gy, gx, gy);
			std::cout << "Ignored MMAP name: " << ""
                  << ", id: " << GetId()
                  << ", x: " << gx
                  << ", y: " << gy
                  << " (vmap rep.: x: " << gx
                  << ", y: " << gy << ")" << std::endl;
            break;
    }
}

void Map::LoadVMap(int gx, int gy)
{
    // x and y are swapped !!
    //int vmapLoadResult = VMAP::VMapFactory::createOrGetVMapMgr()->loadMap((sWorld->GetDataPath() + "vmaps").c_str(), GetId(), gx, gy);
    //int vmapLoadResult = VMAP::VMapFactory::createOrGetVMapMgr()->loadMap(std::string("C:\\local\\acore\\vmaps").c_str(), GetId(), gx, gy);
    int vmapLoadResult = VMAP::VMapFactory::createOrGetVMapMgr()->loadMap(std::string("/home/jonas/acore/bin/vmaps").c_str(), GetId(), gx, gy);
    switch (vmapLoadResult)
    {
        case VMAP::VMAP_LOAD_RESULT_OK:
            //LOG_DEBUG("maps", "VMAP loaded name:{}, id:{}, x:{}, y:{} (vmap rep.: x:{}, y:{})", GetMapName(), GetId(), gx, gy, gx, gy);
			//std::cout << "VMAP loaded name: " << ""
            //      << ", id: " << GetId()
            //      << ", x: " << gx
            //      << ", y: " << gy
            //      << " (vmap rep.: x: " << gx
            //      << ", y: " << gy << ")" << std::endl;
            break;
        case VMAP::VMAP_LOAD_RESULT_ERROR:
            //LOG_DEBUG("maps", "Could not load VMAP name:{}, id:{}, x:{}, y:{} (vmap rep.: x:{}, y:{})", GetMapName(), GetId(), gx, gy, gx, gy);
			std::cout << "Could not load VMAP name: " << ""
                  << ", id: " << GetId()
                  << ", x: " << gx
                  << ", y: " << gy
                  << " (vmap rep.: x: " << gx
                  << ", y: " << gy << ")" << std::endl;
            break;
        case VMAP::VMAP_LOAD_RESULT_IGNORED:
            //LOG_DEBUG("maps", "Ignored VMAP name:{}, id:{}, x:{}, y:{} (vmap rep.: x:{}, y:{})", GetMapName(), GetId(), gx, gy, gx, gy);
			std::cout << "Ignored VMAP name: " << ""
                  << ", id: " << GetId()
                  << ", x: " << gx
                  << ", y: " << gy
                  << " (vmap rep.: x: " << gx
                  << ", y: " << gy << ")" << std::endl;
            break;
    }
}

void Map::LoadMap(int gx, int gy, bool reload)
{
    if (i_InstanceId != 0)
    {
        if (GridMaps[gx][gy])
            return;

        // load grid map for base map
        m_parentMap->EnsureGridCreated(GridCoord(63 - gx, 63 - gy));

        GridMaps[gx][gy] = m_parentMap->GridMaps[gx][gy];
        return;
    }

    if (GridMaps[gx][gy] && !reload)
        return;

    //map already load, delete it before reloading (Is it necessary? Do we really need the ability the reload maps during runtime?)
    if (GridMaps[gx][gy])
    {
        //LOG_DEBUG("maps", "Unloading previously loaded map {} before reloading.", GetId());
        delete (GridMaps[gx][gy]);
        GridMaps[gx][gy] = nullptr;
    }

    // map file name
    char* tmp = nullptr;
    //int len = sWorld->GetDataPath().length() + strlen("maps/%03u%02u%02u.map") + 1;
    //int len = std::string("C:\\local\\acore\\").length() + strlen("maps/%03u%02u%02u.map") + 1;
    int len = std::string("/home/jonas/acore/bin/").length() + strlen("maps/%03u%02u%02u.map") + 1;

    tmp = new char[len];
    //snprintf(tmp, len, (char*)(sWorld->GetDataPath() + "maps/%03u%02u%02u.map").c_str(), GetId(), gx, gy);
    //snprintf(tmp, len, (char*)(std::string("C:\\local\\acore\\") + "maps/%03u%02u%02u.map").c_str(), GetId(), gx, gy);
    snprintf(tmp, len, (char*)(std::string("/home/jonas/acore/bin/") + "maps/%03u%02u%02u.map").c_str(), GetId(), gx, gy);

    //LOG_DEBUG("maps", "Loading map {}", tmp);
    // loading data
    GridMaps[gx][gy] = new GridMap();
    if (!GridMaps[gx][gy]->loadData(tmp))
    {
        //LOG_ERROR("maps", "Error loading map file: \n {}\n", tmp);
    }
    delete [] tmp;
}

void Map::LoadMapAndVMap(int gx, int gy)
{
    LoadMap(gx, gy);
    if (i_InstanceId == 0)
    {
        LoadVMap(gx, gy);                                   // Only load the data for the base map
        LoadMMap(gx, gy);
    }
}

//Map::Map(uint32 id, uint32 InstanceId, uint8 SpawnMode, Map* _parent) :
//    i_mapEntry(sMapStore.LookupEntry(id)), i_spawnMode(SpawnMode), i_InstanceId(InstanceId),
//    m_unloadTimer(0), m_VisibleDistance(DEFAULT_VISIBILITY_DISTANCE),
//    _instanceResetPeriod(0), m_activeNonPlayersIter(m_activeNonPlayers.end()),
//    _transportsUpdateIter(_transports.end()), i_scriptLock(false), _defaultLight(GetDefaultMapLight(id))
Map::Map(uint32 id, uint32 InstanceId, uint8 SpawnMode, Map* _parent)
{
    m_parentMap = (_parent ? _parent : this);
    for (unsigned int idx = 0; idx < MAX_NUMBER_OF_GRIDS; ++idx)
    {
        for (unsigned int j = 0; j < MAX_NUMBER_OF_GRIDS; ++j)
        {
            //z code
            GridMaps[idx][j] = nullptr;
            setNGrid(nullptr, idx, j);
        }
    }

    //lets initialize visibility distance for map
    //Map::InitVisibilityDistance();
}

void Map::EnsureGridCreated(const GridCoord& p)
{
    if (getNGrid(p.x_coord, p.y_coord)) // pussywizard
        return;
    std::lock_guard<std::mutex> guard(GridLock);
    EnsureGridCreated_i(p);
}

//Create NGrid so the object can be added to it
//But object data is not loaded here
void Map::EnsureGridCreated_i(const GridCoord& p)
{
    if (!getNGrid(p.x_coord, p.y_coord))
    {
        // pussywizard: moved setNGrid to the end of the function
        NGridType* ngt = new NGridType(p.x_coord * MAX_NUMBER_OF_GRIDS + p.y_coord, p.x_coord, p.y_coord);

        // build a linkage between this map and NGridType
        buildNGridLinkage(ngt); // pussywizard: getNGrid(x, y) changed to: ngt

        //z coord
        int gx = (MAX_NUMBER_OF_GRIDS - 1) - p.x_coord;
        int gy = (MAX_NUMBER_OF_GRIDS - 1) - p.y_coord;

        if (!GridMaps[gx][gy])
        {
            LoadMapAndVMap(gx, gy);
        }

        // pussywizard: moved here
        setNGrid(ngt, p.x_coord, p.y_coord);
    }
}

//Create NGrid and load the object data in it
bool Map::EnsureGridLoaded(const Cell& cell)
{
    EnsureGridCreated(GridCoord(cell.GridX(), cell.GridY()));
    NGridType* grid = getNGrid(cell.GridX(), cell.GridY());

    //ASSERT(grid);
    if (!isGridObjectDataLoaded(cell.GridX(), cell.GridY()))
    {
        //if (!isGridObjectDataLoaded(cell.GridX(), cell.GridY()))
        //{
        //LOG_DEBUG("maps", "Loading grid[{}, {}] for map {} instance {}", cell.GridX(), cell.GridY(), GetId(), i_InstanceId);
        //std::cout << "Loading grid: " << cell.GridX() << ", " << cell.GridY() << " for map: " << GetId() << std::endl;

        setGridObjectDataLoaded(true, cell.GridX(), cell.GridY());

        ObjectGridLoader loader(*grid, this, cell);
        loader.LoadN();

        Balance();
        return true;
        //}
    }

    return false;
}

void Map::LoadGrid(float x, float y)
{
    EnsureGridLoaded(Cell(x, y));
}

void Map::LoadAllCells()
{
    for (uint32 cellX = 0; cellX < TOTAL_NUMBER_OF_CELLS_PER_MAP; cellX++)
        for (uint32 cellY = 0; cellY < TOTAL_NUMBER_OF_CELLS_PER_MAP; cellY++)
            LoadGrid((cellX + 0.5f - CENTER_GRID_CELL_ID) * SIZE_OF_GRID_CELL, (cellY + 0.5f - CENTER_GRID_CELL_ID) * SIZE_OF_GRID_CELL);
}

template<class T>
void Map::InitializeObject(T* /*obj*/)
{
}

template<>
void Map::InitializeObject(Creature*  /*obj*/)
{
    //obj->_moveState = MAP_OBJECT_CELL_MOVE_NONE;
}

template<>
void Map::InitializeObject(GameObject*  /*obj*/)
{
    //obj->_moveState = MAP_OBJECT_CELL_MOVE_NONE;
}

bool Map::IsGridLoaded(const GridCoord& p) const
{
    return (getNGrid(p.x_coord, p.y_coord) && isGridObjectDataLoaded(p.x_coord, p.y_coord));
}

bool Map::UnloadGrid(NGridType& ngrid)
{
    // pussywizard: UnloadGrid only done when whole map is unloaded, no need to worry about moving npcs between grids, etc.

    const uint32 x = ngrid.getX();
    const uint32 y = ngrid.getY();

    // TODO?
    //{
    //    ObjectGridCleaner worker;
    //    TypeContainerVisitor<ObjectGridCleaner, GridTypeMapContainer> visitor(worker);
    //    ngrid.VisitAllGrids(visitor);
    //}

    //RemoveAllObjectsInRemoveList();

    // TODO?
    //{
    //    ObjectGridUnloader worker;
    //    TypeContainerVisitor<ObjectGridUnloader, GridTypeMapContainer> visitor(worker);
    //    ngrid.VisitAllGrids(visitor);
    //}

    //ASSERT(i_objectsToRemove.empty());

    delete &ngrid;
    setNGrid(nullptr, x, y);

    int gx = (MAX_NUMBER_OF_GRIDS - 1) - x;
    int gy = (MAX_NUMBER_OF_GRIDS - 1) - y;

    if (i_InstanceId == 0)
    {
        if (GridMaps[gx][gy])
        {
            GridMaps[gx][gy]->unloadData();
            delete GridMaps[gx][gy];
        }
        // x and y are swapped
        // TODO?
        //VMAP::VMapFactory::createOrGetVMapMgr()->unloadMap(GetId(), gx, gy);
        //MMAP::MMapFactory::createOrGetMMapMgr()->unloadMap(GetId(), gx, gy);
    }

    GridMaps[gx][gy] = nullptr;

    //LOG_DEBUG("maps", "Unloading grid[{}, {}] for map {} finished", x, y, GetId());
    return true;
}

void Map::UnloadAll()
{
    // clear all delayed moves, useless anyway do this moves before map unload.
    _creaturesToMove.clear();
    _gameObjectsToMove.clear();

    for (GridRefMgr<NGridType>::iterator i = GridRefMgr<NGridType>::begin(); i != GridRefMgr<NGridType>::end();)
    {
        NGridType& grid(*i->GetSource());
        ++i;
        UnloadGrid(grid); // deletes the grid and removes it from the GridRefMgr
    }
}

// *****************************
// Grid function
// *****************************
GridMap::GridMap()
{
    _flags = 0;
    // Area data
    _gridArea = 0;
    _areaMap = nullptr;
    // Height level data
    _gridHeight = INVALID_HEIGHT;
    _gridGetHeight = &GridMap::getHeightFromFlat;
    _gridIntHeightMultiplier = 0;
    m_V9 = nullptr;
    m_V8 = nullptr;
    _maxHeight = nullptr;
    _minHeight = nullptr;
    // Liquid data
    _liquidGlobalEntry = 0;
    _liquidGlobalFlags = 0;
    _liquidOffX   = 0;
    _liquidOffY   = 0;
    _liquidWidth  = 0;
    _liquidHeight = 0;
    _liquidLevel = INVALID_HEIGHT;
    _liquidEntry = nullptr;
    _liquidFlags = nullptr;
    _liquidMap  = nullptr;
    _holes = nullptr;
}

GridMap::~GridMap()
{
    unloadData();
}

bool GridMap::loadData(char* filename)
{
    // Unload old data if exist
    unloadData();

    map_fileheader header;
    // Not return error if file not found
    FILE* in = fopen(filename, "rb");
    if (!in)
        return true;

    if (fread(&header, sizeof(header), 1, in) != 1)
    {
        fclose(in);
        return false;
    }

    if (header.mapMagic == MapMagic.asUInt && header.versionMagic == MapVersionMagic)
    {
        // loadup area data
        if (header.areaMapOffset && !loadAreaData(in, header.areaMapOffset, header.areaMapSize))
        {
            //LOG_ERROR("maps", "Error loading map area data\n");
            fclose(in);
            return false;
        }
        // loadup height data
        if (header.heightMapOffset && !loadHeightData(in, header.heightMapOffset, header.heightMapSize))
        {
            //LOG_ERROR("maps", "Error loading map height data\n");
            fclose(in);
            return false;
        }
        // loadup liquid data
        if (header.liquidMapOffset && !loadLiquidData(in, header.liquidMapOffset, header.liquidMapSize))
        {
            //LOG_ERROR("maps", "Error loading map liquids data\n");
            fclose(in);
            return false;
        }
        // loadup holes data (if any. check header.holesOffset)
        if (header.holesSize && !loadHolesData(in, header.holesOffset, header.holesSize))
        {
            //LOG_ERROR("maps", "Error loading map holes data\n");
            fclose(in);
            return false;
        }
        fclose(in);
        return true;
    }
    //LOG_ERROR("maps", "Map file '{}' is from an incompatible clientversion. Please recreate using the mapextractor.", filename);
    fclose(in);
    return false;
}

void GridMap::unloadData()
{
    delete[] _areaMap;
    delete[] m_V9;
    delete[] m_V8;
    delete[] _maxHeight;
    delete[] _minHeight;
    delete[] _liquidEntry;
    delete[] _liquidFlags;
    delete[] _liquidMap;
    delete[] _holes;
    _areaMap = nullptr;
    m_V9 = nullptr;
    m_V8 = nullptr;
    _maxHeight = nullptr;
    _minHeight = nullptr;
    _liquidEntry = nullptr;
    _liquidFlags = nullptr;
    _liquidMap  = nullptr;
    _holes = nullptr;
    _gridGetHeight = &GridMap::getHeightFromFlat;
}

bool GridMap::loadAreaData(FILE* in, uint32 offset, uint32 /*size*/)
{
    map_areaHeader header;
    fseek(in, offset, SEEK_SET);

    if (fread(&header, sizeof(header), 1, in) != 1 || header.fourcc != MapAreaMagic.asUInt)
        return false;

    _gridArea = header.gridArea;
    if (!(header.flags & MAP_AREA_NO_AREA))
    {
        _areaMap = new uint16 [16 * 16];
        if (fread(_areaMap, sizeof(uint16), 16 * 16, in) != 16 * 16)
            return false;
    }
    return true;
}

bool GridMap::loadHeightData(FILE* in, uint32 offset, uint32 /*size*/)
{
    map_heightHeader header;
    fseek(in, offset, SEEK_SET);

    if (fread(&header, sizeof(header), 1, in) != 1 || header.fourcc != MapHeightMagic.asUInt)
        return false;

    _gridHeight = header.gridHeight;
    if (!(header.flags & MAP_HEIGHT_NO_HEIGHT))
    {
        if ((header.flags & MAP_HEIGHT_AS_INT16))
        {
            m_uint16_V9 = new uint16 [129 * 129];
            m_uint16_V8 = new uint16 [128 * 128];
            if (fread(m_uint16_V9, sizeof(uint16), 129 * 129, in) != 129 * 129 ||
                    fread(m_uint16_V8, sizeof(uint16), 128 * 128, in) != 128 * 128)
                return false;
            _gridIntHeightMultiplier = (header.gridMaxHeight - header.gridHeight) / 65535;
            _gridGetHeight = &GridMap::getHeightFromUint16;
        }
        else if ((header.flags & MAP_HEIGHT_AS_INT8))
        {
            m_uint8_V9 = new uint8 [129 * 129];
            m_uint8_V8 = new uint8 [128 * 128];
            if (fread(m_uint8_V9, sizeof(uint8), 129 * 129, in) != 129 * 129 ||
                    fread(m_uint8_V8, sizeof(uint8), 128 * 128, in) != 128 * 128)
                return false;
            _gridIntHeightMultiplier = (header.gridMaxHeight - header.gridHeight) / 255;
            _gridGetHeight = &GridMap::getHeightFromUint8;
        }
        else
        {
            m_V9 = new float [129 * 129];
            m_V8 = new float [128 * 128];
            if (fread(m_V9, sizeof(float), 129 * 129, in) != 129 * 129 ||
                    fread(m_V8, sizeof(float), 128 * 128, in) != 128 * 128)
                return false;
            _gridGetHeight = &GridMap::getHeightFromFloat;
        }
    }
    else
        _gridGetHeight = &GridMap::getHeightFromFlat;

    if (header.flags & MAP_HEIGHT_HAS_FLIGHT_BOUNDS)
    {
        _maxHeight = new int16[3 * 3];
        _minHeight = new int16[3 * 3];
        if (fread(_maxHeight, sizeof(int16), 3 * 3, in) != 3 * 3 ||
                fread(_minHeight, sizeof(int16), 3 * 3, in) != 3 * 3)
            return false;
    }

    return true;
}

bool GridMap::loadLiquidData(FILE* in, uint32 offset, uint32 /*size*/)
{
    map_liquidHeader header;
    fseek(in, offset, SEEK_SET);

    if (fread(&header, sizeof(header), 1, in) != 1 || header.fourcc != MapLiquidMagic.asUInt)
        return false;

    _liquidGlobalEntry = header.liquidType;
    _liquidGlobalFlags = header.liquidFlags;
    _liquidOffX  = header.offsetX;
    _liquidOffY  = header.offsetY;
    _liquidWidth = header.width;
    _liquidHeight = header.height;
    _liquidLevel  = header.liquidLevel;

    if (!(header.flags & MAP_LIQUID_NO_TYPE))
    {
        _liquidEntry = new uint16[16 * 16];
        if (fread(_liquidEntry, sizeof(uint16), 16 * 16, in) != 16 * 16)
            return false;

        _liquidFlags = new uint8[16 * 16];
        if (fread(_liquidFlags, sizeof(uint8), 16 * 16, in) != 16 * 16)
            return false;
    }
    if (!(header.flags & MAP_LIQUID_NO_HEIGHT))
    {
        _liquidMap = new float[uint32(_liquidWidth) * uint32(_liquidHeight)];
        if (fread(_liquidMap, sizeof(float), _liquidWidth * _liquidHeight, in) != (uint32(_liquidWidth) * uint32(_liquidHeight)))
            return false;
    }
    return true;
}

bool GridMap::loadHolesData(FILE* in, uint32 offset, uint32 /*size*/)
{
    if (fseek(in, offset, SEEK_SET) != 0)
        return false;

    _holes = new uint16[16 * 16];
    if (fread(_holes, sizeof(uint16), 16 * 16, in) != 16 * 16)
        return false;

    return true;
}

uint16 GridMap::getArea(float x, float y) const
{
    if (!_areaMap)
        return _gridArea;

    x = 16 * (32 - x / SIZE_OF_GRIDS);
    y = 16 * (32 - y / SIZE_OF_GRIDS);
    int lx = (int)x & 15;
    int ly = (int)y & 15;
    return _areaMap[lx * 16 + ly];
}

float GridMap::getHeightFromFlat(float /*x*/, float /*y*/) const
{
    return _gridHeight;
}

float GridMap::getHeightFromFloat(float x, float y) const
{
    if (!m_V8 || !m_V9)
        return _gridHeight;

    x = MAP_RESOLUTION * (32 - x / SIZE_OF_GRIDS);
    y = MAP_RESOLUTION * (32 - y / SIZE_OF_GRIDS);

    int x_int = (int)x;
    int y_int = (int)y;
    x -= x_int;
    y -= y_int;
    x_int &= (MAP_RESOLUTION - 1);
    y_int &= (MAP_RESOLUTION - 1);

    if (isHole(x_int, y_int))
        return INVALID_HEIGHT;

    // Height stored as: h5 - its v8 grid, h1-h4 - its v9 grid
    // +--------------> X
    // | h1-------h2     Coordinates is:
    // | | \  1  / |     h1 0, 0
    // | |  \   /  |     h2 0, 1
    // | | 2  h5 3 |     h3 1, 0
    // | |  /   \  |     h4 1, 1
    // | | /  4  \ |     h5 1/2, 1/2
    // | h3-------h4
    // V Y
    // For find height need
    // 1 - detect triangle
    // 2 - solve linear equation from triangle points
    // Calculate coefficients for solve h = a*x + b*y + c

    float a, b, c;
    // Select triangle:
    if (x + y < 1)
    {
        if (x > y)
        {
            // 1 triangle (h1, h2, h5 points)
            float h1 = m_V9[(x_int) * 129 + y_int];
            float h2 = m_V9[(x_int + 1) * 129 + y_int];
            float h5 = 2 * m_V8[x_int * 128 + y_int];
            a = h2 - h1;
            b = h5 - h1 - h2;
            c = h1;
        }
        else
        {
            // 2 triangle (h1, h3, h5 points)
            float h1 = m_V9[x_int * 129 + y_int  ];
            float h3 = m_V9[x_int * 129 + y_int + 1];
            float h5 = 2 * m_V8[x_int * 128 + y_int];
            a = h5 - h1 - h3;
            b = h3 - h1;
            c = h1;
        }
    }
    else
    {
        if (x > y)
        {
            // 3 triangle (h2, h4, h5 points)
            float h2 = m_V9[(x_int + 1) * 129 + y_int  ];
            float h4 = m_V9[(x_int + 1) * 129 + y_int + 1];
            float h5 = 2 * m_V8[x_int * 128 + y_int];
            a = h2 + h4 - h5;
            b = h4 - h2;
            c = h5 - h4;
        }
        else
        {
            // 4 triangle (h3, h4, h5 points)
            float h3 = m_V9[(x_int) * 129 + y_int + 1];
            float h4 = m_V9[(x_int + 1) * 129 + y_int + 1];
            float h5 = 2 * m_V8[x_int * 128 + y_int];
            a = h4 - h3;
            b = h3 + h4 - h5;
            c = h5 - h4;
        }
    }
    // Calculate height
    return a * x + b * y + c;
}

float GridMap::getHeightFromUint8(float x, float y) const
{
    if (!m_uint8_V8 || !m_uint8_V9)
        return _gridHeight;

    x = MAP_RESOLUTION * (32 - x / SIZE_OF_GRIDS);
    y = MAP_RESOLUTION * (32 - y / SIZE_OF_GRIDS);

    int x_int = (int)x;
    int y_int = (int)y;
    x -= x_int;
    y -= y_int;
    x_int &= (MAP_RESOLUTION - 1);
    y_int &= (MAP_RESOLUTION - 1);

    if (isHole(x_int, y_int))
        return INVALID_HEIGHT;

    int32 a, b, c;
    uint8* V9_h1_ptr = &m_uint8_V9[x_int * 128 + x_int + y_int];
    if (x + y < 1)
    {
        if (x > y)
        {
            // 1 triangle (h1, h2, h5 points)
            int32 h1 = V9_h1_ptr[  0];
            int32 h2 = V9_h1_ptr[129];
            int32 h5 = 2 * m_uint8_V8[x_int * 128 + y_int];
            a = h2 - h1;
            b = h5 - h1 - h2;
            c = h1;
        }
        else
        {
            // 2 triangle (h1, h3, h5 points)
            int32 h1 = V9_h1_ptr[0];
            int32 h3 = V9_h1_ptr[1];
            int32 h5 = 2 * m_uint8_V8[x_int * 128 + y_int];
            a = h5 - h1 - h3;
            b = h3 - h1;
            c = h1;
        }
    }
    else
    {
        if (x > y)
        {
            // 3 triangle (h2, h4, h5 points)
            int32 h2 = V9_h1_ptr[129];
            int32 h4 = V9_h1_ptr[130];
            int32 h5 = 2 * m_uint8_V8[x_int * 128 + y_int];
            a = h2 + h4 - h5;
            b = h4 - h2;
            c = h5 - h4;
        }
        else
        {
            // 4 triangle (h3, h4, h5 points)
            int32 h3 = V9_h1_ptr[  1];
            int32 h4 = V9_h1_ptr[130];
            int32 h5 = 2 * m_uint8_V8[x_int * 128 + y_int];
            a = h4 - h3;
            b = h3 + h4 - h5;
            c = h5 - h4;
        }
    }
    // Calculate height
    return (float)((a * x) + (b * y) + c) * _gridIntHeightMultiplier + _gridHeight;
}

float GridMap::getHeightFromUint16(float x, float y) const
{
    if (!m_uint16_V8 || !m_uint16_V9)
        return _gridHeight;

    x = MAP_RESOLUTION * (32 - x / SIZE_OF_GRIDS);
    y = MAP_RESOLUTION * (32 - y / SIZE_OF_GRIDS);

    int x_int = (int)x;
    int y_int = (int)y;
    x -= x_int;
    y -= y_int;
    x_int &= (MAP_RESOLUTION - 1);
    y_int &= (MAP_RESOLUTION - 1);

    if (isHole(x_int, y_int))
        return INVALID_HEIGHT;

    int32 a, b, c;
    uint16* V9_h1_ptr = &m_uint16_V9[x_int * 128 + x_int + y_int];
    if (x + y < 1)
    {
        if (x > y)
        {
            // 1 triangle (h1, h2, h5 points)
            int32 h1 = V9_h1_ptr[  0];
            int32 h2 = V9_h1_ptr[129];
            int32 h5 = 2 * m_uint16_V8[x_int * 128 + y_int];
            a = h2 - h1;
            b = h5 - h1 - h2;
            c = h1;
        }
        else
        {
            // 2 triangle (h1, h3, h5 points)
            int32 h1 = V9_h1_ptr[0];
            int32 h3 = V9_h1_ptr[1];
            int32 h5 = 2 * m_uint16_V8[x_int * 128 + y_int];
            a = h5 - h1 - h3;
            b = h3 - h1;
            c = h1;
        }
    }
    else
    {
        if (x > y)
        {
            // 3 triangle (h2, h4, h5 points)
            int32 h2 = V9_h1_ptr[129];
            int32 h4 = V9_h1_ptr[130];
            int32 h5 = 2 * m_uint16_V8[x_int * 128 + y_int];
            a = h2 + h4 - h5;
            b = h4 - h2;
            c = h5 - h4;
        }
        else
        {
            // 4 triangle (h3, h4, h5 points)
            int32 h3 = V9_h1_ptr[  1];
            int32 h4 = V9_h1_ptr[130];
            int32 h5 = 2 * m_uint16_V8[x_int * 128 + y_int];
            a = h4 - h3;
            b = h3 + h4 - h5;
            c = h5 - h4;
        }
    }
    // Calculate height
    return (float)((a * x) + (b * y) + c) * _gridIntHeightMultiplier + _gridHeight;
}

bool GridMap::isHole(int row, int col) const
{
    if (!_holes)
        return false;

    int cellRow = row / 8; // 8 squares per cell
    int cellCol = col / 8;
    int holeRow = row % 8 / 2;
    int holeCol = (col - (cellCol * 8)) / 2;

    uint16 hole = _holes[cellRow * 16 + cellCol];

    return (hole & holetab_h[holeCol] & holetab_v[holeRow]) != 0;
}

float GridMap::getMinHeight(float x, float y) const
{
    if (!_minHeight)
        return -500.0f;

    static uint32 const indices[] =
    {
        3, 0, 4,
        0, 1, 4,
        1, 2, 4,
        2, 5, 4,
        5, 8, 4,
        8, 7, 4,
        7, 6, 4,
        6, 3, 4
    };

    static float const boundGridCoords[] =
    {
        0.0f, 0.0f,
        0.0f, -266.66666f,
        0.0f, -533.33331f,
        -266.66666f, 0.0f,
        -266.66666f, -266.66666f,
        -266.66666f, -533.33331f,
        -533.33331f, 0.0f,
        -533.33331f, -266.66666f,
        -533.33331f, -533.33331f
    };

    Cell cell(x, y);
    float gx = x - (int32(cell.GridX()) - CENTER_GRID_ID + 1) * SIZE_OF_GRIDS;
    float gy = y - (int32(cell.GridY()) - CENTER_GRID_ID + 1) * SIZE_OF_GRIDS;

    uint32 quarterIndex = 0;
    if (cell.CellY() < MAX_NUMBER_OF_CELLS / 2)
    {
        if (cell.CellX() < MAX_NUMBER_OF_CELLS / 2)
        {
            quarterIndex = 4 + (gy > gx);
        }
        else
            quarterIndex = 2 + ((-SIZE_OF_GRIDS - gx) > gy);
    }
    else if (cell.CellX() < MAX_NUMBER_OF_CELLS / 2)
    {
        quarterIndex = 6 + ((-SIZE_OF_GRIDS - gx) <= gy);
    }
    else
        quarterIndex = gx > gy;

    quarterIndex *= 3;

    return G3D::Plane(
               G3D::Vector3(boundGridCoords[indices[quarterIndex + 0] * 2 + 0], boundGridCoords[indices[quarterIndex + 0] * 2 + 1], _minHeight[indices[quarterIndex + 0]]),
               G3D::Vector3(boundGridCoords[indices[quarterIndex + 1] * 2 + 0], boundGridCoords[indices[quarterIndex + 1] * 2 + 1], _minHeight[indices[quarterIndex + 1]]),
               G3D::Vector3(boundGridCoords[indices[quarterIndex + 2] * 2 + 0], boundGridCoords[indices[quarterIndex + 2] * 2 + 1], _minHeight[indices[quarterIndex + 2]])
           ).distance(G3D::Vector3(gx, gy, 0.0f));
}

float GridMap::getLiquidLevel(float x, float y) const
{
    if (!_liquidMap)
        return _liquidLevel;

    x = MAP_RESOLUTION * (32 - x / SIZE_OF_GRIDS);
    y = MAP_RESOLUTION * (32 - y / SIZE_OF_GRIDS);

    int cx_int = ((int)x & (MAP_RESOLUTION - 1)) - _liquidOffY;
    int cy_int = ((int)y & (MAP_RESOLUTION - 1)) - _liquidOffX;

    if (cx_int < 0 || cx_int >= _liquidHeight)
        return INVALID_HEIGHT;
    if (cy_int < 0 || cy_int >= _liquidWidth)
        return INVALID_HEIGHT;

    return _liquidMap[cx_int * _liquidWidth + cy_int];
}

// Get water state on map
inline LiquidData const GridMap::GetLiquidData(float x, float y, float z, float collisionHeight, uint8 ReqLiquidType) const
{
    LiquidData liquidData;

    // Check water type (if no water return)
    if (_liquidGlobalFlags || _liquidFlags)
    {
        // Get cell
        float cx = MAP_RESOLUTION * (32 - x / SIZE_OF_GRIDS);
        float cy = MAP_RESOLUTION * (32 - y / SIZE_OF_GRIDS);

        int x_int = (int) cx & (MAP_RESOLUTION - 1);
        int y_int = (int) cy & (MAP_RESOLUTION - 1);

        // Check water type in cell
        int idx=(x_int>>3)*16 + (y_int>>3);
        uint8 type = _liquidFlags ? _liquidFlags[idx] : _liquidGlobalFlags;
        uint32 entry = _liquidEntry ? _liquidEntry[idx] : _liquidGlobalEntry;
        //if (LiquidTypeEntry const* liquidEntry = sLiquidTypeStore.LookupEntry(entry))
        //{
        //    type &= MAP_LIQUID_TYPE_DARK_WATER;
        //    uint32 liqTypeIdx = liquidEntry->Type;
        //    if (entry < 21)
        //    {
        //        if (AreaTableEntry const* area = sAreaTableStore.LookupEntry(getArea(x, y)))
        //        {
        //            uint32 overrideLiquid = area->LiquidTypeOverride[liquidEntry->Type];
        //            if (!overrideLiquid && area->zone)
        //            {
        //                area = sAreaTableStore.LookupEntry(area->zone);
        //                if (area)
        //                    overrideLiquid = area->LiquidTypeOverride[liquidEntry->Type];
        //            }

        //            if (LiquidTypeEntry const* liq = sLiquidTypeStore.LookupEntry(overrideLiquid))
        //            {
        //                entry      = overrideLiquid;
        //                liqTypeIdx = liq->Type;
        //            }
        //        }
        //    }

        //    type |= 1 << liqTypeIdx;
        //}

         // Check req liquid type mask
        if (type != 0 && (!ReqLiquidType || (ReqLiquidType & type) != 0))
        {
            // Check water level:
            // Check water height map
            int lx_int = x_int - _liquidOffY;
            int ly_int = y_int - _liquidOffX;
            if (lx_int >= 0 && lx_int < _liquidHeight && ly_int >= 0 && ly_int < _liquidWidth)
            {
                // Get water level
                float liquid_level = _liquidMap ? _liquidMap[lx_int * _liquidWidth + ly_int] : _liquidLevel;
                // Get ground level
                float ground_level = getHeight(x, y);

                // Check water level and ground level (sub 0.2 for fix some errors)
                if (liquid_level >= ground_level && z >= ground_level - 0.2f)
                {
                    // All ok in water -> store data
                    liquidData.Entry  = entry;
                    liquidData.Flags = type;
                    liquidData.Level = liquid_level;
                    liquidData.DepthLevel = ground_level;

                    // For speed check as int values
                    float delta = liquid_level - z;

                    if (delta > collisionHeight)
                        liquidData.Status = LIQUID_MAP_UNDER_WATER;
                    else if (delta > 0.0f)
                        liquidData.Status = LIQUID_MAP_IN_WATER;
                    else if (delta > -0.1f)
                        liquidData.Status = LIQUID_MAP_WATER_WALK;
                    else
                        liquidData.Status = LIQUID_MAP_ABOVE_WATER;
                }
            }
        }
    }

    return liquidData;
}

GridMap* Map::GetGrid(float x, float y)
{
    // half opt method
    int gx = (int)(32 - x / SIZE_OF_GRIDS);                 //grid x
    int gy = (int)(32 - y / SIZE_OF_GRIDS);                 //grid y

    // ensure GridMap is loaded
    EnsureGridCreated(GridCoord(63 - gx, 63 - gy));

    return GridMaps[gx][gy];
}

float Map::GetWaterOrGroundLevel(uint32 phasemask, float x, float y, float z, float* ground /*= nullptr*/, bool /*swim = false*/, float collisionHeight) const
{
    if (const_cast<Map*>(this)->GetGrid(x, y))
    {
        // we need ground level (including grid height version) for proper return water level in point
        float ground_z = GetHeight(phasemask, x, y, z + Z_OFFSET_FIND_HEIGHT, true, 50.0f);
        if (ground)
            *ground = ground_z;

        LiquidData const& liquidData = const_cast<Map*>(this)->GetLiquidData(phasemask, x, y, ground_z, collisionHeight, MAP_ALL_LIQUIDS);
        switch (liquidData.Status)
        {
            case LIQUID_MAP_ABOVE_WATER:
                return std::max<float>(liquidData.Level, ground_z);
            case LIQUID_MAP_NO_WATER:
                return ground_z;
            default:
                return liquidData.Level;
        }
    }

    return VMAP_INVALID_HEIGHT_VALUE;
}

float Map::GetHeight(float x, float y, float z, bool checkVMap /*= true*/, float maxSearchDist /*= DEFAULT_HEIGHT_SEARCH*/) const
{
    // find raw .map surface under Z coordinates
    float mapHeight = VMAP_INVALID_HEIGHT_VALUE;
    float gridHeight = GetGridHeight(x, y);
    if (G3D::fuzzyGe(z, gridHeight - GROUND_HEIGHT_TOLERANCE))
        mapHeight = gridHeight;

    float vmapHeight = VMAP_INVALID_HEIGHT_VALUE;
    // TODO?
    if (checkVMap)
    {
        VMAP::IVMapMgr* vmgr = VMAP::VMapFactory::createOrGetVMapMgr();
        vmapHeight = vmgr->getHeight(GetId(), x, y, z, maxSearchDist);   // look from a bit higher pos to find the floor
    }

    // mapHeight set for any above raw ground Z or <= INVALID_HEIGHT
    // vmapheight set for any under Z value or <= INVALID_HEIGHT
    if (vmapHeight > INVALID_HEIGHT)
    {
        if (mapHeight > INVALID_HEIGHT)
        {
            // we have mapheight and vmapheight and must select more appropriate

            // we are already under the surface or vmap height above map heigt
            // or if the distance of the vmap height is less the land height distance
            if (vmapHeight > mapHeight || std::fabs(mapHeight - z) > std::fabs(vmapHeight - z))
                return vmapHeight;
            else
                return mapHeight;                           // better use .map surface height
        }
        else
            return vmapHeight;                              // we have only vmapHeight (if have)
    }

    return mapHeight;                               // explicitly use map data
}

float Map::GetGridHeight(float x, float y) const
{
    if (GridMap* gmap = const_cast<Map*>(this)->GetGrid(x, y))
        return gmap->getHeight(x, y);

    return VMAP_INVALID_HEIGHT_VALUE;
}

float Map::GetMinHeight(float x, float y) const
{
    if (GridMap const* grid = const_cast<Map*>(this)->GetGrid(x, y))
        return grid->getMinHeight(x, y);

    return -500.0f;
}

static inline bool IsInWMOInterior(uint32 mogpFlags)
{
    return (mogpFlags & 0x2000) != 0;
}

//bool Map::GetAreaInfo(uint32 phaseMask, float x, float y, float z, uint32& flags, int32& adtId, int32& rootId, int32& groupId) const
//{
//    float vmap_z = z;
//    float dynamic_z = z;
//    float check_z = z;
//    VMAP::IVMapMgr* vmgr = VMAP::VMapFactory::createOrGetVMapMgr();
//    uint32 vflags;
//    int32 vadtId;
//    int32 vrootId;
//    int32 vgroupId;
//    uint32 dflags;
//    int32 dadtId;
//    int32 drootId;
//    int32 dgroupId;
//
//    bool hasVmapAreaInfo = vmgr->GetAreaInfo(GetId(), x, y, vmap_z, vflags, vadtId, vrootId, vgroupId);
//    bool hasDynamicAreaInfo = _dynamicTree.GetAreaInfo(x, y, dynamic_z, phaseMask, dflags, dadtId, drootId, dgroupId);
//    auto useVmap = [&]() { check_z = vmap_z; flags = vflags; adtId = vadtId; rootId = vrootId; groupId = vgroupId; };
//    auto useDyn = [&]() { check_z = dynamic_z; flags = dflags; adtId = dadtId; rootId = drootId; groupId = dgroupId; };
//
//    if (hasVmapAreaInfo)
//    {
//        if (hasDynamicAreaInfo && dynamic_z > vmap_z)
//            useDyn();
//        else
//            useVmap();
//    }
//    else if (hasDynamicAreaInfo)
//    {
//        useDyn();
//    }
//
//    if (hasVmapAreaInfo || hasDynamicAreaInfo)
//    {
//        // check if there's terrain between player height and object height
//        if (GridMap* gmap = const_cast<Map*>(this)->GetGrid(x, y))
//        {
//            float mapHeight = gmap->getHeight(x, y);
//            // z + 2.0f condition taken from GetHeight(), not sure if it's such a great choice...
//            if (z + 2.0f > mapHeight && mapHeight > check_z)
//                return false;
//        }
//
//        return true;
//    }
//
//    return false;
//}

//uint32 Map::GetAreaId(uint32 phaseMask, float x, float y, float z) const
//{
//    uint32 mogpFlags;
//    int32 adtId, rootId, groupId;
//    float vmapZ = z;
//    bool hasVmapArea = GetAreaInfo(phaseMask, x, y, vmapZ, mogpFlags, adtId, rootId, groupId);
//
//    uint32 gridAreaId    = 0;
//    float  gridMapHeight = INVALID_HEIGHT;
//    if (GridMap* gmap = const_cast<Map*>(this)->GetGrid(x, y))
//    {
//        gridAreaId    = gmap->getArea(x, y);
//        gridMapHeight = gmap->getHeight(x, y);
//    }
//
//    uint16 areaId = 0;
//
//    // floor is the height we are closer to (but only if above)
//    if (hasVmapArea && G3D::fuzzyGe(z, vmapZ - GROUND_HEIGHT_TOLERANCE) && (G3D::fuzzyLt(z, gridMapHeight - GROUND_HEIGHT_TOLERANCE) || vmapZ > gridMapHeight))
//    {
//        // wmo found
//        if (WMOAreaTableEntry const* wmoEntry = GetWMOAreaTableEntryByTripple(rootId, adtId, groupId))
//            areaId = wmoEntry->areaId;
//
//        if (!areaId)
//            areaId = gridAreaId;
//    }
//    else
//        areaId = gridAreaId;
//
//    if (!areaId)
//        areaId = i_mapEntry->linked_zone;
//
//    return areaId;
//}

//uint32 Map::GetZoneId(uint32 phaseMask, float x, float y, float z) const
//{
//    uint32 areaId = GetAreaId(phaseMask, x, y, z);
//    if (AreaTableEntry const* area = sAreaTableStore.LookupEntry(areaId))
//        if (area->zone)
//            return area->zone;
//
//    return areaId;
//}

//void Map::GetZoneAndAreaId(uint32 phaseMask, uint32& zoneid, uint32& areaid, float x, float y, float z) const
//{
//    areaid = zoneid = GetAreaId(phaseMask, x, y, z);
//    if (AreaTableEntry const* area = sAreaTableStore.LookupEntry(areaid))
//        if (area->zone)
//            zoneid = area->zone;
//}
//
//LiquidData const Map::GetLiquidData(uint32 phaseMask, float x, float y, float z, float collisionHeight, uint8 ReqLiquidType)
//{
//   LiquidData liquidData;
//
//    VMAP::IVMapMgr* vmgr = VMAP::VMapFactory::createOrGetVMapMgr();
//    float liquid_level = INVALID_HEIGHT;
//    float ground_level = INVALID_HEIGHT;
//    uint32 liquid_type = 0;
//    uint32 mogpFlags = 0;
//    bool useGridLiquid = true;
//    if (vmgr->GetLiquidLevel(GetId(), x, y, z, ReqLiquidType, liquid_level, ground_level, liquid_type, mogpFlags))
//    {
//        useGridLiquid = !IsInWMOInterior(mogpFlags);
//        //LOG_DEBUG("maps", "GetLiquidStatus(): vmap liquid level: {} ground: {} type: {}", liquid_level, ground_level, liquid_type);
//        // Check water level and ground level
//        if (liquid_level > ground_level && G3D::fuzzyGe(z, ground_level - GROUND_HEIGHT_TOLERANCE))
//        {
//            // hardcoded in client like this
//            if (GetId() == 530 && liquid_type == 2)
//                liquid_type = 15;
//
//            uint32 liquidFlagType = 0;
//            if (LiquidTypeEntry const* liq = sLiquidTypeStore.LookupEntry(liquid_type))
//                liquidFlagType = liq->Type;
//
//            if (liquid_type && liquid_type < 21)
//            {
//                if (AreaTableEntry const* area = sAreaTableStore.LookupEntry(GetAreaId(phaseMask, x, y, z)))
//                {
//                    uint32 overrideLiquid = area->LiquidTypeOverride[liquidFlagType];
//                    if (!overrideLiquid && area->zone)
//                    {
//                        area = sAreaTableStore.LookupEntry(area->zone);
//                        if (area)
//                            overrideLiquid = area->LiquidTypeOverride[liquidFlagType];
//                    }
//
//                    if (LiquidTypeEntry const* liq = sLiquidTypeStore.LookupEntry(overrideLiquid))
//                    {
//                        liquid_type = overrideLiquid;
//                        liquidFlagType = liq->Type;
//                    }
//                }
//            }
//
//            liquidData.Level = liquid_level;
//            liquidData.DepthLevel = ground_level;
//            liquidData.Entry = liquid_type;
//            liquidData.Flags = 1 << liquidFlagType;
//        }
//
//        float delta = liquid_level - z;
//
//        // Get position delta
//        if (delta > collisionHeight)
//            liquidData.Status = LIQUID_MAP_UNDER_WATER;
//        else if (delta > 0.0f)
//            liquidData.Status = LIQUID_MAP_IN_WATER;
//        else if (delta > -0.1f)
//            liquidData.Status = LIQUID_MAP_WATER_WALK;
//        else
//            liquidData.Status = LIQUID_MAP_ABOVE_WATER;
//    }
//
//    if (useGridLiquid)
//    {
//        if (GridMap* gmap = const_cast<Map*>(this)->GetGrid(x, y))
//        {
//            LiquidData const& map_data = gmap->GetLiquidData(x, y, z, collisionHeight, ReqLiquidType);
//            // Not override LIQUID_MAP_ABOVE_WATER with LIQUID_MAP_NO_WATER:
//            if (map_data.Status != LIQUID_MAP_NO_WATER && (map_data.Level > ground_level))
//            {
//                // hardcoded in client like this
//                uint32 liquidEntry = map_data.Entry;
//                if (GetId() == 530 && liquidEntry == 2)
//                    liquidEntry = 15;
//
//                liquidData = map_data;
//                liquidData.Entry = liquidEntry;
//            }
//        }
//    }
//
//   return liquidData;
//}

//void Map::GetFullTerrainStatusForPosition(uint32 /*phaseMask*/, float x, float y, float z, float collisionHeight, PositionFullTerrainStatus& data, uint8 reqLiquidType)
//{
//    GridMap* gmap = GetGrid(x, y);
//
//    VMAP::IVMapMgr* vmgr = VMAP::VMapFactory::createOrGetVMapMgr();
//    VMAP::AreaAndLiquidData vmapData;
//    // VMAP::AreaAndLiquidData dynData;
//    VMAP::AreaAndLiquidData* wmoData = nullptr;
//    vmgr->GetAreaAndLiquidData(GetId(), x, y, z, reqLiquidType, vmapData);
//    // _dynamicTree.GetAreaAndLiquidData(x, y, z, phaseMask, reqLiquidType, dynData);
//
//    uint32 gridAreaId = 0;
//    float gridMapHeight = INVALID_HEIGHT;
//    if (gmap)
//    {
//        gridAreaId = gmap->getArea(x, y);
//        gridMapHeight = gmap->getHeight(x, y);
//    }
//
//    bool useGridLiquid = true;
//
//    // floor is the height we are closer to (but only if above)
//    data.floorZ = VMAP_INVALID_HEIGHT;
//    if (gridMapHeight > INVALID_HEIGHT && G3D::fuzzyGe(z, gridMapHeight - GROUND_HEIGHT_TOLERANCE))
//        data.floorZ = gridMapHeight;
//
//    if (vmapData.floorZ > VMAP_INVALID_HEIGHT && G3D::fuzzyGe(z, vmapData.floorZ - GROUND_HEIGHT_TOLERANCE) &&
//        (G3D::fuzzyLt(z, gridMapHeight - GROUND_HEIGHT_TOLERANCE) || vmapData.floorZ > gridMapHeight))
//    {
//        data.floorZ = vmapData.floorZ;
//        wmoData = &vmapData;
//    }
//
//    // NOTE: Objects will not detect a case when a wmo providing area/liquid despawns from under them
//    // but this is fine as these kind of objects are not meant to be spawned and despawned a lot
//    // example: Lich King platform
//    /*
//    if (dynData.floorZ > VMAP_INVALID_HEIGHT && G3D::fuzzyGe(z, dynData.floorZ - GROUND_HEIGHT_TOLERANCE) &&
//        (G3D::fuzzyLt(z, gridMapHeight - GROUND_HEIGHT_TOLERANCE) || dynData.floorZ > gridMapHeight) &&
//        (G3D::fuzzyLt(z, vmapData.floorZ - GROUND_HEIGHT_TOLERANCE) || dynData.floorZ > vmapData.floorZ))
//    {
//        data.floorZ = dynData.floorZ;
//        wmoData = &dynData;
//    }
//    */
//
//    if (wmoData)
//    {
//        if (wmoData->areaInfo)
//        {
//            // wmo found
//            WMOAreaTableEntry const* wmoEntry = GetWMOAreaTableEntryByTripple(wmoData->areaInfo->rootId, wmoData->areaInfo->adtId, wmoData->areaInfo->groupId);
//            data.outdoors = (wmoData->areaInfo->mogpFlags & 0x8) != 0;
//            if (wmoEntry)
//            {
//                data.areaId = wmoEntry->areaId;
//                if (wmoEntry->Flags & 4)
//                    data.outdoors = true;
//                else if (wmoEntry->Flags & 2)
//                    data.outdoors = false;
//            }
//
//            if (!data.areaId)
//                data.areaId = gridAreaId;
//
//            useGridLiquid = !IsInWMOInterior(wmoData->areaInfo->mogpFlags);
//        }
//    }
//    else
//    {
//        data.outdoors = true;
//        data.areaId = gridAreaId;
//        if (AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(data.areaId))
//            data.outdoors = (areaEntry->flags & (AREA_FLAG_INSIDE | AREA_FLAG_OUTSIDE)) != AREA_FLAG_INSIDE;
//    }
//
//    if (!data.areaId)
//        data.areaId = i_mapEntry->linked_zone;
//
//    AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(data.areaId);
//
//    // liquid processing
//    if (wmoData && wmoData->liquidInfo && wmoData->liquidInfo->level > wmoData->floorZ)
//    {
//        uint32 liquidType = wmoData->liquidInfo->type;
//        if (GetId() == 530 && liquidType == 2) // gotta love blizzard hacks
//            liquidType = 15;
//
//        uint32 liquidFlagType = 0;
//        if (LiquidTypeEntry const* liquidData = sLiquidTypeStore.LookupEntry(liquidType))
//            liquidFlagType = liquidData->Type;
//
//        if (liquidType && liquidType < 21 && areaEntry)
//        {
//            uint32 overrideLiquid = areaEntry->LiquidTypeOverride[liquidFlagType];
//            if (!overrideLiquid && areaEntry->zone)
//            {
//                AreaTableEntry const* zoneEntry = sAreaTableStore.LookupEntry(areaEntry->zone);
//                if (zoneEntry)
//                    overrideLiquid = zoneEntry->LiquidTypeOverride[liquidFlagType];
//            }
//
//            if (LiquidTypeEntry const* overrideData = sLiquidTypeStore.LookupEntry(overrideLiquid))
//            {
//                liquidType = overrideLiquid;
//                liquidFlagType = overrideData->Type;
//            }
//        }
//
//        data.liquidInfo.Level = wmoData->liquidInfo->level;
//        data.liquidInfo.DepthLevel = wmoData->floorZ;
//        data.liquidInfo.Entry = liquidType;
//        data.liquidInfo.Flags = 1 << liquidFlagType;
//
//        // Get position delta
//        float delta = wmoData->liquidInfo->level - z;
//
//        if (delta > collisionHeight)
//            data.liquidInfo.Status = LIQUID_MAP_UNDER_WATER;
//        else if (delta > 0.0f)
//            data.liquidInfo.Status = LIQUID_MAP_IN_WATER;
//        else if (delta > -0.1f)
//            data.liquidInfo.Status = LIQUID_MAP_WATER_WALK;
//        else
//            data.liquidInfo.Status = LIQUID_MAP_ABOVE_WATER;
//    }
//
//    // look up liquid data from grid map
//    if (gmap && useGridLiquid)
//    {
//        LiquidData const& gridLiquidData = gmap->GetLiquidData(x, y, z, collisionHeight, reqLiquidType);
//        if (gridLiquidData.Status != LIQUID_MAP_NO_WATER && (!wmoData || gridLiquidData.Level > wmoData->floorZ))
//        {
//            uint32 liquidEntry = gridLiquidData.Entry;
//            if (GetId() == 530 && liquidEntry == 2)
//                liquidEntry = 15;
//
//            data.liquidInfo = gridLiquidData;
//            data.liquidInfo.Entry = liquidEntry;
//        }
//    }
//}

float Map::GetWaterLevel(float x, float y) const
{
    if (GridMap* gmap = const_cast<Map*>(this)->GetGrid(x, y))
        return gmap->getLiquidLevel(x, y);
    else
        return 0;
}

//bool Map::isInLineOfSight(float x1, float y1, float z1, float x2, float y2, float z2, uint32 phasemask, LineOfSightChecks checks, VMAP::ModelIgnoreFlags ignoreFlags) const
//{
//    if (!sWorld->getBoolConfig(CONFIG_VMAP_BLIZZLIKE_PVP_LOS))
//    {
//        if (IsBattlegroundOrArena())
//        {
//            ignoreFlags = VMAP::ModelIgnoreFlags::Nothing;
//        }
//    }
//
//    if ((checks & LINEOFSIGHT_CHECK_VMAP) && !VMAP::VMapFactory::createOrGetVMapMgr()->isInLineOfSight(GetId(), x1, y1, z1, x2, y2, z2, ignoreFlags))
//    {
//        return false;
//    }
//
//    if (sWorld->getBoolConfig(CONFIG_CHECK_GOBJECT_LOS) && (checks & LINEOFSIGHT_CHECK_GOBJECT_ALL))
//    {
//        ignoreFlags = VMAP::ModelIgnoreFlags::Nothing;
//        if (!(checks & LINEOFSIGHT_CHECK_GOBJECT_M2))
//        {
//            ignoreFlags = VMAP::ModelIgnoreFlags::M2;
//        }
//
//        if (!_dynamicTree.isInLineOfSight(x1, y1, z1, x2, y2, z2, phasemask, ignoreFlags))
//        {
//            return false;
//        }
//    }
//
//    return true;
//}

bool Map::GetObjectHitPos(uint32 phasemask, float x1, float y1, float z1, float x2, float y2, float z2, float& rx, float& ry, float& rz, float modifyDist)
{
    G3D::Vector3 startPos(x1, y1, z1);
    G3D::Vector3 dstPos(x2, y2, z2);

    G3D::Vector3 resultPos;
    bool result = _dynamicTree.GetObjectHitPos(phasemask, startPos, dstPos, resultPos, modifyDist);

    rx = resultPos.x;
    ry = resultPos.y;
    rz = resultPos.z;
    return result;
}

float Map::GetHeight(uint32 phasemask, float x, float y, float z, bool vmap/*=true*/, float maxSearchDist /*= DEFAULT_HEIGHT_SEARCH*/) const
{
    float h1, h2;
    h1 = GetHeight(x, y, z, vmap, maxSearchDist);
    h2 = _dynamicTree.getHeight(x, y, z, maxSearchDist, phasemask);
    return std::max<float>(h1, h2);
}

bool Map::IsInWater(uint32 phaseMask, float x, float y, float pZ, float collisionHeight) const
{
    LiquidData const& liquidData = const_cast<Map*>(this)->GetLiquidData(phaseMask, x, y, pZ, collisionHeight, MAP_ALL_LIQUIDS);
    return (liquidData.Status & MAP_LIQUID_STATUS_SWIMMING) != 0;
}

bool Map::IsUnderWater(uint32 phaseMask, float x, float y, float z, float collisionHeight) const
{
    LiquidData const& liquidData = const_cast<Map*>(this)->GetLiquidData(phaseMask, x, y, z, collisionHeight, MAP_LIQUID_TYPE_WATER | MAP_LIQUID_TYPE_OCEAN);
    return liquidData.Status == LIQUID_MAP_UNDER_WATER;
}

bool Map::HasEnoughWater(WorldObject const* searcher, float x, float y, float z) const
{
    LiquidData const& liquidData = const_cast<Map*>(this)->GetLiquidData(searcher->GetPhaseMask(), x, y, z, searcher->GetCollisionHeight(), MAP_ALL_LIQUIDS);
    return (liquidData.Status & MAP_LIQUID_STATUS_SWIMMING) != 0 && HasEnoughWater(searcher, liquidData);
}

bool Map::HasEnoughWater(WorldObject const* searcher, LiquidData const& liquidData) const
{
    float minHeightInWater = searcher->GetMinHeightInWater();
    return liquidData.Level > INVALID_HEIGHT && liquidData.Level > liquidData.DepthLevel && liquidData.Level - liquidData.DepthLevel >= minHeightInWater;
}

//char const* Map::GetMapName() const
//{
//    return i_mapEntry ? i_mapEntry->name[sWorld->GetDefaultDbcLocale()] : "UNNAMEDMAP\x0";
//}

inline void Map::setNGrid(NGridType* grid, uint32 x, uint32 y)
{
    if (x >= MAX_NUMBER_OF_GRIDS || y >= MAX_NUMBER_OF_GRIDS)
    {
        //LOG_ERROR("maps", "map::setNGrid() Invalid grid coordinates found: {}, {}!", x, y);
        //ABORT();
        std::cout << "map::setNGrid() Invalid grid coordinates found: " << x << ", " << y << std::endl;
        return;
    }
    i_grids[x][y] = grid;
}

/**
 * @brief Check if a given source can reach a specific point following a path
 * and normalize the coords. Use this method for long paths, otherwise use the
 * overloaded method with the start coords when you need to do a quick check on small segments
 *
 */
bool Map::CanReachPositionAndGetValidCoords(WorldObject const* source, PathGenerator *path, float &destX, float &destY, float &destZ, bool failOnCollision, bool failOnSlopes) const
{
    G3D::Vector3 prevPath = path->GetStartPosition();
    for (auto& vector : path->GetPath())
    {
        float x = vector.x;
        float y = vector.y;
        float z = vector.z;

        if (!CanReachPositionAndGetValidCoords(source, prevPath.x, prevPath.y, prevPath.z, x, y, z, failOnCollision, failOnSlopes))
        {
            destX = x;
            destY = y;
            destZ = z;
            return false;
        }

        prevPath = vector;
    }

    destX = prevPath.x;
    destY = prevPath.y;
    destZ = prevPath.z;

    return true;
}

/**
 * @brief validate the new destination and set reachable coords
 * Check if a given unit can reach a specific point on a segment
 * and set the correct dest coords
 * NOTE: use this method with small segments.
 *
 * @param failOnCollision if true, the methods will return false when a collision occurs
 * @param failOnSlopes if true, the methods will return false when a non walkable slope is found
 *
 * @return true if the destination is valid, false otherwise
 *
 **/

bool Map::CanReachPositionAndGetValidCoords(WorldObject const* source, float& destX, float& destY, float& destZ, bool failOnCollision, bool failOnSlopes) const
{
    return CanReachPositionAndGetValidCoords(source, source->GetPositionX(), source->GetPositionY(), source->GetPositionZ(), destX, destY, destZ, failOnCollision, failOnSlopes);
}

bool Map::CanReachPositionAndGetValidCoords(WorldObject const* source, float startX, float startY, float startZ, float &destX, float &destY, float &destZ, bool failOnCollision, bool failOnSlopes) const
{
    if (!CheckCollisionAndGetValidCoords(source, startX, startY, startZ, destX, destY, destZ, failOnCollision))
    {
        return false;
    }

    Unit const* unit = source->ToUnit();
    // if it's not an unit (Object) then we do not have to continue
    // with walkable checks
    //if (!unit)
    //{
    //    return true;
    //}

    /*
     * Walkable checks
     */
    bool isWaterNext = HasEnoughWater(unit, destX, destY, destZ);
    //Creature const* creature = unit->ToCreature();
    //bool cannotEnterWater = isWaterNext && (creature && !creature->CanEnterWater());
    bool cannotEnterWater = false;
    //bool cannotWalkOrFly = !isWaterNext && !source->ToPlayer() && !unit->CanFly() && (creature && !creature->CanWalk());
    //bool cannotWalkOrFly = !isWaterNext && !source->ToPlayer() && !unit->CanFly();
    bool cannotWalkOrFly = false;

    if (cannotEnterWater || cannotWalkOrFly ||
        (failOnSlopes && !PathGenerator::IsWalkableClimb(startX, startY, startZ, destX, destY, destZ, source->GetCollisionHeight())))
    {
        return false;
    }

    return true;
}

/**
 * @brief validate the new destination and set coords
 * Check if a given unit can face collisions in a specific segment
 *
 * @return true if the destination is valid, false otherwise
 *
 **/
bool Map::CheckCollisionAndGetValidCoords(WorldObject const* source, float startX, float startY, float startZ, float &destX, float &destY, float &destZ, bool failOnCollision) const
{
    // Prevent invalid coordinates here, position is unchanged
    if (!Acore::IsValidMapCoord(startX, startY, startZ) || !Acore::IsValidMapCoord(destX, destY, destZ))
    {
        //LOG_FATAL("maps", "Map::CheckCollisionAndGetValidCoords invalid coordinates startX: {}, startY: {}, startZ: {}, destX: {}, destY: {}, destZ: {}", startX, startY, startZ, destX, destY, destZ);
        std::cout << "NOT VALID CORD" << std::endl;
        return false;
    }

    bool isWaterNext = IsInWater(source->GetPhaseMask(), destX, destY, destZ, source->GetCollisionHeight());

    //PathGenerator path(source);
    PathGenerator path(source, GetId(), GetId());

    // Use a detour raycast to get our first collision point
    path.SetUseRaycast(true);
    bool result = path.CalculatePath(startX, startY, startZ, destX, destY, destZ, false);
    bool doPrint = false;

    if (doPrint)
    {
		Movement::PointsArray myPath = path.GetPath();
		size_t pathSize = myPath.size();
		std::cout << "PATH len: " << pathSize << std::endl;
		for (int i = 0; i < pathSize; ++i) {
			std::cout << "Point " << i + 1 << ": "
				<< "X=" << myPath[i].x << ", "
				<< "Y=" << myPath[i].y << ", "
				<< "Z=" << myPath[i].z << std::endl;
		}
    }

    Unit const* unit = source->ToUnit();
    bool notOnGround = path.GetPathType() & PATHFIND_NOT_USING_PATH
        || isWaterNext || (unit && unit->IsFlying());

    //G3D::Vector3 slopeTest = path.GetPath().back();
    //// Hacky fix for fails on hillside
    //if (startZ - slopeTest.z < 1.0f)
	//{
		// Check for valid path types before we proceeid
		if (!result || (!notOnGround && path.GetPathType() & ~(PATHFIND_NORMAL | PATHFIND_SHORTCUT | PATHFIND_INCOMPLETE | PATHFIND_FARFROMPOLY_END)))
		{
			return false;
		}
	//}

    G3D::Vector3 endPos = path.GetPath().back();
    destX = endPos.x;
    destY = endPos.y;
    destZ = endPos.z;

    // collision check
    bool collided = false;

    // check static LOS
    float halfHeight = source->GetCollisionHeight() * 0.5f;

    // Unit is not on the ground, check for potential collision via vmaps
    // TODO?
    if (notOnGround)
    {
        bool col = VMAP::VMapFactory::createOrGetVMapMgr()->GetObjectHitPos(source->GetMapId(),
            startX, startY, startZ + halfHeight,
            destX, destY, destZ + halfHeight,
            destX, destY, destZ, -CONTACT_DISTANCE);

        destZ -= halfHeight;

        // Collided with static LOS object, move back to collision point
        if (col)
        {
            collided = true;
        }
    }

    // check dynamic collision
    bool col = source->GetMap()->GetObjectHitPos(source->GetPhaseMask(),
        startX, startY, startZ + halfHeight,
        destX, destY, destZ + halfHeight,
        destX, destY, destZ, -CONTACT_DISTANCE);

    destZ -= halfHeight;

    // Collided with a gameobject, move back to collision point
    if (col)
    {
        collided = true;
    }

    float groundZ = VMAP_INVALID_HEIGHT_VALUE;
    source->UpdateAllowedPositionZ(destX, destY, destZ, &groundZ);
    //source->SetPositionZNew(groundZ);
    source->SetPositionZNew(destZ);

	float gridHeightTest = GetGridHeight(destX, destY);
    // position has no ground under it (or is too far away)
    //if (groundZ <= INVALID_HEIGHT && unit && !unit->CanFly())
    if (groundZ <= INVALID_HEIGHT)
    {
        // fall back to gridHeight if any
        float gridHeight = GetGridHeight(destX, destY);
        if (gridHeight > INVALID_HEIGHT)
        {
            destZ = gridHeight + unit->GetHoverHeight();
        }
        else
        {
            return false;
        }
    }

    //return !failOnCollision || !collided;
    if (failOnCollision && collided) {
        return false;
    }
    else {
        return true;
    }
    //return !(failOnCollision && collided); // Using De Morgan's Laws to convert logic to an equivalent form
    //return (failOnCollision && collided) ? false : true; // Ternary Operator
}

