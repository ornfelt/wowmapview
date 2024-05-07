#ifndef MODEL_H
#define MODEL_H

#include "vec3d.h"

class Model;
class Bone;
Vec3D fixCoordSystem(Vec3D v);

#include "manager.h"
#include "mpq.h"
#include "video.h"

#include "modelheaders.h"
#include "quaternion.h"
#include "matrix.h"

#include <vector>

#include "animated.h"
#include "particle.h"


class Bone {
	Vec3D pivot;
	int parent;

	Animated<Vec3D> trans;
	Animated<Quaternion> rot;
	Animated<Vec3D> scale;

public:
	bool billboard;
	Matrix mat;
	Matrix mrot;

	bool calc;
	void calcMatrix(Bone* allbones, int anim, int time);
	void init(MPQFile &f, ModelBoneDef &b, int *global);
    void init(MPQFile &f, ModelBoneDefTBC &b, int *global);

};


class TextureAnim {
	Animated<Vec3D> trans, rot, scale;

public:
	Vec3D tval, rval, sval;

	void calc(int anim, int time);
	void init(MPQFile &f, ModelTexAnimDef &mta, int *global);
	void setup();
};

struct ModelColor {
	Animated<Vec3D> color;
	AnimatedShort opacity;

	void init(MPQFile &f, ModelColorDef &mcd, int *global);
};

struct ModelTransparency {
	AnimatedShort trans;

	void init(MPQFile &f, ModelTransDef &mtd, int *global);
};

// copied from the .mdl docs? this might be completely wrong
enum BlendModes {
	BM_OPAQUE,
	BM_TRANSPARENT,
	BM_ALPHA_BLEND,
	BM_ADDITIVE,
	BM_ADDITIVE_ALPHA,
	BM_MODULATE
};

struct ModelRenderPass {
	uint16 indexStart, indexCount, vertexStart, vertexEnd;
	TextureID texture, texture2;
	bool usetex2, useenvmap, cull, trans, unlit, nozwrite;
	float p;
	
	int16 texanim, color, opacity, blendmode;
	int16 order;

	bool init(Model *m);
	void deinit();

	bool operator< (const ModelRenderPass &m) const
	{
		//return !trans;
		if (order<m.order) return true;
		else if (order>m.order) return false;
		else return blendmode == m.blendmode ? (p<m.p) : blendmode < m.blendmode;
	}
};

struct ModelCamera {
	bool ok;

	Vec3D pos, target;
	float nearclip, farclip, fov;
	Animated<Vec3D> tPos, tTarget;
	Animated<float> rot;

	void init(MPQFile &f, ModelCameraDef &mcd, int *global);
	void setup(int time=0);

	ModelCamera():ok(false) {}
};

struct ModelLight {
	int type, parent;
	Vec3D pos, tpos, dir, tdir;
	Animated<Vec3D> diffColor, ambColor;
	Animated<float> diffIntensity, ambIntensity;

	void init(MPQFile &f, ModelLightDef &mld, int *global);
	void setup(int time, GLuint l);
};

class Model: public ManagedItem {

	GLuint dlist;
	GLuint vbuf, nbuf, tbuf;
	size_t vbufsize;
	bool animated;
	bool animGeometry,animTextures,animBones;

	bool forceAnim;

	void init(MPQFile &f);

	ModelHeader header;
	TextureAnim *texanims;
	ModelAnimation *anims;
	int *globalSequences;
	ModelColor *colors;
	ModelTransparency *transparency;
	ModelLight *lights;
	ParticleSystem *particleSystems;
	RibbonEmitter *ribbons;

	void drawModel();
	void initCommon(MPQFile &f);
	bool isAnimated(MPQFile &f);
	void initAnimated(MPQFile &f);
	void initStatic(MPQFile &f);

	ModelVertex *origVertices;
	Vec3D *vertices, *normals;
	uint16 *indices;
	size_t nIndices;
	std::vector<ModelRenderPass> passes;

	void animate(int anim);
	void calcBones(int anim, int time);

	void lightsOn(GLuint lbase);
	void lightsOff(GLuint lbase);

	bool containsCreaturePath(const std::string& str);
	bool containsSpellPath(const std::string& str);

public:
	ModelCamera cam;
	Bone *bones;
	TextureID *textures;

	bool ok;
	bool ind;

	float rad;
	float trans;
	bool animcalc;
	int anim, animtime;

	Model(std::string name, bool forceAnim=false);
	~Model();
	void draw();
	void updateEmitters(float dt);

	friend struct ModelRenderPass;

	std::string modelPath;
	bool isUnit;
	bool isSpell;
	bool isNpc = false;

};

