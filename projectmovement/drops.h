#pragma once
#include <gl/glut.h>
#include <bits\stdc++.h>
#include <time.h>
#include "loader.h"
extern bool keys[6];
extern float slowmo;
using namespace std;
GLuint smg_drop,bat_drop,shotgun_drop,pistol_drop,cheat_drop;
class weapondrop {
public:
	int weapontype;
	float posx, posy, velo = 10, accel = 0.25, rotspeed, deathrot, rot = 0;
	static int x[6], y[6];
	weapondrop(float posx, float posy, float rotspeed, float deathrot, int weapontype) :posx(posx), posy(posy), rotspeed(rotspeed), deathrot(deathrot), weapontype(weapontype) {
		velo = float(rand() % 10)+5;
	}
	static void init() {
		loadTexture(bat_drop, "..\\Textures\\Drops\\bat.png", x[1], y[1]);
		loadTexture(pistol_drop, "..\\Textures\\Drops\\pistol.png", x[2], y[2]);
		loadTexture(smg_drop, "..\\Textures\\Drops\\smg.png", x[3], y[3]);
		loadTexture(shotgun_drop, "..\\Textures\\Drops\\shotgun.png",x[4],y[4]);
		loadTexture(cheat_drop, "..\\Textures\\Drops\\cheat.png", x[5], y[5]);


	}
	void move() {
		if (velo >= 1) {
			velo -= accel;
			posx += velo * cos(-deathrot) * slowmo;
			posy += velo * sin(-deathrot) * slowmo;
			rot += rotspeed;
		}
	}
	void draw() {
		glPushMatrix();
		glTranslated(posx, posy, 0);
		glRotatef(rot, 0, 0, 1);
		switch (weapontype)
		{
		case 1: //bat
			tdisplay(bat_drop, 3.5, x[1], y[1], 1, 0);
			break;
		case 2: //pistol
			tdisplay(pistol_drop, 3.5, x[2], y[2], 1, 0);
			break;
		case 3: //smg
			tdisplay(smg_drop, 3.5, x[3], y[3], 1, 0);
			break;
		case 4: //shutgon
			tdisplay(shotgun_drop, 3.5, x[4], y[4], 1, 0);
			break;
		case 5: //shutgon
			tdisplay(cheat_drop, 3.5, x[5], y[5], 1, 0);
			break;
		default:
			glColor3f(1, 0, 0);
			glBegin(GL_QUADS);
			glVertex2f(-50, -50);
			glVertex2f(-50, 50);
			glVertex2f(50, 50);
			glVertex2f(50, -50);
			glEnd();
			break;
		}
		glPopMatrix();
	}
};
int weapondrop::x[6], weapondrop::y[6];
