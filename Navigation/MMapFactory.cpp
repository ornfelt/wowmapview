#include "MMapFactory.h"
#include <cstring>
#include <set>

namespace MMAP
{
    // ######################## MMapFactory ########################
    // our global singleton copy
    MMapMgr* g_MMapMgr = nullptr;
    bool MMapFactory::forbiddenMaps[1000] = {0};

    MMapMgr* MMapFactory::createOrGetMMapMgr()
    {
        if (!g_MMapMgr)
        {
            g_MMapMgr = new MMapMgr();
        }

        return g_MMapMgr;
    }

    void MMapFactory::InitializeDisabledMaps()
    {
        memset(&forbiddenMaps, 0, sizeof(forbiddenMaps));
        int32 f[] = {616 /*EoE*/, 649 /*ToC25*/, 650 /*ToC5*/, -1};
        uint32 i = 0;
        while (f[i] >= 0)
        {
            forbiddenMaps[f[i++]] = true;
        }
    }

    void MMapFactory::clear()
    {
        if (g_MMapMgr)
        {
            delete g_MMapMgr;
            g_MMapMgr = nullptr;
        }
    }
}