class ModelManager: public SimpleManager {
public:
	int add(std::string name);

	ModelManager() : v(0) {}

	int v;

	void resetAnim();
	void updateEmitters(float dt);

};


class ModelInstance {
public:
	Model *model;

	int id;

	Vec3D pos, dir;
	unsigned int d1, scale;

	float frot,w,sc;

	int light;
	Vec3D ldir;
	Vec3D lcol;

	ModelInstance() {}
	ModelInstance(Model *m, MPQFile &f);
    void init2(Model *m, MPQFile &f);
	void draw();
	void draw2(const Vec3D& ofs, const float rot);


	struct Point {
		double x, y, z;
	};

	//static const std::array<Point, 24> path;
    size_t currentTargetIndex = 0;
    //Point pos;
    double moveSpeed = 0.1;

	const std::vector<Point> currentPath = {
		{ 717.929, -475.351, 75.6262},
		{ 715.583, -472.112, 73.8247},
		{ 713.236, -468.872, 72.0745},
		{ 710.89, -465.633, 70.0785},
		{ 708.544, -462.393, 67.98},
		{ 706.197, -459.154, 66.2349},
		{ 703.851, -455.914, 64.8988},
		{ 701.504, -452.674, 63.9584},
		{ 699.158, -449.435, 63.3191},
		{ 696.812, -446.195, 63.1477},
		{ 694.465, -442.956, 63.3726},
		{ 692.119, -439.716, 63.7494},
		{ 689.772, -436.477, 64.3756},
		{ 687.426, -433.237, 65.7292},
		{ 685.08, -429.998, 66.6914},
		{ 682.733, -426.758, 66.6629},
		{ 679.431, -424.501, 66.9688},
		{ 676.128, -422.244, 67.4488},
		{ 672.826, -419.987, 68.3},
		{ 669.524, -417.73, 68.0332},
		{ 666.221, -415.473, 68.1515},
		{ 662.919, -413.216, 68.5465},
		{ 659.617, -410.959, 68.5527},
		{ 656.314, -408.702, 68.7116},
		{ 653.012, -406.445, 68.6472},
		{ 649.709, -404.188, 68.5478},
		{ 646.407, -401.931, 68.4585},
		{ 643.105, -399.674, 68.5721},
		{ 639.802, -397.417, 68.3847},
		{ 636.5, -395.16, 68.2479},
		{ 633.198, -392.903, 68.2916},
		{ 629.895, -390.646, 68.2826},
		{ 625.899, -390.812, 68.4743},
		{ 621.902, -390.977, 68.3371},
		{ 617.905, -391.143, 68.2315},
		{ 613.909, -391.309, 67.9549},
		{ 610.016, -392.23, 67.6257},
		{ 606.124, -393.152, 67.0884},
		{ 602.232, -394.073, 66.054},
		{ 598.339, -394.995, 66.3042},
		{ 594.447, -395.916, 66.448},
		{ 590.554, -396.838, 66.2725},
		{ 586.662, -397.759, 66.1199},
		{ 582.77, -398.68, 65.7377},
		{ 578.877, -399.602, 65.0491},
		{ 574.985, -400.523, 64.3588},
		{ 571.092, -401.445, 63.8789},
		{ 567.2, -402.366, 63.0437},
		{ 563.308, -403.288, 61.5816},
		{ 559.415, -404.209, 60.0067},
		{ 555.523, -405.131, 58.7376},
		{ 551.63, -406.052, 57.4211},
		{ 547.871, -407.418, 56.3555},
		{ 544.111, -408.783, 55.1918},
		{ 540.352, -410.149, 54.0622},
		{ 536.592, -411.515, 52.4365},
		{ 532.832, -412.881, 50.2002},
		{ 529.073, -414.246, 48.4309},
		{ 525.313, -415.612, 46.4263},
		{ 521.553, -416.978, 44.9593},
		{ 517.794, -418.343, 43.8814},
		{ 514.034, -419.709, 42.7535},
		{ 510.274, -421.075, 41.6899},
		{ 506.515, -422.44, 40.777},
		{ 502.755, -423.806, 39.8449},
		{ 498.995, -425.172, 38.9802},
		{ 495.236, -426.538, 38.3078},
		{ 491.476, -427.903, 37.843},
		{ 487.717, -429.269, 37.3088},
		{ 483.957, -430.635, 36.8557},
		{ 480.197, -432, 36.1987},
		{ 476.438, -433.366, 35.6295},
		{ 472.678, -434.732, 34.9238},
		{ 468.918, -436.098, 34.2285},
		{ 465.159, -437.463, 33.7328},
		{ 463.128, -438.201, 33.5102}
	};

};

#endif
