#pragma once
#include "SAT.h"
#include <bits\stdc++.h>
#include <time.h>
//#include "SAT.h"
const float PI = 3.14159265359;
using namespace std;
extern bool left_click = 0;
extern float mousey, mousex; extern int score;
extern bool keys[5];
float slowmo = 1;

void drawfromhitbox(vector<vect> vec) {
	glColor3ub(0, 0, 0);
	glBegin(GL_POLYGON);
	for (auto i : vec) {
		glVertex2f(i.x, i.y);
	}
	glEnd();
}
class entity {
public:
	float posx = 0, posy = 0, rot = 0, velo = 0, accel = 0.5, lastx = 0, lasty = 0, initx = 0, inity = 0;
	vector<shape> hitboxes;
	entity(float posx, float posy, float rot) : posx(posx), posy(posy), rot(rot), initx(posx), inity(posy) {}
	bool collision(const entity& en2) {
		for (auto i : hitboxes) {
			for (auto j : en2.hitboxes) {
				if (i.check(j)) {
					return true;
				}
			}
		}
		return false;
	}
	void rotate_point(float& xout, float& yout) {
		float rad = rot * PI / 180;
		float x = xout, y = yout;
		xout = (x - posx) * cos(rad) - (y - posy) * sin(rad) + posx;
		yout = (x - posx) * sin(rad) + (y - posy) * cos(rad) + posy;
	}
	void rotate_point(float& xout, float& yout, float angle) {
		float rad = angle * PI / 180;
		float x = xout, y = yout;
		xout = (x - posx) * cos(rad) - (y - posy) * sin(rad) + posx;
		yout = (x - posx) * sin(rad) + (y - posy) * cos(rad) + posy;
	}

};

class bullet : public entity {
public:
	float velo = 75, rad = rot * PI / 180;
	bool type;
	bool hit = false;
	vector<vect> oldp = { vect(-20,20), vect(20,20), vect({20,-20}), vect({-20,-20}) };
	vector<vect> temp = { vect(-20,20), vect(20,20), vect(20,-20), vect(-20,-20) };
	bullet(float posx, float posy, float rot, bool type) : entity(posx, posy, rot), type(type) {

		for (int i = 0; i < temp.size(); i++) {
			temp[i].x += posx;
			temp[i].y += posy;
			rotate_point(temp[i].x, temp[i].y);
		}
		hitboxes.push_back(shape(temp));
		}
	void draw() {
		glPushMatrix();
		glTranslatef(posx, posy, 0);
		glRotatef(rot, 0, 0, 1.0f);
		glBegin(GL_QUADS);
		glColor3ub(240, 150, 4);
		glVertex2d(-20, 20);
		glVertex2d(20, 20);
		glVertex2d(20, -20);
		glVertex2d(-20, -20);
		glEnd();
		glColor3f(1, 0, 0);
		glPopMatrix();

	}
	bool unneeded(const entity& p1) {
		float disx = p1.posx - posx;
		float disy = p1.posy - posy;
		if (abs(disx) > 2500 || abs(disy) > 2500)
			return 1;
		return 0;
	}
	void move() {
		posx += (velo)*cos(rad)*slowmo;
		posy += (velo)*sin(rad)*slowmo;
		
		for (int i = 0; i < temp.size(); i++) {
			temp[i].x = oldp[i].x + posx;
			temp[i].y = oldp[i].y+posy;
		}
		hitboxes[0]=(shape(temp));
	}

};
vector<bullet> bullet_buffer;
class weapon {
public:
	float cooldown;
	int basecooldown,type;
	bool melee=false, burst_shot=false, spread_shot=false,isenemy=false;
	vector<vect> meleeOriginalHitbox, meleeCurrentHitbox;
	weapon() : type(0), isenemy(false), melee(0), basecooldown(0), cooldown(0), burst_shot(false), spread_shot(false) {}
	weapon(int type, bool isenemy) : type(type), isenemy(isenemy) {
		switch (type)
		{
		case 0: //fists
			melee = 1;
			basecooldown = 10;
			if (isenemy) cooldown = basecooldown;
			else cooldown = 0;
			meleeOriginalHitbox = meleeCurrentHitbox = { {-25,0},{-25,250},{25,250},{25,0} };
		break;
		case 1: //bat
			melee = 1;
			basecooldown = 15;
			if (isenemy) cooldown = basecooldown;
			else cooldown = 0;
			meleeOriginalHitbox = meleeCurrentHitbox = { {-25,0},{-25,250},{25,250},{25,0} };
			break;
		case 2: //pistol
			basecooldown = 50;
			if (isenemy) cooldown = basecooldown;
			else cooldown = 0;
			break;
		case 3: //smg
			burst_shot = true;
			basecooldown = 40;
			if (isenemy) cooldown = basecooldown;
			else cooldown = 0;
			break;
		case 4:
			spread_shot = true;
			basecooldown = 75;
			if (isenemy) cooldown = basecooldown;
			else cooldown = 0;
			break;
		default:
			break;
		}
	}
	void shoot(float posx,float posy,float rot) {
		if (!melee) {
			if (cooldown == 0) {
				if (spread_shot) {
					srand(time(0));
					bullet_buffer.push_back(bullet(posx, posy, rot + ((rand() % 20) * 2 - 20) / 2, isenemy));
					bullet_buffer.push_back(bullet(posx, posy, rot + ((rand() % 20) * 2 - 20) / 2, isenemy));
					bullet_buffer.push_back(bullet(posx, posy, rot + ((rand() % 20) * 2 - 20) / 2, isenemy));
					bullet_buffer.push_back(bullet(posx, posy, rot + ((rand() % 20) * 2 - 20) / 2, isenemy));

				}
				bullet_buffer.push_back(bullet(posx, posy, rot, isenemy));
				cooldown = basecooldown;
			}
		}
		else {

		}
	}
};
class player : public entity
{
public:
	bool gameover = 0, jumpbool = 0, burst_shot = false, spread_shot = false;
	int base_cooldown = 50, wesapon = 3;
	float cooldown = 0; weapon pweapon;
	vector<vect> temp = { { -100,100 },{ 100,100 },{ 100,-100 },{ -100,-100 } };
	vector<vect> oldp = { { -100,100 },{ 100,100 },{ 100,-100 },{ -100,-100 } };
	player(float posx = 0, float posy = 0, float rot = 0) :entity(posx, posy, rot) {
		hitboxes.push_back(shape(oldp));
		pweapon = weapon(2, 0);
	}

