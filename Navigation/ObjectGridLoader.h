#ifndef ACORE_OBJECTGRIDLOADER_H
#define ACORE_OBJECTGRIDLOADER_H

#include "Cell.h"
#include "Define.h"
#include "GridDefines.h"
#include "GridLoader.h"
#include "TypeList.h"

class ObjectWorldLoader;

class ObjectGridLoader
{
    friend class ObjectWorldLoader;

public:
    ObjectGridLoader(NGridType& grid, Map* map, const Cell& cell)
        : i_cell(cell), i_grid(grid), i_map(map), i_gameObjects(0), i_creatures(0), i_corpses (0)
    {}

    //void Visit(GameObjectMapType& m);
    //void Visit(CreatureMapType& m);
    //void Visit(CorpseMapType&) const {}
    //void Visit(DynamicObjectMapType&) const {}

	void LoadN(void)
	{
		i_gameObjects = 0;
		i_creatures = 0;
		i_corpses = 0;
		i_cell.data.Part.cell_y = 0;
		for (uint32 x = 0; x < MAX_NUMBER_OF_CELLS; ++x)
		{
			i_cell.data.Part.cell_x = x;
			for (uint32 y = 0; y < MAX_NUMBER_OF_CELLS; ++y)
			{
				i_cell.data.Part.cell_y = y;

				//Load corpses (not bones)
				//{
				//	ObjectWorldLoader worker(*this);
				//	TypeContainerVisitor<ObjectWorldLoader, WorldTypeMapContainer> visitor(worker);
				//	i_grid.VisitGrid(x, y, visitor);
				//}
			}
		}
		//LOG_DEBUG("maps", "{} GameObjects, {} Creatures, and {} Corpses/Bones loaded for grid {} on map {}", i_gameObjects, i_creatures, i_corpses, i_grid.GetGridId(), i_map->GetId());
	}

    template<class T> static void SetObjectCell(T* obj, CellCoord const& cellCoord);

private:
    Cell i_cell;
    NGridType& i_grid;
    Map* i_map;
    uint32 i_gameObjects;
    uint32 i_creatures;
    uint32 i_corpses;
};

//Clean up and remove from world
class ObjectGridCleaner
{
public:
    template<class T> void Visit(GridRefMgr<T>&);
};

#endif
