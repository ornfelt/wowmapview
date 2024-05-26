#include "Navigation.h"
#include <filesystem>

Map* Navigation::GetMapInstance(int mapId) {
    static Map* instance = new Map(mapId, 0, 0);
    if (instance->GetId() != mapId)
        instance = new Map(mapId, 0, 0);
    return instance;
}

Unit* Navigation::GetUnitInstance() {
    static Unit* instance = new Unit();
    return instance;
}

static Navigation* s_singletonInstance; ///< Singleton instance of the Navigation class.
Navigation* Navigation::GetInstance() {
	if (s_singletonInstance == nullptr) {
		s_singletonInstance = new Navigation();  // Create a new instance if one does not exist
	}
	return s_singletonInstance;  // Return the existing or new instance
}

void Navigation::InitializeMapsForContinent(MMAP::MMapMgr* manager, unsigned int mapId) {
    //for (auto& p : std::filesystem::directory_iterator("C:\\local\\acore\\mmaps\\")) {
    for (auto& p : std::filesystem::directory_iterator("/home/jonas/acore/bin/mmaps/")) {
        std::string path = p.path().string();
        std::string extension = path.substr(path.find_last_of(".") + 1);
        if (extension == "mmtile") {
            std::string filename = std::filesystem::path(path).filename().string();
            int x = std::stoi(filename.substr(3, 2));
            int y = std::stoi(filename.substr(5, 2));
            std::string mapIdString = std::to_string(mapId);
            while (mapIdString.length() < 3) mapIdString = '0' + mapIdString;
            if (filename.substr(0, 3) == mapIdString)
                manager->loadMap(mapId, x, y);
        }
    }
}

XYZ* Navigation::CalculatePath(unsigned int mapId, XYZ start, XYZ end, bool smoothPath, int *length, float hoverHeight, float objectSize, float collisionHeight) {
    //const Unit* _owner = GetUnitInstance();
    //std::cout << "START CALC PATH" << std::endl;
    //std::cout << start.X << std::endl;
    //std::cout << start.Y << std::endl;
    //std::cout << start.Z << std::endl;
    Unit* _owner = GetUnitInstance();
    //if (_owner->GetCollisionHeight() <= 0.0f) {
        // Set values
        _owner->SetHoverHeight(hoverHeight);
        _owner->SetObjectSize(objectSize);
        _owner->SetCollisionHeight(collisionHeight);
        _owner->SetMinHeightInWater(collisionHeight); // Configure as well??
    //}

    MMAP::MMapMgr* mmmgr = MMAP::MMapFactory::createOrGetMMapMgr();
    //mmmgr->InitializeThreadUnsafe({ 0, 1, 30, 489, 529, 530, 559, 571, 723 });
    mmmgr->InitializeThreadUnsafe({0, 1, 30, 33, 36, 37, 47, 169, 209, 289, 309, 469, 489, 509, 529, 530, 531, 543, 559, 560, 562, 564, 566, 568, 571, 572, 574, 575, 578, 580, 585, 595, 599, 600, 601, 602, 603, 604, 607, 608, 615, 616, 617, 618, 619, 624, 628, 631, 632, 649, 650, 658, 723, 724});
    InitializeMapsForContinent(mmmgr, mapId);

	Map* map = GetMapInstance(mapId);
    map->SetId(mapId);
	if (map)
	{
        std::cout << ">> Loading All Grids For Map " << mapId << std::endl;
		map->LoadAllCells();
	}
    _owner->SetMap(map);

    PathGenerator path(_owner, mapId, mapId);
    //path.SetUseStraightPath(smoothPath);
    path.CalculatePath(start.X, start.Y, start.Z, end.X, end.Y, end.Z, smoothPath);

    // Results
    Movement::PointsArray myPath = path.GetPath();
    size_t pathSize = myPath.size();
    *length = static_cast<int>(pathSize);

	XYZ* pathArr = new XYZ[pathSize];

	for (unsigned int i = 0; i < pathSize; i++)
	{
		pathArr[i].X = myPath[i].x;
		pathArr[i].Y = myPath[i].y;
		pathArr[i].Z = myPath[i].z;
        //std::cout << "pathArr[i].X: " << pathArr[i].X << std::endl;
        //std::cout << "pathArr[i].Y: " << pathArr[i].Y << std::endl;
        //std::cout << "pathArr[i].Z: " << pathArr[i].Z << std::endl;
	}

	return pathArr;
}

//XYZ* Navigation::CalculatePath(unsigned int mapId, XYZ start, XYZ end, bool smoothPath, int* length)
//{
//    Movement::PointsArray myPath;
//    CalculatePathInternal(mapId, start, end, smoothPath, myPath);
//    *length = static_cast<int>(myPath.size());
//    XYZ* pathArr = new XYZ[*length];
//    for (int i = 0; i < *length; ++i) {
//        pathArr[i].X = myPath[i].x;
//        pathArr[i].Y = myPath[i].y;
//        pathArr[i].Z = myPath[i].z;
//    }
//    return pathArr;
//}

void Navigation::FreePathArr(XYZ* pathArr) {
    delete[] pathArr;
}

XYZ* Navigation::MoveForward(uint32 mapId, float startX, float startY, float startZ, float angle, float hoverHeight, float objectSize, float collisionHeight, float dist)
{
    Unit* _owner = GetUnitInstance();
    //if (_owner->GetCollisionHeight() <= 0.0f) {
        // Set values
        _owner->SetHoverHeight(hoverHeight);
        _owner->SetObjectSize(objectSize);
        _owner->SetCollisionHeight(collisionHeight);
        _owner->SetMinHeightInWater(collisionHeight); // Configure as well??
    //}

	_owner->SetPositionX(startX);
	_owner->SetPositionY(startY);
	_owner->SetPositionZ(startZ);
    Map* map = GetMapInstance(mapId);
    //if (map) {
	map->SetId(mapId);
	_owner->SetMap(map);
	map->LoadAllCells();
	//float dist = 1.0f; // Distance to move
	G3D::Vector3 point;
	point.x = startX + dist * cosf(angle);
	point.y = startY + dist * sinf(angle);
	point.z = startZ;
	//std::cout << "start pos: " << startX << ", " << startY << ", " << startZ << std::endl;
	//std::cout << "target pos: " << point.x << ", " << point.y << ", " << point.z << std::endl;
	//G3D::Vector3 point(startX + 10.0f * cosf(angle), startY + 10.0f * sinf(angle), startZ);

	XYZ* newPosition = new XYZ();
	newPosition->X = startX;
	newPosition->Y = startY;
	newPosition->Z = startZ;
	//if (map->CanReachPositionAndGetValidCoords(_owner, point.x, point.y, point.z, true, true))
	if (map->CanReachPositionAndGetValidCoords(_owner, point.x, point.y, point.z, true, false))
	{
		//return true;
		newPosition->X = point.x;
		newPosition->Y = point.y;
		newPosition->Z = _owner->GetPositionZNew();
		//std::cout << "end pos: " << newPosition->X << ", " << newPosition->Y << ", " << newPosition->Z << std::endl;
		return newPosition;
	}
    //}
    //return false;
    //return nullptr;
    return newPosition;
}

