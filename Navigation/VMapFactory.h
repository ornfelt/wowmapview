#ifndef _VMAPFACTORY_H
#define _VMAPFACTORY_H

#include "IVMapMgr.h"

// This is the access point to the VMapMgr.
namespace VMAP
{
    class VMapMgr2;

    class VMapFactory
    {
    public:
        static VMapMgr2* createOrGetVMapMgr();
        static void clear();
    };
}
#endif
