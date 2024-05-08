#include <iostream>
#include "Map.h"
#include "MapMgr.h"
#include "Unit.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdio.h>

#include "Navigation.h"

//void MotionMaster::MoveBackwards(Unit* target, float dist)
//{
//    if (!target)
//    {
//        return;
//    }
//
//    Position const& pos = target->GetPosition();
//    float angle = target->GetAngle(_owner);
//    G3D::Vector3 point;
//    point.x = pos.m_positionX + dist * cosf(angle);
//    point.y = pos.m_positionY + dist * sinf(angle);
//    point.z = pos.m_positionZ;
//
//    if (!_owner->GetMap()->CanReachPositionAndGetValidCoords(_owner, point.x, point.y, point.z, true, true))
//    {
//        return;
//    }
//
//    Movement::MoveSplineInit init(_owner);
//    init.MoveTo(point.x, point.y, point.z, false);
//    init.SetFacing(target);
//    init.SetOrientationInversed();
//    init.Launch();
//}
//

void InitializeMapsForContinent(MMAP::MMapMgr* manager, unsigned int mapId)
{
	for (auto& p : std::filesystem::directory_iterator(std::string("C:\\local\\acore\\mmaps\\")))
	{
		std::string path = p.path().string();
		std::string extension = path.substr(path.find_last_of(".") + 1);
		if (extension == "mmtile")
		{
#ifdef _WIN32
			std::string filename = path.substr(path.find_last_of("\\") + 1);
#else
			string filename = path.substr(path.find_last_of("/") + 1);
#endif

			int xTens = filename[3] - '0';
			int xOnes = filename[4] - '0';
			int yTens = filename[5] - '0';
			int yOnes = filename[6] - '0';

			int x = (xTens * 10) + xOnes;
			int y = (yTens * 10) + yOnes;

			std::string mapIdString;
			if (mapId < 10)
				mapIdString = "00" + std::to_string(mapId);
			else if (mapId < 100)
				mapIdString = "0" + std::to_string(mapId);
			else
				mapIdString = std::to_string(mapId);

			if (filename[0] == mapIdString[0] && filename[1] == mapIdString[1] && filename[2] == mapIdString[2])
				manager->loadMap(mapId, x, y);
		}
	}
}

static bool gridIsLoaded = false;

Map* GetMapInstance(int mapId)
{
    static Map* instance = new Map(mapId, 0, 0);
    //static Map* instance = new Map(mapId, mapId, mapId);
	if (instance->GetId() != mapId)
		instance = new Map(mapId, 0, 0);

    return instance;
}

void TestCalculatePath(Unit *_owner)
{
	std::cout << "\nTesting CalculatePath!\n";
	int mapId = 1;

	float startX = -614.7f;
	float startY = -4335.4f;
	float startZ = 40.4f;

	float destX = -590.2f;
	float destY = -4206.1f;
	float destZ = 38.7;

	// This can be done if we want to skip vmaps initialization (not needed for calculatepath)
	// Then the GetMapInstance etc. below can be removed as well...
	//MMAP::MMapMgr* mmmgr = MMAP::MMapFactory::createOrGetMMapMgr();
	//std::vector<uint32> mapIds = { 0, 1 };
	//mmmgr->InitializeThreadUnsafe(mapIds);
	//InitializeMapsForContinent(mmmgr, mapId);

	// --- This part can be skipped if we comment out the code in NormalizePath in PathGenerator.cpp...
	Map* map = GetMapInstance(mapId);
    map->SetId(mapId);
	if (map && !gridIsLoaded)
	{
        std::cout << ">> Loading All Grids For Map " << mapId << std::endl;
		map->LoadAllCells();
	}
    _owner->SetMap(map);
	// ---

	//PathGenerator path(_owner, 1, 1);
	//PathGenerator path(_owner);
	PathGenerator path(_owner, mapId, mapId);
	bool result = path.CalculatePath(startX, startY, startZ, destX, destY, destZ, false);

	// Results
	Movement::PointsArray myPath = path.GetPath();
	size_t pathSize = myPath.size();
	if (pathSize > 2)
		std::cout << "Success!" << std::endl;

	std::cout << "PATH len: " << pathSize << std::endl;
	for (int i = 0; i < pathSize; ++i) {
		std::cout << "Point " << i + 1 << ": "
			<< "X=" << myPath[i].x << ", "
			<< "Y=" << myPath[i].y << ", "
			<< "Z=" << myPath[i].z << std::endl;
	}
}

void TestFailingCalculatePath()
{
	Unit *_owner = new Unit();

	std::cout << "\nTesting CalculatePath!\n";
	int mapId = 530;

	float startX = -138.89;
	float startY = 4776;
	float startZ = 29.32;

	float destX = 193.04;
	float destY = 4800.4;
	float destZ = 83.55;

	// This can be done if we want to skip vmaps initialization (not needed for calculatepath)
	// Then the GetMapInstance etc. below can be removed as well...
	//MMAP::MMapMgr* mmmgr = MMAP::MMapFactory::createOrGetMMapMgr();
	//std::vector<uint32> mapIds = { 0, 1 };
	//mmmgr->InitializeThreadUnsafe(mapIds);
	//InitializeMapsForContinent(mmmgr, mapId);

	// --- This part can be skipped if we comment out the code in NormalizePath in PathGenerator.cpp...
	Map* map = GetMapInstance(mapId);
    map->SetId(mapId);
	if (map && !gridIsLoaded)
	{
        std::cout << ">> Loading All Grids For Map " << mapId << std::endl;
		map->LoadAllCells();
	}
    _owner->SetMap(map);
	// ---

	//PathGenerator path(_owner, 1, 1);
	//PathGenerator path(_owner);
	PathGenerator path(_owner, mapId, mapId);
    //path.SetUseStraightPath(false);
	bool result = path.CalculatePath(startX, startY, startZ, destX, destY, destZ, false);

	// Results
	Movement::PointsArray myPath = path.GetPath();
	size_t pathSize = myPath.size();
	if (pathSize > 2)
		std::cout << "Success!" << std::endl;

	std::cout << "PATH len: " << pathSize << std::endl;
	for (int i = 0; i < pathSize; ++i) {
		std::cout << "Point " << i + 1 << ": "
			<< "X=" << myPath[i].x << ", "
			<< "Y=" << myPath[i].y << ", "
			<< "Z=" << myPath[i].z << std::endl;
	}
}

