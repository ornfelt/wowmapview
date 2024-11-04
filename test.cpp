#include "test.h"
#include "wowmapview.h"
#include "areadb.h"
//#include "shaders.h"

#include <cmath>
#include <string>

using namespace std;


//#define XSENS 15.0f
//#define YSENS 15.0f
#define XSENS 4.0f
#define YSENS 8.0f
#define SPEED 66.6f

// why the hell is this class called Test, anyway
// I should rename it to MapViewer or something when I'm not lazy

Test::Test(World *w, float ah0, float av0): world(w), ah(ah0), av(av0)
{
	moving = strafing = updown = 0;

	//mousedir = -1.0f;
	mousedir = 0;

	movespd = SPEED;

	look = false;
	mapmode = false;
	hud = true;

	world->thirdperson = false;
	world->lighting = true;
	world->drawmodels = true;
	world->drawdoodads = true;
	world->drawterrain = true;
	world->drawwmo = true;
	world->drawhighres = true;
	world->drawfog = true; // should this be on or off by default..? :(

	world->drawPolygons = false;
	world->drawPlayerModels = true;

	// in the wow client, fog distance is stored in wtf\config.wtf as "farclip"
	// minimum is 357, maximum is 777
	world->fogdistance = 512.0f;

	world->l_const = 0.0f;
	world->l_linear = 0.7f;
	world->l_quadratic = 0.03f;
}

Test::~Test()
{
	delete world;
}

void Test::tick(float t, float dt)
{
	Vec3D dir(1,0,0);
	rotate(0,0, &dir.x,&dir.y, av*PI/180.0f);
    rotate(0,0, &dir.x,&dir.z, ah*PI/180.0f);

	if (world->playermodelis[0].usePhysics && moving > 0) {
		// Position
		//float myAngle = atan2(world->playermodelis[0].dir.y, world->playermodelis[0].dir.x) * 180.0f / PI;
		float myAngle = (world->playermodelis[0].dir.y * (PI / 180.0f)) + PI / 2 + PI;
		//std::cout << "Calculated angle: " << myAngle << " degrees" << std::endl;

		Vec3D newPos = world->MoveForward(-(world->playermodelis[0].pos.z - ZEROPOINT), -(world->playermodelis[0].pos.x - ZEROPOINT), world->playermodelis[0].pos.y, myAngle, 0.5f);
		float newPosX = -newPos.y + ZEROPOINT;
		float newPosY = newPos.z;
		float newPosZ = -newPos.x + ZEROPOINT;

		Vec3D targetPos(newPosX, newPosY, newPosZ);
		Vec3D newdir = targetPos - world->playermodelis[0].pos;
		double distance = sqrt(newdir.x * newdir.x + newdir.y * newdir.y + newdir.z * newdir.z);
		//if (distance < 0.001) {
		//	std::cout << "Can't move! Distance: " << distance << std::endl;
		//}
		//else {
			float moveSpeed = 1.0;
			// Move towards
			//world->playermodelis[0].pos.x += newdir.x * moveSpeed;
			//world->playermodelis[0].pos.y += newdir.y * moveSpeed;
			//world->playermodelis[0].pos.z += newdir.z * moveSpeed;
			world->playermodelis[0].pos.x = newPosX;
			world->playermodelis[0].pos.y = newPosY;
			world->playermodelis[0].pos.z = newPosZ;

			// Camera
			//world->camera = world->playermodelis[0].pos;
			////Vec3D newcampos = gWorld->camera - cameranewdir * distanceBehindCamera;
			//newdir.y = 0.0f;
			//Vec3D newcampos = gWorld->camera - (newdir * distanceBehindCamera);
			//newcampos.y += distanceBehindCamera/2;
			//world->lookat = world->playermodelis[0].pos;
			//world->camera = newcampos;

			world->camera = world->playermodelis[0].pos;
			float rotationAngle = -(world->playermodelis[0].dir.y * (PI / 180.0f)) + PI;
			Vec3D newmovedir(cos(rotationAngle + PI), 0, sin(rotationAngle + PI));
			newmovedir.y = 0.0f;
			Vec3D newcampos = gWorld->camera - (newmovedir * distanceBehindCamera);
			newcampos.y += distanceBehindCamera/2;
			world->lookat = world->playermodelis[0].pos;
			world->camera = newcampos;

			//world->lookat = world->camera + newmovedir;
			//world->camera = world->playermodelis[0].pos - (newmovedir * 20.0f);
			//world->camera = world->playermodelis[0].pos;
			//world->camera.y += 20.0f;
		//}
	}
	else if (!world->playermodelis[0].usePhysics && !world->playermodelis[0].isWandering) {
		if (moving != 0) world->camera += dir * dt * movespd * moving;
		if (strafing != 0) {
			Vec3D right = dir % Vec3D(0, 1, 0);
			right.normalize();
			world->camera += right * dt * movespd * strafing;
		}
		if (updown != 0) world->camera += Vec3D(0, dt * movespd * updown, 0);
		world->lookat = world->camera + dir;
	}
	if (world->playermodelis[0].usePhysics) {
		if (strafing != 0) {
			// Kinda cool effect
			////Vec3D right = dir % Vec3D(0, 1, 0);
			//Vec3D right = dir % Vec3D(1, 1, 0);
			//right.normalize();
			////world->playermodelis[0].dir += right * dt * movespd * strafing;
			//world->camera += right * dt * movespd * strafing;

			world->playermodelis[0].dir.y -= strafing;

			world->camera = world->playermodelis[0].pos;
			float rotationAngle = -(world->playermodelis[0].dir.y * (PI / 180.0f)) + PI;
			Vec3D newdir(cos(rotationAngle + PI), 0, sin(rotationAngle + PI));
			newdir.y = 0.0f;
			Vec3D newcampos = gWorld->camera - (newdir * distanceBehindCamera);
			newcampos.y += distanceBehindCamera/2;
			world->lookat = world->playermodelis[0].pos;
			world->camera = newcampos;

			//Vec3D newcampos = gWorld->camera - (world->playermodelis[0].dir * 40.0f);
			//newcampos.y += 20.0f;
			//world->lookat = world->playermodelis[0].pos;
			//world->camera = newcampos;
		}
		//if (updown != 0) world->camera += Vec3D(0, dt * movespd * updown, 0);
		if (strafing == 0 && mousedir) {
			world->lookat = world->camera + dir;
		}
	}

	world->time += (world->modelmanager.v * /*360.0f*/ 90.0f * dt);
	world->animtime += dt * 1000.0f;
	globalTime = (int)world->animtime;

	world->tick(dt);
	mousedir = 0;
}

