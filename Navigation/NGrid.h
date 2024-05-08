#ifndef ACORE_NGRID_H
#define ACORE_NGRID_H

/** NGrid is nothing more than a wrapper of the Grid with an NxN cells
 */

#include "Grid.h"
#include "GridReference.h"
#include "Util.h"

template
<
    uint32 N,
    class ACTIVE_OBJECT,
    class WORLD_OBJECT_TYPES,
    class GRID_OBJECT_TYPES
    >
class NGrid
{
public:
    typedef Grid<ACTIVE_OBJECT, WORLD_OBJECT_TYPES, GRID_OBJECT_TYPES> GridType;
    NGrid(uint32 id, int32 x, int32 y)
        : i_gridId(id), i_x(x), i_y(y), i_GridObjectDataLoaded(false)
    {
    }

    GridType& GetGridType(const uint32 x, const uint32 y)
    {
        //ASSERT(x < N && y < N);
        return i_cells[x][y];
    }

    [[nodiscard]] GridType const& GetGridType(const uint32 x, const uint32 y) const
    {
        //ASSERT(x < N && y < N);
        return i_cells[x][y];
    }

    [[nodiscard]] uint32 GetGridId() const { return i_gridId; }
    [[nodiscard]] int32 getX() const { return i_x; }
    [[nodiscard]] int32 getY() const { return i_y; }

    void link(GridRefMgr<NGrid<N, ACTIVE_OBJECT, WORLD_OBJECT_TYPES, GRID_OBJECT_TYPES> >* pTo)
    {
        i_Reference.link(pTo, this);
    }
    [[nodiscard]] bool isGridObjectDataLoaded() const { return i_GridObjectDataLoaded; }
    void setGridObjectDataLoaded(bool pLoaded) { i_GridObjectDataLoaded = pLoaded; }

    /*
    template<class SPECIFIC_OBJECT> void AddWorldObject(const uint32 x, const uint32 y, SPECIFIC_OBJECT *obj)
    {
        GetGridType(x, y).AddWorldObject(obj);
    }

    template<class SPECIFIC_OBJECT> void RemoveWorldObject(const uint32 x, const uint32 y, SPECIFIC_OBJECT *obj)
    {
        GetGridType(x, y).RemoveWorldObject(obj);
    }

    template<class SPECIFIC_OBJECT> void AddGridObject(const uint32 x, const uint32 y, SPECIFIC_OBJECT *obj)
    {
        GetGridType(x, y).AddGridObject(obj);
    }

    template<class SPECIFIC_OBJECT> void RemoveGridObject(const uint32 x, const uint32 y, SPECIFIC_OBJECT *obj)
    {
        GetGridType(x, y).RemoveGridObject(obj);
    }
    */


private:
    uint32 i_gridId;
    GridReference<NGrid<N, ACTIVE_OBJECT, WORLD_OBJECT_TYPES, GRID_OBJECT_TYPES> > i_Reference;
    int32 i_x;
    int32 i_y;
    GridType i_cells[N][N];
    bool i_GridObjectDataLoaded;
};
#endif
