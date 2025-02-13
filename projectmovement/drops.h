#pragma once
#include <gl/glut.h>
#include <bits\stdc++.h>
#include <time.h>
#include "loader.h"
extern bool keys[6];
extern float slowmo;
using namespace std;
GLuint smgdrop;
class weapondrop {
public:
	int weapontype;
	float posx, posy, velo = 10, accel = 0.25, rotspeed, deathrot, rot = 0;
	static int x[5], y[5];
	weapondrop(float posx, float posy, float rotspeed, float deathrot, int weapontype) :posx(posx), posy(posy), rotspeed(rotspeed), deathrot(deathrot), weapontype(weapontype) {}
	static void init() {
		loadTexture(smgdrop, "..\\Textures\\Drops\\smgdrop2.png",x[3],y[3]);
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
		
		glTranslatef(posx, posy, 0);
		glRotatef(rot, 0, 0, 1);

			glColor3f(1, 1, 0);
			glBegin(GL_QUADS);
			glVertex2f(-100 / 2, -100 / 2);
			glVertex2f(-100 / 2, 100 / 2);
			glVertex2f(100 / 2, 100 / 2);
			glVertex2f(100 / 2, -100 / 2);
			glEnd();
			tdisplay(smgdrop, 3.5, x[3], y[3], 1, 0);
		glPopMatrix();
	}
};
int weapondrop::x[5], weapondrop::y[5];
