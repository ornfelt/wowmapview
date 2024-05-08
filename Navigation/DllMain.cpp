#include "Navigation.h"
#include <windows.h>

#ifdef _WIN32
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#endif

/**
 * @brief Singleton instance of the Navigation class.
 */
Navigation* Navigation::s_singletonInstance = NULL;

/**
 * @brief Gets the singleton instance of the Navigation class.
 *
 * @return The Navigation instance.
 */
Navigation* Navigation::GetInstance()
{
	if (s_singletonInstance == NULL)
		s_singletonInstance = new Navigation();
	return s_singletonInstance;
}

/**
 * @brief Initializes the navigation system.
 */
void Navigation::Initialize()
{
	dtAllocSetCustom(dtCustomAlloc, dtCustomFree);
}

/**
 * @brief Releases resources used by the navigation system.
 */
void Navigation::Release()
{
	//MMAP::MMapFactory::createOrGetMMapManager()->~MMapManager();
}

extern "C"
{
    __declspec(dllexport) XYZ* CalculatePathWithParams(unsigned int mapId, XYZ start, XYZ end, bool smoothPath, int* length, float hoverHeight, float objectSize, float collisionHeight)
    {
        return Navigation::GetInstance()->CalculatePath(mapId, start, end, smoothPath, length, hoverHeight, objectSize, collisionHeight);
    }

    __declspec(dllexport) XYZ* CalculatePath(unsigned int mapId, XYZ start, XYZ end, bool smoothPath, int* length)
    {
        return Navigation::GetInstance()->CalculatePath(mapId, start, end, smoothPath, length);
    }

    __declspec(dllexport) XYZ* MoveForwardWithParams(uint32 mapId, float startX, float startY, float startZ, float angle, float hoverHeight, float objectSize, float collisionHeight)
    {
        return Navigation::GetInstance()->MoveForward(mapId, startX, startY, startZ, angle, hoverHeight, objectSize, collisionHeight);
    }

    __declspec(dllexport) XYZ* MoveForward(uint32 mapId, float startX, float startY, float startZ, float angle)
    {
        return Navigation::GetInstance()->MoveForward(mapId, startX, startY, startZ, angle);
    }

    __declspec(dllexport) void FreePathArr(XYZ* pathArr)
    {
        return Navigation::GetInstance()->FreePathArr(pathArr);
    }
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    Navigation* navigation = Navigation::GetInstance();
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        navigation->Initialize();
        break;

    case DLL_PROCESS_DETACH:
        navigation->Release();
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}