	void draw() {
		glPushMatrix();
		glTranslatef(posx, posy, 0);
		glRotatef(rot, 0, 0, 1.0f);

		glColor3f(0, 0, 0.8);
		glBegin(GL_QUADS);
		glVertex2f(-100, 100);
		glVertex2f(-100, -100);
		glVertex2f(100, -100);
		glVertex2f(100, 100);
		glEnd();
		glPopMatrix();
		if (pweapon.cooldown >= 1)
			pweapon.cooldown -= 1 * slowmo;
		else pweapon.cooldown = 0;
	}
	void move() {
		float x = 0, y = 0, maxvelo = 30;
		bool dash = 0;
		for (int i = 0; i < 5; i++) {
			if (keys[i]) {
				switch (i)
				{
				case 0: y += 1; break;
				case 1: x += -1; break;
				case 2:	y += -1; break;
				case 3: x += 1; break;
				default:
					break;
				}
			}
		}
		if (keys[4]) {
			slowmo = 0.1;
		}
		else { slowmo += (1 - slowmo) * 0.05; 
		//cout << slowmo<<endl; 
		}
		if (x != 0 || y != 0) {
			if (velo <= maxvelo)
				velo += accel;
			else velo = maxvelo;
			float smoothing = 0.1;
			float norm = sqrt(x * x + y * y);
			if (norm != 0) {
				x /= norm;
				y /= norm;
			}
			float currx = (1 - smoothing) * lastx + smoothing * x;
			float curry = (1 - smoothing) * lasty + smoothing * y;
			lastx = currx;
			lasty = curry;
		}
		else {
			if (velo > 0.0f)
				velo -= accel;
			else { velo = 0.0; lastx = lasty = 0; }
		}
		posx += lastx * velo*slowmo;
		posy += lasty * velo*slowmo;
		rot = atan2(mousey, mousex);
		rot = rot * 180 / PI;


		for (int i = 0; i < temp.size(); i++) {
			temp[i].x =  oldp[i].x+posx;
			temp[i].y = oldp[i].y + posy;
			rotate_point(temp[i].x, temp[i].y);
		}
		hitboxes.clear();
		hitboxes.push_back(shape(temp));


	}
	void shoot() {
		
		if (left_click) {
			pweapon.shoot(posx,posy,rot);
		}
	}
};
player p1;
struct crosshair {
	float posx = 0, posy = 0;

