#ifndef _GRIDREFERENCE_H
#define _GRIDREFERENCE_H

#include "Reference.h"

template<class OBJECT>
class GridRefMgr;

template<class OBJECT>
class GridReference : public Reference<GridRefMgr<OBJECT>, OBJECT>
{
protected:
    void targetObjectBuildLink() override
    {
        // called from link()
        this->getTarget()->insertFirst(this);
        this->getTarget()->incSize();
    }
    void targetObjectDestroyLink() override
    {
        // called from unlink()
        if (this->isValid()) this->getTarget()->decSize();
    }
    void sourceObjectDestroyLink() override
    {
        // called from invalidate()
        this->getTarget()->decSize();
    }
public:
    GridReference() : Reference<GridRefMgr<OBJECT>, OBJECT>() {}
    ~GridReference() override { this->unlink(); }
    GridReference* next() { return (GridReference*)Reference<GridRefMgr<OBJECT>, OBJECT>::next(); }
};
#endif