void Test::display(float t, float dt)
{
	if (mapmode && world->minimap) {
		// show map
        // TODO: try to use a real map from WoW? either the large map or the minimap would be nice
		video.clearScreen();
		video.set2D();

		const int len = 768;
		const int basex = 200;
		const int basey = 0;
		glColor4f(1,1,1,1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, world->minimap);
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2i(basex,basey);
		glTexCoord2f(1,0);
		glVertex2i(basex+len,basey);
		glTexCoord2f(1,1);
		glVertex2i(basex+len,basey+len);
		glTexCoord2f(0,1);
		glVertex2i(basex,basey+len);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glBegin(GL_LINES);
		float fx, fz;
		fx = basex + world->camera.x / TILESIZE * 12.0f;
		fz = basey + world->camera.z / TILESIZE * 12.0f;
		glVertex2f(fx, fz);
		glColor4f(1,1,1,0);
		glVertex2f(fx + 10.0f*cosf(ah/180.0f*PI), fz + 10.0f*sinf(ah/180.0f*PI));
		glEnd();
	} else {
        // draw 3D view
		video.set3D();
		world->draw();
		
		video.set2D();
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glColor4f(1,1,1,1);

		if (hud) {
			f16->print(5,0,"%.2f fps", gFPS);

			//char *sn = world->skies->getSkyName();
			//if (sn)	f16->print(5,60,"%s", sn);

			// TODO: look up WMO names/group names as well from some client db?
			unsigned int areaID = world->getAreaID();
			unsigned int regionID = 0;
			/// Look up area
			try {
				AreaDB::Record rec = gAreaDB.getByAreaID(areaID);
				std::string areaName = rec.getString(AreaDB::Name);
				regionID = rec.getUInt(AreaDB::Region);
				f16->print(5,20,"%s", areaName.c_str());
			} catch(AreaDB::NotFound)
			{
				/// Not found, unknown area
				//f16->print(5,20,"Unknown [%i]", areaID);
			}
			if (regionID != 0) {
				/// Look up region
				try {
					AreaDB::Record rec = gAreaDB.getByAreaID(regionID);
					std::string regionName = rec.getString(AreaDB::Name);
					f16->print(5,40,"%s", regionName.c_str());
				} catch(AreaDB::NotFound)
				{
					//f16->print(5,40,"Unknown [%i]", regionID);
				}
			}

			//f16->print(5,60,"%d", world->modelmanager.v);

			int time = ((int)world->time)%2880;
			int hh,mm;

            hh = time / 120;
			mm = (time % 120) / 2;
			
			//f16->print(5, 60, "%02d:%02d", hh,mm);
			f16->print(video.xres - 50, 0, "%02d:%02d", hh,mm);

			//f16->print(5, video.yres-22, "(%.0f, %.0f, %.0f)", 
			//	-(world->camera.x - ZEROPOINT), 
			//	-(world->camera.z - ZEROPOINT),
			//	world->camera.y);
			f16->print(5, video.yres-22, "(%.0f, %.0f, %.0f)", 
				-(world->camera.z - ZEROPOINT),
				-(world->camera.x - ZEROPOINT), 
				world->camera.y);

		}

		if (world->loading) {
			const char* loadstr = "Loading...";
			const char* oobstr = "Out of bounds";

			f16->print(video.xres/2 - f16->textwidth(loadstr)/2, /*video.yres/2-8*/ 0, world->oob?oobstr:loadstr);
		}

		/*
		f16->print(5,20,"C: %.1f", world->l_const);
		f16->print(5,40,"L: %.2f", world->l_linear);
		f16->print(5,60,"Q: %.3f", world->l_quadratic);
		*/
	}

};

