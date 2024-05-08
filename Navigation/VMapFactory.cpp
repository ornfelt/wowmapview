#include "VMapFactory.h"
#include "VMapMgr2.h"

namespace VMAP
{
    VMapMgr2* gVMapMgr = nullptr;

    //===============================================
    // just return the instance
    VMapMgr2* VMapFactory::createOrGetVMapMgr()
    {
        if (!gVMapMgr)
        {
            gVMapMgr = new VMapMgr2();
        }

        return gVMapMgr;
    }

    //===============================================
    // delete all internal data structures
    void VMapFactory::clear()
    {
        delete gVMapMgr;
        gVMapMgr = nullptr;
    }
}
