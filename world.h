#ifndef WORLD_H
#define WORLD_H

#define USE_OLD_CHAR 0

#include "wowmapview.h"
#include "maptile.h"
#include "wmo.h"
#include "frustum.h"
#include "sky.h"

#include <string>

#define MAPTILECACHESIZE 16

const float detail_size = 8.0f;

class World {

	MapTile *maptilecache[MAPTILECACHESIZE];
	MapTile *current[3][3];
	int ex,ez;

public:

	std::string basename;

	bool maps[64][64];
	GLuint lowrestiles[64][64];
	bool autoheight;

	std::vector<std::string> gwmos;
	std::vector<WMOInstance> gwmois;
	int gnWMO, nMaps;

	float mapdrawdistance, modeldrawdistance, doodaddrawdistance, highresdistance;
	float mapdrawdistance2, modeldrawdistance2, doodaddrawdistance2, highresdistance2;

	float culldistance, culldistance2, fogdistance;

	float l_const, l_linear, l_quadratic;

	Skies *skies;
	float time,animtime;

    bool hadSky;

	bool thirdperson,lighting,drawmodels,drawdoodads,drawterrain,drawwmo,loading,drawhighres,drawfog;
	bool uselowlod;

	GLuint detailtexcoords, alphatexcoords;

	short *mapstrip,*mapstrip2;

	TextureID water;
	Vec3D camera, lookat;
	Frustum frustum;
	int cx,cz;
	bool oob;

	WMOManager wmomanager;
	ModelManager modelmanager;

	OutdoorLighting *ol;
	OutdoorLightStats outdoorLightStats;

	GLuint minimap;

	World(const char* name);
	~World();
	void init();
	void initMinimap();
	void initDisplay();
	void initWMOs();
	void initLowresTerrain();

	void enterTile(int x, int z);
	MapTile *loadTile(int x, int z);
	void tick(float dt);
	void draw();

	void outdoorLighting();
	void outdoorLights(bool on);
	void setupFog();

	/// Get the tile on wich the camera currently is on
	unsigned int getAreaID();

	//std::string playerModelPath = "spells\\PyroBlast_Missile.mdx";
	//std::string playerModelPath = "spells\\Frostbolt.mdx";
	//std::string playerModelPath = "character\\human\\male\\humanmale.mdx";
	//std::string playerModelPath = "character\\scourge\\male\\scourgemale.mdx";

	//std::string playerModelPath = "creature\\dragon\\dragononyxia.mdx";
	std::string playerModelPath = "creature\\drake\\drake.mdx";
	//std::string playerModelPath = "creature\\Cow\\cow.mdx";
	//std::string playerModelPath = "creature\\druidbear\\druidbear.mdx";
	//std::string playerModelPath = "creature\\diablo\\DiabloFunSized.mdx";
	//std::string playerModelPath = "creature\\voidwalker\\voidwalker.mdx";
	//std::string playerModelPath = "creature\\panda\\pandacub.mdx";
	//std::string playerModelPath = "creature\\rabbit\\rabbit.mdx";
	//std::string playerModelPath = "creature\\SkeletonNaked\\SkeletonNaked.mdx";
	//std::string playerModelPath = "creature\\ragnaros\\ragnaros.mdx";

#if !USE_OLD_CHAR
	void createPlayer(MPQFile& f);
	void createPlayerTwo(MPQFile& f);
	Model* player;
	Model* playertwo;
	std::vector<ModelInstance> playermodelis;
#endif
};

extern World *gWorld;


void lightingDefaults();
void myFakeLighting();



#endif