void Test::keypressed(SDL_KeyboardEvent *e)
{
	if (e->type == SDL_KEYDOWN) {
		// key DOWN

		// quit
		if (e->keysym.sym == SDLK_ESCAPE) {
		    gPop = true;
		}
		// movement
		if (e->keysym.sym == SDLK_w) {
			moving = 1.0f;
			world->playermodelis[0].isWandering = false;
			world->playermodelis[0].currentPath = std::vector<Vec3D>();
			world->playermodelis[0].currentNode.x = 0.0f;
		}
		if (e->keysym.sym == SDLK_s) {
			moving = -1.0f;
		}
		if (e->keysym.sym == SDLK_a) {
			strafing = -1.0f;
		}
		if (e->keysym.sym == SDLK_d) {
			strafing = 1.0f;
		}
		if (e->keysym.sym == SDLK_e) {
			updown = -1.0f;
		}
		if (e->keysym.sym == SDLK_q) {
			updown = 1.0f;
		}

		// invertmouse
		if (e->keysym.sym == SDLK_i) {
			mousedir *= -1.0f;
		}
		// move speed
		if (e->keysym.sym == SDLK_p) {
			movespd *= 2.0f;
		}
		if (e->keysym.sym == SDLK_o) {
			movespd *= 0.5f;
		}
        if (e->keysym.sym == SDLK_LSHIFT) {
            movespd *= 5.0f;
        }
		// turn around
		if (e->keysym.sym == SDLK_r) {
			ah += 180.0f;
		}

		// testing
		if (e->keysym.sym == SDLK_n) {
			world->modelmanager.v++;
		}
		if (e->keysym.sym == SDLK_b) {
			world->modelmanager.v--;
			if (world->modelmanager.v<0) world->modelmanager.v = 0;
		}

		// toggles
		//if (e->keysym.sym == SDLK_t) {
		//	world->thirdperson = !world->thirdperson;
		//}
		if (e->keysym.sym == SDLK_l) {
			world->lighting = !world->lighting;
		}

		if (e->keysym.sym == SDLK_F1) {
			world->drawmodels = !world->drawmodels;
		}
		if (e->keysym.sym == SDLK_F2) {
			world->drawdoodads = !world->drawdoodads;
		}
		if (e->keysym.sym == SDLK_F3) {
			world->drawterrain = !world->drawterrain;
		}
		if (e->keysym.sym == SDLK_F4) {
			hud = !hud;
		}
		if (e->keysym.sym == SDLK_F6) {
			world->drawwmo = !world->drawwmo;
		}
		if (e->keysym.sym == SDLK_F7) {
			world->drawPolygons = !world->drawPolygons;
		}
		if (e->keysym.sym == SDLK_F8) {
			world->drawPlayerModels = !world->drawPlayerModels;
		}
		if (e->keysym.sym == SDLK_h) {
			world->drawhighres = !world->drawhighres;
		}
		if (e->keysym.sym == SDLK_f) {
			world->drawfog = !world->drawfog;
		}

		if (e->keysym.sym == SDLK_KP_PLUS || e->keysym.sym == SDLK_PLUS) {
			world->fogdistance += 60.0f;
		}
		if (e->keysym.sym == SDLK_KP_MINUS || e->keysym.sym == SDLK_MINUS) {
			world->fogdistance -= 60.0f;
		}

		// minimap
		if (e->keysym.sym == SDLK_m) {
			mapmode = !mapmode;
		}

		/*
		// lighting
		if (e->keysym.sym == SDLK_1) {
			world->l_const -= 0.1f;
			if (world->l_const <= 0) world->l_const = 0.0f;
		}
		if (e->keysym.sym == SDLK_2) {
			world->l_const += 0.1f;
		}
		if (e->keysym.sym == SDLK_3) {
			world->l_linear -= 0.01f;
			if (world->l_linear <= 0) world->l_linear = 0.0f;
		}
		if (e->keysym.sym == SDLK_4) {
			world->l_linear += 0.01f;
		}
		if (e->keysym.sym == SDLK_5) {
			world->l_quadratic -= 0.001f;
			if (world->l_quadratic <= 0) world->l_quadratic = 0.0f;
		}
		if (e->keysym.sym == SDLK_6) {
			world->l_quadratic += 0.001f;
		}
		*/

		if (e->keysym.sym == SDLK_F5) {
			FILE *bf = fopen("bookmarks.txt","a");
			// copied from above: retreive area name for bookmarks, too
			unsigned int areaID = world->getAreaID();
			unsigned int regionID = 0;
			std::string areaName = "";
			try {
				AreaDB::Record rec = gAreaDB.getByAreaID(areaID);
				areaName = rec.getString(AreaDB::Name);
				//regionID = rec.getUInt(AreaDB::Region);
			} catch(AreaDB::NotFound)
			{
				if (world->gnWMO==0) areaName = "Unknown location";
			}
			if (regionID != 0) {
				/// Look up region
				try {
					AreaDB::Record rec = gAreaDB.getByAreaID(regionID);
					areaName = rec.getString(AreaDB::Name);
				} catch(AreaDB::NotFound)
				{
					// do nothing
				}
			}

			//fprintf(bf, "%s %f %f %f  %f %f  %s\n", world->basename.c_str(), world->camera.x, world->camera.y, world->camera.z, ah, av, areaName.c_str());
			fclose(bf);
		}

		// Teleport to playertwo
		if (e->keysym.sym == SDLK_0) {
			world->playermodelis[0].teleToTarget = true;
		}
		if (e->keysym.sym == SDLK_t) {
			world->playermodelis[0].targetIndex = (world->playermodelis[0].targetIndex + 1) % world->playermodelis.size();
			if (world->playermodelis[0].targetIndex == 0 && world->playermodelis.size() > 1) // Don't pick player as target
				world->playermodelis[0].targetIndex = 1;
			world->playermodelis[0].target = &world->playermodelis[world->playermodelis[0].targetIndex];
			std::cout << "New target index: " << world->playermodelis[0].targetIndex << std::endl;
		}

		if (e->keysym.sym == SDLK_1) {
			world->playermodelis[0].chosenSpell = &world->spellmodelis[0];
			if (world->playermodelis[0].chosenSpell && !world->playermodelis[0].chosenSpell->isCasting) {
				world->playermodelis[0].chosenSpell->isHidden = false;
				world->playermodelis[0].chosenSpell->isCasting = true;
				world->playermodelis[0].chosenSpell->target = world->playermodelis[0].target;
				world->playermodelis[0].chosenSpell->pos = world->playermodelis[0].pos;
			}
		}
		if (e->keysym.sym == SDLK_2) {
			world->playermodelis[0].chosenSpell = &world->spellmodelis[1];
			if (world->playermodelis[0].chosenSpell && !world->playermodelis[0].chosenSpell->isCasting) {
				world->playermodelis[0].chosenSpell->isHidden = false;
				world->playermodelis[0].chosenSpell->isCasting = true;
				world->playermodelis[0].chosenSpell->target = world->playermodelis[0].target;
				world->playermodelis[0].chosenSpell->pos = world->playermodelis[0].pos;
			}
		}
		if (e->keysym.sym == SDLK_3) {
			world->playermodelis[0].chosenSpell = &world->spellmodelis[2];
			if (world->playermodelis[0].chosenSpell && !world->playermodelis[0].chosenSpell->isCasting) {
				world->playermodelis[0].chosenSpell->isHidden = false;
				world->playermodelis[0].chosenSpell->isCasting = true;
				world->playermodelis[0].chosenSpell->target = world->playermodelis[0].target;
				world->playermodelis[0].chosenSpell->pos = world->playermodelis[0].pos;
			}
		}
		if (e->keysym.sym == SDLK_4) {
			world->playermodelis[0].chosenSpell = &world->spellmodelis[3];
			if (world->playermodelis[0].chosenSpell && !world->playermodelis[0].chosenSpell->isCasting) {
				world->playermodelis[0].chosenSpell->isHidden = false;
				world->playermodelis[0].chosenSpell->isCasting = true;
				world->playermodelis[0].chosenSpell->target = world->playermodelis[0].target;
				world->playermodelis[0].chosenSpell->pos = world->playermodelis[0].pos;
			}
		}
		if (e->keysym.sym == SDLK_y) {
			world->playermodelis[0].isWandering = true;
		}

		if (e->keysym.sym == SDLK_8) {
			world->playermodelis[0].usePhysics = !world->playermodelis[0].usePhysics;
			//world->thirdperson = !world->thirdperson;
			if (world->playermodelis[0].usePhysics) {
				Vec3D currentNode = gWorld->GetClosestNode(-(world->playermodelis[0].pos.z - ZEROPOINT), -(world->playermodelis[0].pos.x - ZEROPOINT), world->playermodelis[0].pos.y, world->playermodelis[0]);
				std::cout << "New closest node! " << currentNode.x << ", " << currentNode.y << ", " << currentNode.z << std::endl;

				float newPosX = -currentNode.y + ZEROPOINT;
				float newPosY = currentNode.z;
				float newPosZ = -currentNode.x + ZEROPOINT;
				Vec3D targetPos(newPosX, newPosY, newPosZ);
				Vec3D newdir = targetPos - world->playermodelis[0].pos;

				world->playermodelis[0].pos.x = newPosX;
				world->playermodelis[0].pos.y = newPosY;
				world->playermodelis[0].pos.z = newPosZ;
				world->camera = world->playermodelis[0].pos;
				//world->camera = world->playermodelis[0].pos - (world->lookat * 20.0f);

                world->camera = world->playermodelis[0].pos;
                float rotationAngle = -(world->playermodelis[0].dir.y * (PI / 180.0f)) + PI;
                Vec3D newmovedir(cos(rotationAngle + PI), 0, sin(rotationAngle + PI));
                newmovedir.y = 0.0f;
                Vec3D newcampos = gWorld->camera - (newmovedir * distanceBehindCamera);
                newcampos.y += distanceBehindCamera/2;
                world->lookat = world->playermodelis[0].pos;
                world->camera = newcampos;
			}
		}

	} else {
		// key UP

		if (e->keysym.sym == SDLK_w) {
			if (moving > 0) moving = 0;
		}
		if (e->keysym.sym == SDLK_s) {
			if (moving < 0) moving = 0;
		}
		if (e->keysym.sym == SDLK_d) {
			if (strafing > 0) strafing = 0;
		}
		if (e->keysym.sym == SDLK_a) {
			if (strafing < 0) strafing = 0;
		}
		if (e->keysym.sym == SDLK_q) {
			if (updown > 0) updown = 0;
		}
		if (e->keysym.sym == SDLK_e) {
			if (updown < 0) updown = 0;
		}
        if (e->keysym.sym == SDLK_LSHIFT) {
            movespd /= 5.0f;
        }
	}
}

void Test::mousemove(SDL_MouseMotionEvent *e)
{
	if (look || fullscreen) {
		mousedir = -1.0f;
		ah += e->xrel / XSENS;
		av += mousedir * e->yrel / YSENS;
		if (av < -80) av = -80;
		else if (av > 80) av = 80;
	}

}

void Test::mouseclick(SDL_MouseButtonEvent *e)
{
	if (e->type == SDL_MOUSEBUTTONDOWN) {
		look = true;
	} else if (e->type == SDL_MOUSEBUTTONUP) {
		look = false;
	}

}

