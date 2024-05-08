#ifndef ACORE_CELL_H
#define ACORE_CELL_H

#include "GridDefines.h"
#include "TypeContainer.h"
#include "TypeContainerVisitor.h"

class Map;
class WorldObject;

struct CellArea
{
    CellArea() = default;
    CellArea(CellCoord low, CellCoord high) : low_bound(low), high_bound(high) {}

    bool operator!() const { return low_bound == high_bound; }

    void ResizeBorders(CellCoord& begin_cell, CellCoord& end_cell) const
    {
        begin_cell = low_bound;
        end_cell = high_bound;
    }

    CellCoord low_bound;
    CellCoord high_bound;
};

struct Cell
{
    Cell() { data.All = 0; }
    Cell(Cell const& cell) { data.All = cell.data.All; }
    explicit Cell(CellCoord const& p);
    explicit Cell(float x, float y);

    void Compute(uint32& x, uint32& y) const
    {
        x = data.Part.grid_x * MAX_NUMBER_OF_CELLS + data.Part.cell_x;
        y = data.Part.grid_y * MAX_NUMBER_OF_CELLS + data.Part.cell_y;
    }

    [[nodiscard]] bool DiffCell(const Cell& cell) const
    {
        return(data.Part.cell_x != cell.data.Part.cell_x ||
               data.Part.cell_y != cell.data.Part.cell_y);
    }

    [[nodiscard]] bool DiffGrid(const Cell& cell) const
    {
        return(data.Part.grid_x != cell.data.Part.grid_x ||
               data.Part.grid_y != cell.data.Part.grid_y);
    }

    [[nodiscard]] uint32 CellX() const { return data.Part.cell_x; }
    [[nodiscard]] uint32 CellY() const { return data.Part.cell_y; }
    [[nodiscard]] uint32 GridX() const { return data.Part.grid_x; }
    [[nodiscard]] uint32 GridY() const { return data.Part.grid_y; }
    [[nodiscard]] bool NoCreate() const { return data.Part.nocreate; }
    void SetNoCreate() { data.Part.nocreate = 1; }

    [[nodiscard]] CellCoord GetCellCoord() const
    {
        return CellCoord(
                   data.Part.grid_x * MAX_NUMBER_OF_CELLS + data.Part.cell_x,
                   data.Part.grid_y * MAX_NUMBER_OF_CELLS + data.Part.cell_y);
    }

    Cell& operator=(Cell const& cell)
    {
        this->data.All = cell.data.All;
        return *this;
    }

    bool operator == (Cell const& cell) const { return (data.All == cell.data.All); }
    bool operator != (Cell const& cell) const { return !operator == (cell); }
    union
    {
        struct
        {
            unsigned grid_x : 6;
            unsigned grid_y : 6;
            unsigned cell_x : 6;
            unsigned cell_y : 6;
            unsigned nocreate : 1;
            unsigned reserved : 7;
        } Part;
        uint32 All;
    } data;

    static CellArea CalculateCellArea(float x, float y, float radius);

    template<class T> static void VisitGridObjects(WorldObject const* obj, T& visitor, float radius, bool dont_load = true);
    template<class T> static void VisitWorldObjects(WorldObject const* obj, T& visitor, float radius, bool dont_load = true);
    template<class T> static void VisitAllObjects(WorldObject const* obj, T& visitor, float radius, bool dont_load = true);

    template<class T> static void VisitGridObjects(float x, float y, Map* map, T& visitor, float radius, bool dont_load = true);
    template<class T> static void VisitWorldObjects(float x, float y, Map* map, T& visitor, float radius, bool dont_load = true);
    template<class T> static void VisitAllObjects(float x, float y, Map* map, T& visitor, float radius, bool dont_load = true);

private:
};

#endif
