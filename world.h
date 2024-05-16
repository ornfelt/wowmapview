#ifndef WORLD_H
#define WORLD_H

#define USE_OLD_CHAR 0

#include "wowmapview.h"
#include "maptile.h"
#include "wmo.h"
#include "frustum.h"
#include "sky.h"

#include <string>
#include <unordered_map>

#define MAPTILECACHESIZE 16

const float detail_size = 8.0f;

const std::unordered_map<std::string, int> mapNameToId = {
    {"Azeroth", 0},
    {"Kalimdor", 1},
    {"PVPZone01", 30},
    {"shadowfang", 33},
    {"deadminesinstance", 36},
    {"PVPZone02", 37},
    {"razorfenkraulinstance", 47},
    {"emeralddream", 169},
    {"tanarisinstance", 209},
    {"schoolofnecromancy", 289},
    {"zul'gurub", 309},
    {"blackwinglair", 469},
    {"pvpzone03", 489},
    {"ahnqiraj", 509},
    {"pvpzone04", 529},
    {"expansion01", 530},
    {"ahnqirajtemple", 531},
    {"hellfirerampart", 543},
    {"pvpzone05", 559},
    {"netherstormbg", 566},
    {"Northrend", 571},
    {"azjol_uppercity", 601},
    {"gundrak", 604},
    {"northrendbg", 607},
    {"dalaranprison", 608},
    {"DeathKnightStart", 609},
    {"orgrimmararena", 618},
    {"nexusraid", 616},
    {"dalaranarena", 617},
    {"wintergraspraid", 624},
    {"icecrowncitadel5man", 632},
    {"hillsbradpast", 560},
    {"bladesedgearena", 562},
    {"blacktemple", 564},
    {"zulaman", 568},
    {"utgardepinnacle", 575},
    {"nexus80", 578},
    {"sunwellplateau", 580},
    {"sunwell5manfix", 585},
    {"stratholmecot", 595},
    {"draktheronkeep", 600},
    {"ulduar80", 602},
    {"ulduarraid", 603},
    {"chamberofaspectsblack", 615},
    {"azjol_lowercity", 619},
    {"isleofconquest", 628},
    {"icecrowncitadel", 631},
    {"argenttournamentraid", 649},
    {"argenttournamentdungeon", 650},
    {"quarryoftears", 658},
    {"ulduar70", 599},
    {"pvplordaeron", 572},
    {"valgarde70", 574},
    {"stormwind", 723},
    {"chamberofaspectsred", 724}
};

class World {

	MapTile *maptilecache[MAPTILECACHESIZE];
	MapTile *current[3][3];
	int ex,ez;

public:

	std::string basename;
	int mapId;

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
	void createPlayerTwo(MPQFile& f, std::string modelPath);
	Model* player;
	Model* playertwo;
	std::vector<Model*> playermodels;
	std::vector<ModelInstance> playermodelis;
	std::vector<ModelInstance> spellmodelis;
	bool teleToTarget;
#endif

	//void CalculatePath(float startX, float startY, float startZ, float destX, float destY, float destZ);
	std::vector<Vec3D> CalculatePath(float startX, float startY, float startZ, float destX, float destY, float destZ);
	void InitNavigation();
	Vec3D GetClosestNode(double posX, double posY, double posZ, ModelInstance& modelInstance);
	Vec3D GetRandomNode(ModelInstance& modelInstance);
	Vec3D GetRandomLinkedNode(uint32_t nodeId, ModelInstance& modelInstance);
};

extern World *gWorld;


void lightingDefaults();
void myFakeLighting();



#endif
