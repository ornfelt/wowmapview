#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <iostream>
#include "Map.h"
#include "MapMgr.h"
#include "Unit.h"

#include <vector>
#include <fstream>
#include <filesystem>
#include <stdio.h>

class XYZ
{
public:
    float X;
    float Y;
    float Z;

    XYZ() : X(0), Y(0), Z(0) {}

    XYZ(double X, double Y, double Z) : X(static_cast<float>(X)), Y(static_cast<float>(Y)), Z(static_cast<float>(Z)) {}
};

class Navigation {
public:
	Navigation() {
	}

	~Navigation() {
	}

	static Navigation* GetInstance();
	void Initialize();
	void Release();

    //Navigation(const Unit* owner, unsigned int startMap, unsigned int endMap);
	//XYZ* CalculatePath(unsigned int mapId, XYZ start, XYZ end, bool smoothPath, int* length);
	XYZ* CalculatePath(unsigned int mapId, XYZ start, XYZ end, bool smoothPath, int* length, float hoverHeight=0.5f, float objectSize=3.0f, float collisionHeight=5.0f);
	void FreePathArr(XYZ* pathArr);
	//XYZ* MoveForward(uint32 mapId, float startX, float startY, float startZ, float angle);
	XYZ* MoveForward(uint32 mapId, float startX, float startY, float startZ, float angle, float hoverHeight=0.5f, float objectSize=1.0f, float collisionHeight=5.0f, float dist=1.0f);

private:
	XYZ* currentPath; ///< Current calculated path.

	void InitializeMapsForContinent(MMAP::MMapMgr* manager, unsigned int mapId);
	Map* GetMapInstance(int mapId);
	Unit* GetUnitInstance();
};

#endif
