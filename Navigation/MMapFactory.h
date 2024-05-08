#ifndef _MMAP_FACTORY_H
#define _MMAP_FACTORY_H

#include "DetourAlloc.h"
#include "DetourExtended.h"
#include "DetourNavMesh.h"
#include "MMapMgr.h"
#include <unordered_map>

namespace MMAP
{
    enum MMAP_LOAD_RESULT
    {
        MMAP_LOAD_RESULT_ERROR,
        MMAP_LOAD_RESULT_OK,
        MMAP_LOAD_RESULT_IGNORED,
    };

    // static class
    // holds all mmap global data
    // access point to MMapMgr singleton
    class MMapFactory
    {
    public:
        static MMapMgr* createOrGetMMapMgr();
        static void clear();
        static void InitializeDisabledMaps();
        static bool forbiddenMaps[1000];
    };
}

#endif