void TestMoveForwards(Unit *_owner, uint32 mapId, float startX, float startY, float startZ, float angle)
{
	std::cout << "start pos: " << startX << ", " << startY << ", " << startZ << std::endl;
	_owner->SetPositionX(startX);
	_owner->SetPositionY(startY);
	_owner->SetPositionZ(startZ);

	//Map* map = new Map(mapId, 0, 0);
	//Map* map = sMapMgr->CreateBaseMap(mapId);
	Map* map = GetMapInstance(mapId);
    map->SetId(mapId);
	if (map && !gridIsLoaded)
	{
        std::cout << ">> Loading All Grids For Map " << mapId << std::endl;
		map->LoadAllCells();
		// Loading all cells method won't reload if already loaded so we,
		// don't really need to check gridIsLoaded.
		// However, the boolean should be map specific...
		//gridIsLoaded = true;
	}
    _owner->SetMap(map);

	int dist = 10.0f; // Distance to move
    Position const& pos = _owner->GetPosition();
    //float angle = _owner->GetAngle(_owner);
    G3D::Vector3 point;
    point.x = pos.m_positionX + dist * cosf(angle);
    point.y = pos.m_positionY + dist * sinf(angle);
    point.z = pos.m_positionZ;
	std::cout << "target pos: " << point.x << ", " << point.y << ", " << point.z << std::endl;

    if (map && map->CanReachPositionAndGetValidCoords(_owner, point.x, point.y, point.z, true, true))
    {
		std::cout << "Can reach!\n";
		std::cout << "end: " << point.x << ", "
			<< point.y << ", " << _owner->GetPositionZNew() << std::endl;
        return;
    } else {
		std::cout << "Can NOT reach!\n";
	}
}

void RunTests()
{
	// TODO:::
	// Phasemask? CollisionHeight (unit height) moe forwards distance? LiquidData and other stuff???
	// Also, use Navigation.h instead of this?

	//WorldObject *_owner = new WorldObject();
	Unit *_owner = new Unit();

	//std::cout << "\nTesting MoveForwards! Should be valid...\n";
	std::cout << "\nTesting MoveForwards! Should be Valid...\n";
	TestMoveForwards(_owner, 1, -614.7f, -4335.4f, 40.4f, 0.0f); // Valid
	TestCalculatePath(_owner);

	std::cout << "\nTesting MoveForwards! Should be Invalid...\n";
	TestMoveForwards(_owner, 0, -10540.51685f, -1164.34599f, 28.0f, 0.0f); // Invalid
	std::cout << "\nTesting MoveForwards! Should be Valid...\n";
	TestMoveForwards(_owner, 0, -10280.243f, -1147.06f, 22.32f, 0.0f); // Valid
	std::cout << "\nTesting MoveForwards! Should be Invalid...\n";
	TestMoveForwards(_owner, 0, -10280.607f, -1144.988f, 21.86f, 0.0f); // Invalid
	std::cout << "\nTesting MoveForwards! Should be Invalid...\n";
	TestMoveForwards(_owner, 0, -10536.11906f, -1179.0126f, 28.26f, 2.98f); // Invalid
	std::cout << "\nTesting MoveForwards! Should be Valid...\n";
	TestMoveForwards(_owner, 0, -10536.11906f, -1179.0126f, 28.26f, -0.50f); // Valid
}

void checkAndPrintPath(XYZ* original, XYZ* result, float epsilon = 0.1f) {
    if (result != nullptr && (
        std::fabs(original->X - result->X) > epsilon ||
        std::fabs(original->Y - result->Y) > epsilon ||
        std::fabs(original->Z - result->Z) > epsilon)) {
        // If positions are different beyond the epsilon margin
        std::cout << "PATH: " << result->X << ", " << result->Y << ", " << result->Z << std::endl;
    } else {
        // If there is no significant movement or result is nullptr
        std::cout << "CAN'T MOVE FORWARD" << std::endl;
        std::cout << "Original Position: " << original->X << ", " << original->Y << ", " << original->Z << std::endl;
        if (result != nullptr) {
            std::cout << "Result Position: " << result->X << ", " << result->Y << ", " << result->Z << std::endl;
        } else {
            std::cout << "Result Position: null" << std::endl;
        }
    }
}

void RunTestsTwo()
{
    std::cout << "\nTesting MoveForwardWithParams! Should be Invalid...\n";
    XYZ position(-10386.0009765625, -1165.5711669921875, 42.643245697021484);
	
	//Navigation* nav = Navigation::GetInstance();
    //XYZ* forwardPath = nav->MoveForward(0, position.X, position.Y, position.Z, 0.0f, 0.0f, 3.0f, 5.0f, 0.5f);
	Navigation nav;
    XYZ* forwardPath = nav.MoveForward(0, position.X, position.Y, position.Z, 2.066, 0.0f, 3.0f, 5.0f, 1.0f);
	checkAndPrintPath(&position, forwardPath);
}

int main()
{
    std::cout << "Hello World!\n";
    //RunTests();
    //RunTestsTwo();
	TestFailingCalculatePath();
    std::cout << "\nBye World!\n";
}
