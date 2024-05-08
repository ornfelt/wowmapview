#ifndef ACORE_CELLIMPL_H
#define ACORE_CELLIMPL_H

#include "Cell.h"
#include "Map.h"
#include "Object.h"
#include <cmath>

inline Cell::Cell(CellCoord const& p)
{
    data.Part.grid_x = p.x_coord / MAX_NUMBER_OF_CELLS;
    data.Part.grid_y = p.y_coord / MAX_NUMBER_OF_CELLS;
    data.Part.cell_x = p.x_coord % MAX_NUMBER_OF_CELLS;
    data.Part.cell_y = p.y_coord % MAX_NUMBER_OF_CELLS;
    data.Part.nocreate = 0;
    data.Part.reserved = 0;
}

inline Cell::Cell(float x, float y)
{
    CellCoord p = Acore::ComputeCellCoord(x, y);
    data.Part.grid_x = p.x_coord / MAX_NUMBER_OF_CELLS;
    data.Part.grid_y = p.y_coord / MAX_NUMBER_OF_CELLS;
    data.Part.cell_x = p.x_coord % MAX_NUMBER_OF_CELLS;
    data.Part.cell_y = p.y_coord % MAX_NUMBER_OF_CELLS;
    data.Part.nocreate = 0;
    data.Part.reserved = 0;
}

inline CellArea Cell::CalculateCellArea(float x, float y, float radius)
{
    if (radius <= 0.0f)
    {
        CellCoord center = Acore::ComputeCellCoord(x, y).normalize();
        return CellArea(center, center);
    }

    CellCoord centerX = Acore::ComputeCellCoord(x - radius, y - radius).normalize();
    CellCoord centerY = Acore::ComputeCellCoord(x + radius, y + radius).normalize();

    return CellArea(centerX, centerY);
}

#endif
