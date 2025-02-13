#pragma once
#include <gl\glut.h>
#include <bits\stdc++.h>
extern bool keys[6];
extern float slowmo;
using namespace std;
class weapondrop {
public:
	int weapontype;
	float posx, posy, velo=10,accel=0.25,rotspeed,deathrot,rot=0;
	weapondrop(float posx, float posy, float rotspeed, float deathrot,int weapontype) :posx(posx), posy(posy), rotspeed(rotspeed), deathrot(deathrot),weapontype(weapontype) {}
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
		glColor3f(1,1,0);
		glTranslatef(posx, posy, 0);
		glRotatef(rot, 0, 0, 1);
		glBegin(GL_QUADS);
		glVertex2f(-100 / 2, -100 / 2);
		glVertex2f(-100 / 2, 100 / 2);
		glVertex2f(100 / 2, 100 / 2);
		glVertex2f(100 / 2, -100 / 2);
		glEnd();
		glPopMatrix();
	}
};