	void draw() {
		glPushMatrix();
		glTranslatef(posx, posy, 0);
		glColor3ub(155, 125, 0);
		glBegin(GL_QUADS);
		glVertex2d(-20, 20);
		glVertex2d(20, 20);
		glVertex2d(20, -20);
		glVertex2d(-20, -20);
		glEnd();
		posx = mousex;
		posy = mousey;
		glPopMatrix();
		glColor3ub(0, 0, 255);
		glBegin(GL_LINES);
		glVertex2d(0, 0);
		glVertex2d(posx, posy);
		glEnd();
	}
}cross;
class enemy :public entity {
public:
	float velo = 75;
	bool alive = true;
	bool type = 0;
	int frameswhendead = 60;
	int framesleft = 60;
	weapon eweapon;
	vector<vect> oldp = { { -100 ,100  },{ 100,100  },{ 100 ,-100  },{ -100 ,-100  } };
	vector<vect> temp = { { -100 ,100  },{ 100,100  },{ 100 ,-100  },{ -100 ,-100  } };
	enemy(float posx, float posy, float rot, bool type) :entity(posx, posy, rot), type(type) {
		eweapon = weapon(4, 1);
	}
	int draw() {
		// alive

		if (alive) {
			glPushMatrix();
			glTranslatef(posx, posy, 0);
			glRotatef(rot, 0, 0, 1.0f);
			if (type)
				glColor3f(0, 0.839, 0.89);
			else glColor3f(1, 0, 0);
			glBegin(GL_QUADS);
			glVertex2f(-100, 100);
			glVertex2f(-100, -100);
			glVertex2f(100, -100);
			glVertex2f(100, 100);
			glEnd();
			glPopMatrix();
		}
		else {
			glPushMatrix();
			glTranslatef(posx, posy, 0);
			glRotatef(rot, 0, 0, 1.0f);

			glColor3f(0, 0, 0);
			glBegin(GL_QUADS);
			glVertex2f(-100, 100);
			glVertex2f(-100, -100);
			glVertex2f(100, -100);
			glVertex2f(100, 100);
			glEnd();
			glPopMatrix();
		}
		if (!alive) {
			framesleft--;
		}
		if (!framesleft) {
			return 0;
		}
		if (eweapon.cooldown >= 1)
			eweapon.cooldown -= 1 * slowmo;
		else eweapon.cooldown = 0;
	}
	void move(){
		rot = atan2(p1.posy - posy, p1.posx - posx);
		if (!eweapon.melee) {
			float dist = (p1.posx - posx) * (p1.posx - posx) + (p1.posy - posy) * (p1.posy - posy);
			dist = sqrt(dist);
			if (dist > 1200) {
				posx += 10 * slowmo * cos(rot);
				posy += 10 * slowmo * sin(rot);
			}
			cout << posx <<" "<<posy << endl;
		}
		else {
			posx += velo * cos(rot) * slowmo;
			posy += velo * sin(rot) * slowmo;
		}
		rot = rot * 180 / PI;
		for (int i = 0; i < temp.size(); i++) {
			temp[i].x = oldp[i].x + posx;
			temp[i].y = oldp[i].y + posy;
			rotate_point(temp[i].x, temp[i].y);
		}
		hitboxes.clear();
		hitboxes.push_back(shape(temp));
	}
	void shoot() {
		eweapon.shoot(posx, posy, rot);
		cout << "shooting";
	}

};
enemy enemy1(-2000, 1000, 0,0);

