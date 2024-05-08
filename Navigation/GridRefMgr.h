#ifndef _GRIDREFMANAGER
#define _GRIDREFMANAGER

#include "RefMgr.h"

template<class OBJECT>
class GridReference;

template<class OBJECT>
class GridRefMgr : public RefMgr<GridRefMgr<OBJECT>, OBJECT>
{
public:
    typedef LinkedListHead::Iterator< GridReference<OBJECT> > iterator;

    GridReference<OBJECT>* getFirst() { return (GridReference<OBJECT>*)RefMgr<GridRefMgr<OBJECT>, OBJECT>::getFirst(); }
    GridReference<OBJECT>* getLast() { return (GridReference<OBJECT>*)RefMgr<GridRefMgr<OBJECT>, OBJECT>::getLast(); }

    iterator begin() { return iterator(getFirst()); }
    iterator end() { return iterator(nullptr); }
    iterator rbegin() { return iterator(getLast()); }
    iterator rend() { return iterator(nullptr); }
};
#endif