class enemy_whooshwhoosh :public enemy {
public:
	bool attacking = 0,end=0;
	float velo = 10;
	float batrot = 0;
	vector<vect> oldp = { { -100 ,100  },{ 100,100  },{ 100 ,-100  },{ -100 ,-100  } };
	vector<vect> temp = { { -100 ,100  },{ 100,100  },{ 100 ,-100  },{ -100 ,-100  } };
	vector<vect> oldbat = { {-25,0},{-25,250},{25,250},{25,0} };
	vector<vect> tempbat = { {-25,0},{-25,250},{25,250},{25,0} };
	enemy_whooshwhoosh(float posx = 0, float posy = 0, float rot = 0) : enemy(posx, posy, rot, 0) {}
	void move() {
		if (alive) {
			rot = atan2(p1.posy - posy, p1.posx - posx);
			posx += velo * cos(rot) * slowmo;
			posy += velo * sin(rot) * slowmo;
			rot = rot * 180 / PI;
			for (int i = 0; i < temp.size(); i++) {
				temp[i].x = oldp[i].x + posx;
				temp[i].y = oldp[i].y + posy;
				rotate_point(temp[i].x, temp[i].y);
			}
			
			hitboxes.clear();
			hitboxes.push_back(shape(temp));
		}
		
	}
	void drawbat() {
		glPushMatrix();
		glTranslatef(posx, posy, 0);
		glRotatef(rot+batrot,0, 0, 1.0f);
		glTranslatef(0,75,0);
		glColor3ub(124, 40, 20);
		glBegin(GL_QUADS);
		glVertex2d(-25, 0);
		glVertex2d(-25, 250);
		glVertex2d(25, 250);
		glVertex2d(25, 0);
		glEnd();
		glPopMatrix();
		if (attacking) {
			if (!end) {
				batrot -= 5*slowmo;
				if (batrot <= -180) end = 1;
			}
			if (end) {
				batrot += 5*slowmo;
				if (batrot >= 0) end = 0;
			}
		}
		else if (batrot < 0) batrot += 3 * slowmo;
	}
	void shoot() {
		if ((posx-p1.posx) * (posx - p1.posx) + (posy-p1.posy)* (posy - p1.posy) <= 250000) {
			if(batrot>=0)
			attacking = 1; 
			for (int i = 0; i < tempbat.size(); i++) {
				tempbat[i].x = oldbat[i].x + posx;
				tempbat[i].y = oldbat[i].y + posy;
				tempbat[i].y += 75;
				if (attacking)
					rotate_point(tempbat[i].x, tempbat[i].y, rot + batrot);
			}
			shape bat(tempbat);
			for (auto i : p1.hitboxes) {
				if (i.check(bat))
					p1.gameover = 1;
			}
		}
		else if(end) attacking = 0;

	}
};
enemy_whooshwhoosh enemy2(1500, 1500), enemy4(-1500, 1500);
vector<enemy_whooshwhoosh> enemy3 = { enemy2,enemy4 };
class Wall {
public:
	void draw() {
		glColor3f(0, 0, 0);
		glBegin(GL_QUADS);
		glVertex2f(31.000, 126.938);
		glVertex2f(31.000, 454.083);
		glVertex2f(37.083, 454.083);
		glVertex2f(37.031, 127.000);
		glVertex2f(63.000, 30.969);
		glVertex2f(62.969, 134.000);
		glVertex2f(69.031, 134.000);
		glVertex2f(69.031, 30.969);
		glVertex2f(31.167, 134.000);
		glVertex2f(64.458, 133.958);
		glVertex2f(64.562, 126.969);
		glVertex2f(31.062, 126.969);
		glVertex2f(31.031, 448.000);
		glVertex2f(31.125, 453.969);
		glVertex2f(165.000, 454.000);
		glVertex2f(165.000, 448.000);
		glVertex2f(158.969, 453.812);
		glVertex2f(158.969, 384.031);
		glVertex2f(164.938, 384.031);
		glVertex2f(165.000, 453.875);
		glVertex2f(158.969, 350.000);
		glVertex2f(159.000, 192.031);
		glVertex2f(164.969, 192.000);
		glVertex2f(165.000, 349.969);
		glVertex2f(31.188, 255.969);
		glVertex2f(31.125, 262.000);
		glVertex2f(164.750, 261.969);
		glVertex2f(164.844, 256.000);
		glVertex2f(160.562, 224.000);
		glVertex2f(188.938, 223.969);
		glVertex2f(189.031, 230.000);
		glVertex2f(159.656, 229.969);
		glVertex2f(158.969, 159.000);
		glVertex2f(159.125, 96.000);
		glVertex2f(164.969, 96.031);
		glVertex2f(165.000, 159.000);
		glVertex2f(63.062, 127.969);
		glVertex2f(63.250, 134.000);
		glVertex2f(161.438, 134.000);
		glVertex2f(161.344, 127.969);
		glVertex2f(62.062, 29.938);
		glVertex2f(324.000, 30.031);
		glVertex2f(324.042, 37.000);
		glVertex2f(62.469, 36.969);
		glVertex2f(159.031, 32.125);
		glVertex2f(158.969, 62.000);
		glVertex2f(165.031, 62.000);
		glVertex2f(164.969, 32.000);
		glVertex2f(319.000, 36.719);
		glVertex2f(319.000, 126.000);
		glVertex2f(325.031, 126.000);
		glVertex2f(325.000, 36.656);
		glVertex2f(319.042, 64.042);
		glVertex2f(389.031, 64.000);
		glVertex2f(388.969, 70.031);
		glVertex2f(319.219, 70.000);
		glVertex2f(383.000, 31.000);
		glVertex2f(383.000, 229.969);
		glVertex2f(389.031, 229.969);
		glVertex2f(389.000, 30.969);
		glVertex2f(383.250, 224.000);
		glVertex2f(476.969, 224.000);
		glVertex2f(477.031, 229.969);
		glVertex2f(383.156, 230.000);
		glVertex2f(510.031, 224.031);
		glVertex2f(612.969, 223.969);
		glVertex2f(613.000, 229.938);
		glVertex2f(509.969, 230.000);
		glVertex2f(383.062, 31.062);
		glVertex2f(613.062, 31.000);
		glVertex2f(613.000, 37.969);
		glVertex2f(383.375, 37.969);
		glVertex2f(607.031, 32.062);
		glVertex2f(606.969, 422.031);
		glVertex2f(613.062, 422.000);
		glVertex2f(613.000, 32.094);
		glVertex2f(611.792, 416.000);
		glVertex2f(478.969, 415.969);
		glVertex2f(479.062, 422.000);
		glVertex2f(612.156, 422.000);
		glVertex2f(478.969, 383.969);
		glVertex2f(479.031, 454.000);
		glVertex2f(485.000, 454.000);
		glVertex2f(485.000, 383.969);
		glVertex2f(485.000, 384.000);
		glVertex2f(382.000, 384.000);
		glVertex2f(382.000, 390.031);
		glVertex2f(485.031, 389.969);
		glVertex2f(482.625, 448.000);
		glVertex2f(221.969, 448.000);
		glVertex2f(221.969, 454.000);
		glVertex2f(482.375, 453.969);
		glVertex2f(222.000, 453.969);
		glVertex2f(221.969, 382.969);
		glVertex2f(229.031, 382.969);
		glVertex2f(229.031, 454.000);
		glVertex2f(159.125, 415.969);
		glVertex2f(159.281, 421.938);
		glVertex2f(228.656, 422.000);
		glVertex2f(228.531, 416.000);
		glVertex2f(229.031, 350.000);
		glVertex2f(229.000, 287.000);
		glVertex2f(221.969, 287.031);
		glVertex2f(221.969, 350.031);
		glVertex2f(221.969, 287.062);
		glVertex2f(389.094, 286.969);
		glVertex2f(389.062, 293.938);
		glVertex2f(222.219, 294.000);
		glVertex2f(382.000, 390.031);
		glVertex2f(381.969, 287.031);
		glVertex2f(389.094, 287.000);
		glVertex2f(389.031, 390.062);
		glVertex2f(413.969, 287.000);
		glVertex2f(413.969, 358.000);
		glVertex2f(421.031, 358.000);
		glVertex2f(421.000, 286.969);
		glVertex2f(414.625, 358.000);
		glVertex2f(485.000, 357.969);
		glVertex2f(485.031, 352.000);
		glVertex2f(414.219, 351.969);
		glVertex2f(485.000, 357.906);
		glVertex2f(485.031, 286.969);
		glVertex2f(479.000, 287.000);
		glVertex2f(479.031, 357.969);
		glVertex2f(414.125, 286.969);
		glVertex2f(484.344, 286.969);
		glVertex2f(484.500, 293.875);
		glVertex2f(417.844, 294.000);
		glVertex2f(319.031, 158.969);
		glVertex2f(319.083, 230.000);
		glVertex2f(325.042, 230.000);
		glVertex2f(325.000, 158.938);
		glVertex2f(325.000, 224.062);
		glVertex2f(222.083, 224.000);
		glVertex2f(222.042, 230.125);
		glVertex2f(325.042, 229.958);
		glEnd();
	}
};

// the goal:
// hidden jam -> the hint is a screen telling you how close you are to it 
// there are two jams one of which is the one you need to get and go to the basemant
// the other is a bomb and you never know which is which
// there is a timer in the back of the game with limited time to finish your goal
// when the timer ends the jam bomb explodes killing you and the players 
// only way out is to get the right jam and getting out through the basement

