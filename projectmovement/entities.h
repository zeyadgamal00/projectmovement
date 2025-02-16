#pragma once
#include "SAT.h"
#include <bits\stdc++.h>
#include <time.h>
#include "drops.h"
#include "Audio.h"
//#include "SAT.h"
const float PI = 3.14159265359;
using namespace std;
extern bool left_click = 0;
extern float mousey, mousex; extern int score;
extern bool keys[6],ispaused;
float slowmo=1;
 float impact;
 vector<weapondrop> dropsbuffer;
void hitenemywithbat(shape);
void rotate_point(float& xout, float& yout,float posx,float posy, float angle) {
	float rad = angle * PI / 180;
	float x = xout, y = yout;
	xout = (x - posx) * cos(rad) - (y - posy) * sin(rad) + posx;
	yout = (x - posx) * sin(rad) + (y - posy) * cos(rad) + posy;
}
void drawfromhitbox(vector<vect> vec) {
	glColor3ub(255, 255, 255);
	glBegin(GL_POLYGON);
	for (auto &i : vec) {
		glVertex2f(i.x, i.y);
	}
	glEnd();
}
class entity {
public:
	float posx = 0, posy = 0, rot = 0, velo = 0, accel = 0.08, lastx = 0, lasty = 0, initx = 0, inity = 0;
	vector<shape> hitboxes;
	entity(float posx, float posy, float rot) : posx(posx), posy(posy), rot(rot), initx(posx), inity(posy) {}
	bool collision(const entity& en2) {
		for (auto &i : hitboxes) {
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
GLuint bullet_texture;
class bullet : public entity {
public:
	float velo = 25, rad;
	bool type;
	bool hit = false;
	bool initialized = false;
	static int x, y;
	static vector<vect> oldp;
	vector<vect> temp;
	bullet(float posx, float posy, float rot, bool type) : entity(posx, posy, rot), type(type) {
		temp = oldp;
		rad = rot * PI / 180;
		for (int i = 0; i < temp.size(); i++) {
			temp[i].x += posx;
			temp[i].y += posy;
			rotate_point(temp[i].x, temp[i].y);
		}
		hitboxes.push_back(shape(temp));
	}
	static void init() {
		loadTexture(bullet_texture, "..\\Textures\\bullet.png", x, y);
		float hx = x / 2.0f, hy = y / 2.f;
		oldp = { vect(-hx, hy), vect(hx ,hy), vect(hx,-hy), vect(-hx,-hy) };
	}
	void draw() {
		glPushMatrix();
		glTranslatef(posx, posy, 0);
		glRotatef(rot, 0, 0, 1.0f);
		tdisplay(bullet_texture, 1.5, x, y, 1, 0);
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
			rotate_point(temp[i].x, temp[i].y);
		}
		hitboxes[0] = (shape(temp));
	}

};
int bullet::x = 0;
int bullet::y = 0;
vector<vect> bullet::oldp;
GLuint player_pistol, player_shutgun, player_bat, player_fist, enemytex[2], player_smg,player_dead;
vector<bullet> bullet_buffer;
class weapon {
public:
	float cooldown=0,batrot=0,move=0;
	int basecooldown=0,type,shotsleft=0,soundtoplay=0,ammo=0;
	bool melee=false, burst_shot=false, spread_shot=false,isenemy=false,isswing=false,isstab=false,attacking=false,bursting=false,shooting=0;
	vector<vect> meleeOriginalHitbox, meleeCurrentHitbox;
	shape weaponhitbox;
	weapon() : type(0), isenemy(false), melee(0), basecooldown(0), cooldown(0), burst_shot(false), spread_shot(false) {}
	weapon(int type, bool isenemy) : type(type), isenemy(isenemy) {
		switch (type)
		{
		case 0: //fists
			melee = 1;
			basecooldown = 100;
			if (isenemy) cooldown = basecooldown;
			else cooldown = 0;
			meleeOriginalHitbox = meleeCurrentHitbox = { {-25,-100},{-25,75},{25,75},{25,-100} };
			isstab = true;
		break;
		case 1: //bat
			melee = 1;
			basecooldown = 350;
			if (isenemy) cooldown = basecooldown;
			else cooldown = 0;
			meleeOriginalHitbox = meleeCurrentHitbox = { {-25,0},{-25,100},{25,100},{25,0} };
			isswing = true;
			break;
		case 2: //pistol
			basecooldown = 300;
			if (isenemy) cooldown = basecooldown;
			else cooldown = 0;
			ammo = 10;
			break;
		case 3: //smg
			burst_shot = true;
			basecooldown = 300;
			if (isenemy) { basecooldown = 300; cooldown = basecooldown; }
			else cooldown = 0;
			shotsleft = 3;
			ammo = 30;
			break;
		case 4:
			spread_shot = true;
			basecooldown = 400;
			if (isenemy) cooldown = basecooldown;
			else cooldown = 0;
			ammo = 5;
			break;
		case 5:
			spread_shot = true;
			burst_shot = true;
			basecooldown = 400;
			if (isenemy) cooldown = basecooldown;
			else cooldown = 0;
			shotsleft = 3;
			break;
		default:
			break;
		}
	}
	void stab(float posx,float posy,float rot) {
		if (cooldown < 1 && isstab) {
			if (attacking) {
				updatefisthitbox(posx, posy, rot,move);
				weaponhitbox = shape(meleeCurrentHitbox);
				drawfromhitbox(meleeCurrentHitbox);
				move+=3;
			}
			if (move >= 20) {
				move = 0;
				attacking = 0;
				cooldown = basecooldown;
			}
		}
	}
	void swing(float posx,float posy,float rot) {
		if (cooldown < 1.f && isswing) {
			if (attacking == 0) {
				playSound(type);
			}
			if (attacking) {
				updatemeleehitbox(posx, posy, rot);
				batrot += 2.5 * slowmo;
				weaponhitbox = shape(meleeCurrentHitbox);
				drawfromhitbox(meleeCurrentHitbox);
			}
			if (batrot >= 180) {
				attacking = 0;
			}
			if (!attacking) {
				batrot = 0;
				cooldown = basecooldown;
			}
		}
		else attacking = 0;
	}
	void shoot(float posx,float posy,float rot) {
		if (!melee) {
			if (cooldown < 1) {
				bullet_buffer.push_back(bullet(posx, posy, rot, isenemy));
				if (spread_shot) {
					bullet_buffer.push_back(bullet(posx, posy, rot + ((rand() % 20) * 2 - 20) / 2, isenemy));
					bullet_buffer.push_back(bullet(posx, posy, rot + ((rand() % 20) * 2 - 20) / 2, isenemy));
					bullet_buffer.push_back(bullet(posx, posy, rot + ((rand() % 20) * 2 - 20) / 2, isenemy));
					bullet_buffer.push_back(bullet(posx, posy, rot + ((rand() % 20) * 2 - 20) / 2, isenemy));
				}
				if (burst_shot) {
					handleburstshots();
				}
				else cooldown = basecooldown;
				shooting = 1;
				if(!isenemy)
				impact = 15;
				if (type == 5) {
					playSound(4);
				}
				else playSound(type);
			}
			else shooting = 0;
		}
	}
	private:
		void handleburstshots() {
			if (shotsleft > 0) {
				shotsleft--;
				cooldown = 10;
				bursting = 1;
			}
			if (!shotsleft) {
				shotsleft = 3;
				bursting = 0;
				cooldown = basecooldown;
			}
		}
		void updatemeleehitbox(float posx,float posy,float rot) {
			for (int i = 0; i < meleeCurrentHitbox.size(); i++) {
				meleeCurrentHitbox[i].x = meleeOriginalHitbox[i].x + posx;
				meleeCurrentHitbox[i].y = meleeOriginalHitbox[i].y + posy+75;
				rotate_point(meleeCurrentHitbox[i].x, meleeCurrentHitbox[i].y, posx, posy, rot + batrot);
			}
		}
		void updatefisthitbox(float posx, float posy, float rot,int move) {
			for (int i = 0; i < meleeCurrentHitbox.size(); i++) {
				meleeCurrentHitbox[i].x = meleeOriginalHitbox[i].x + posx+75-move;
				meleeCurrentHitbox[i].y = meleeOriginalHitbox[i].y + posy+move;
				rotate_point(meleeCurrentHitbox[i].x, meleeCurrentHitbox[i].y, posx, posy, rot+25);
			}
		}
};
int px[5], py[5];

int  animate = 0, selected = 0;

int dx, dy, dselected;

void pload() {

	loadTexture(player_fist, "..\\Textures\\Player\\4.png", px[0], py[0]);

	loadTexture(player_bat, "..\\Textures\\Player\\3.1.png", px[1], py[1]);

	loadTexture(player_pistol, "..\\Textures\\Player\\1.2.png", px[2], py[2]);

	loadTexture(player_smg, "..\\Textures\\Player\\smg.1.png", px[3], py[3]);

	loadTexture(player_shutgun, "..\\Textures\\Player\\2.1.png", px[4], py[4]);

	loadTexture(player_dead, "..\\Textures\\Player\\d.png", dx, dy);

}
class player : public entity
{
public:
	bool gameover = 0;
	weapon pweapon;
	vector<vect> temp = { { -100,100 },{ 100,100 },{ 100,-100 },{ -100,-100 } };
	vector<vect> oldp = { { -100,100 },{ 100,100 },{ 100,-100 },{ -100,-100 } };
	player(int weapontype=2,float posx = 0, float posy = 0, float rot = 0) :entity(posx, posy, rot) {
		hitboxes.push_back(shape(oldp));
		pweapon = weapon(2, 0);
		loadhitboxes();
	}
	void init() {
		float hx = px[0] * 3.5 / 2.f, hy = py[0] * 3.5 / 2.f;
		//oldp = { vect(-hx, hy), vect(hx ,hy), vect(hx,-hy), vect(-hx,-hy) };
		hitboxes.push_back(shape(oldp));

		selected = 0;
		if (pweapon.type == 1)//bat
			selected = 20;

		if (pweapon.type == 0)//fist
			selected = 11;

	}
	void loadhitboxes() {
		switch (pweapon.type)
		{
		case 0:
			oldp = { vect(-85, 60), vect(-85 ,-110), vect(20,-110), vect(20,60) };
			break;
		case 1:
			oldp = { vect(-85, 60), vect(-85 ,-90), vect(20,-90), vect(20,60) };
			break;
		case 2:
			oldp = { vect(-155, 40), vect(-155 ,-40), vect(20,-40), vect(20,40) };
			break;
		case 3:
			oldp = { vect(-155, 40), vect(-155 ,-40), vect(20,-40), vect(20,40) };
			break;
		case 4:
			oldp = { vect(-85, 55), vect(-85 ,-55), vect(50,-55), vect(50,55) };
			break;
		default:
			oldp = { vect(-155, 40), vect(-155 ,-40), vect(20,-40), vect(20,40) };
			break;
		}
	}
	void draw() {
		if (gameover) {

			glPushMatrix();

			glTranslatef(posx, posy, 0);

			glRotatef(rot, 0, 0, 1);

			tdisplay(player_dead, 3.5, dx, dy, 9, dselected);

			glPopMatrix();

			return;

		}
		drawfromhitbox(temp);
		glPushMatrix();
		//glGenTextures(1, &idk);
		glTranslatef(posx,posy,0);
		glRotatef(rot + 90, 0, 0, 1);
		switch (pweapon.type)
			{
			case 0: //fists
				glRotatef(90, 0, 0, 1);
				if (animate) selected--;
				tdisplay(player_fist, 3.5, px[0], py[0], 4, selected / 3);
				if (selected == 0) {
					animate = 0;
					selected = 11;
				}
				break;
			case 1: //bat
				glRotatef(90, 0, 0, 1);
				if (animate) selected++;
				tdisplay(player_bat, 3.5, px[1], py[1], 7	, selected/3);
				if (selected == 20) {
					animate = 0;
				}
				break;
			case 2: //pistol
				if (animate) selected++;
				tdisplay(player_pistol, 3.5, px[2], py[2], 2, !selected);
				if (selected == 6) {
					animate = 0;
					selected = 0;
				}
				break;
			case 3: //smg
				if (animate) selected++;
				tdisplay(player_smg, 3.5, px[3], py[3], 2, !selected);
				if (selected == 5) {
					animate = 0;
					selected = 0;
				}
				break;
			case 4: //shutgon
				if (animate) selected++;
				tdisplay(player_shutgun, 3.5, px[4], py[4], 5, selected / 2);
				if (selected == 9) {
					animate = 0;
					selected = 0;
				}
				break;
			default:
				if (animate) selected++;
				tdisplay(player_pistol, 3.5, px[2], py[2], 2, !selected);
				if (selected == 6) {
					animate = 0;
					selected = 0;
				}
				break;
			}


		glPopMatrix();

		if (pweapon.cooldown >= 1)
			pweapon.cooldown -= 1 * slowmo*!ispaused;
		else pweapon.cooldown = 0;
	}
	void move() {
		float x = 0, y = 0, maxvelo = 12;
		for (int i = 0; i < 5; i++) {
			if (keys[i] && !pweapon.attacking) {
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
		else { slowmo += (1 - slowmo) * 0.05; } 
		slowmotionsoundhandler();
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
		if (!pweapon.attacking) {
			rot = atan2(mousey, mousex);
			rot = rot * 180 / PI;
		}


		for (int i = 0; i < temp.size(); i++) {
			temp[i].x =  oldp[i].x+posx;
			temp[i].y = oldp[i].y + posy;
			rotate_point(temp[i].x, temp[i].y);
		}
		hitboxes.clear();
		hitboxes.push_back(shape(temp));
	}
	void shoot() {
		if (left_click && pweapon.cooldown < 1) {
			if (pweapon.type == 1 && !animate) selected = 0;
			animate = true;
		}
		if (left_click || pweapon.bursting) {
			if (!pweapon.melee) {
				pweapon.shoot(posx, posy, rot);
			}
			else {
				if (pweapon.cooldown < 1 && !pweapon.attacking){
					pweapon.attacking = 1;
					playSound(pweapon.type);
				}

			}
			
		}
		if (pweapon.attacking) {
			if (pweapon.isstab) {
				pweapon.stab(posx, posy, rot - 90);
			}
			else pweapon.swing(posx, posy, rot - 180);
			hitenemywithbat(pweapon.weaponhitbox);
		}
		
	}
	void reset() {
		*this = player(pweapon.type);
		this->init();
	}
	bool playerinrangeofdrops(weapondrop &d) {
		if ((d.posx - posx) * (d.posx - posx) + (d.posy - posy) * (d.posy - posy) <= 250*250) return true;
		return false;
	}
	void pickup() {
		vector<weapondrop> next;
		for (auto &i : dropsbuffer) {
			if (playerinrangeofdrops(i) && keys[5]) {
				pweapon = weapon(i.weapontype, 0);
				selected = 0;
				if (pweapon.type == 1)//bat
					selected = 20;

				if (pweapon.type == 0)//fist
					selected = 11;
				animate = 0;
				loadhitboxes();
				playSound(5);
			}
			else next.push_back(i);
		}
		dropsbuffer = next;
	}
};
player p1(3);

GLuint crosshair_texture,crosshairhover_texture;
struct crosshair {
	float posx = 0, posy = 0;
	bool hover = false;
	int x, y;
	void init() {
		loadTexture(crosshair_texture, "..\\Textures\\crosshair.png", x, y);
		loadTexture(crosshairhover_texture, "..\\Textures\\crosshairhover.png", x, y);
	}
	void draw() {
		glPushMatrix();
		glTranslatef(posx, posy, 0);
		if (hover) tdisplay(crosshairhover_texture, 2, x, y, 1, 0);
		else tdisplay(crosshair_texture, 2, x, y, 1, 0);
		posx = mousex;
		posy = mousey;
		glPopMatrix();
		/*glColor3ub(0, 0, 255);
		glBegin(GL_LINES);
		glVertex2d(0, 0);
		glVertex2d(posx, posy);
		glEnd();*/
	}
}cross;
GLuint enemy_bat, enemy_pistol, enemy_smg, enemy_shutgun;
class enemy :public entity {
public:
	int  animate = 0, selected = 0;
	float velo = 5.f;
	bool alive = true;
	static int x[5], y[5];
	weapon eweapon;
	vector<vect> oldp = { { -100 ,100  },{ 100,100  },{ 100 ,-100  },{ -100 ,-100  } };
	vector<vect> temp = { { -100 ,100  },{ 100,100  },{ 100 ,-100  },{ -100 ,-100  } };
	enemy(float posx, float posy, float rot, int weapontype=1 ) :entity(posx, posy, rot){
		eweapon = weapon(weapontype, 1);
		loadhitboxes();
		hitboxes.push_back(shape(oldp));

	}

	static void init() {

		loadTexture(enemy_bat, "..\\Textures\\Enemy1\\2.png", x[1], y[1]);

		loadTexture(enemy_pistol, "..\\Textures\\Enemy1\\1.1.png", x[2], y[2]);

		loadTexture(enemy_smg, "..\\Textures\\Enemy1\\3.png", x[3], y[3]);

		loadTexture(enemy_shutgun, "..\\Textures\\Enemy1\\4.1.png", x[4], y[4]);

		//for (int i = 0;i < 5;i++) {

		float hx = x[2] * 3.5 / 2, hy = y[2] * 3.5 / 2;
		//}
	}
	void loadhitboxes() {
		switch (eweapon.type)
		{
		case 1:
			oldp = { vect(-105, 140), vect(-105 ,0), vect(20,0), vect(20,140) };
			break;
		case 2:
			oldp = { vect(-155, 40), vect(-155 ,-40), vect(20,-40), vect(20,40) };
			break;
		case 3:
			oldp = { vect(-155, 40), vect(-155 ,-40), vect(20,-40), vect(20,40) };
			break;
		case 4:
			oldp = { vect(-85, 55), vect(-85 ,-55), vect(50,-55), vect(50,55) };
			break;
		default:
			oldp = { vect(-155, 40), vect(-155 ,-40), vect(20,-40), vect(20,40) };
			break;
		}
		temp = oldp;
	}
	void draw() {
		// alive
			drawfromhitbox(temp);
			glPushMatrix();
			glTranslatef(posx, posy, 0);
			glRotatef(rot+90, 0, 0, 1.0f);
			switch (eweapon.type)
			{
			case 1: //bat
				glRotatef(90, 0, 0, 1);
				if (animate) selected++;
				tdisplay(enemy_bat, 3.5, x[1], y[1], 8, selected / 3);
				if (selected == 23) {
					animate = 0;
					selected = 0;
				}
				break;
			case 2: //pistol
				if (animate) selected++;
				tdisplay(enemy_pistol, 3.5, x[2], y[2], 3, bool(selected));
				if (selected == 6) {
					animate = 0;
					selected = 0;
				}
				break;
			case 3: //smg
				if (animate) selected++;
				tdisplay(enemy_smg, 3.5, x[3], y[3], 2, !selected);
				if (selected == 7) {
					animate = 0;
					selected = 0;
				}
				break;
			case 4: //shutgon
				if (animate) selected++;
				tdisplay(enemy_shutgun, 3.5, x[4], y[4], 7, selected / 6);
				if (selected == 41) {
					animate = 0;
					selected = 0;

				}
				break;
			default:
				if (animate) selected++;
				tdisplay(player_pistol, 3.5, x[2], y[2], 2, !selected);
				if (selected == 6) {
					animate = 0;
					selected = 0;
				}
				break;
			}
			glPopMatrix();
		if (eweapon.cooldown >= 1)
			eweapon.cooldown -= 1 * slowmo*!ispaused;
		else eweapon.cooldown = 0;
	}
	void move(){
		if (!eweapon.attacking) {
			rot = atan2(p1.posy - posy, p1.posx - posx);
			float dist = (p1.posx - posx) * (p1.posx - posx) + (p1.posy - posy) * (p1.posy - posy);
			dist = sqrt(dist);
			if (!eweapon.melee) {
				if (dist > 1200) {
					posx += velo * slowmo * cos(rot);
					posy += velo * slowmo * sin(rot);
				}
			}
			else if(dist>150) {
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
	}
	bool playerinrange(float radius) {
		if ((posx - p1.posx) * (posx - p1.posx) + (posy - p1.posy) * (posy - p1.posy) <= radius * radius) return true;
		return false;
	}
	void shoot() {
		
		if (eweapon.melee){
			if (playerinrange(150) || eweapon.attacking) {
				if (eweapon.attacking == 0 && eweapon.cooldown<1) {
					eweapon.attacking = 1;
					playSound(eweapon.type);
					animate = true;
				}
				eweapon.swing(posx, posy, rot-180);
				if (!eweapon.weaponhitbox.vertices.empty()) {
					for (auto& i : p1.hitboxes) {
						if (i.check(eweapon.weaponhitbox)) { p1.rot = rot - 90; p1.gameover = 1; dselected = rand() % 9; }
					}
				}
			}
		}
		else {
			if (playerinrange(1200)) {
				if (!eweapon.cooldown)animate = true;
				eweapon.shoot(posx, posy, rot);
			}
		}
	}
	void drop() {
		dropsbuffer.push_back(weapondrop(posx, posy, ((rand() % 2) == 0 ? 1 : -1)*(rand() % 101) / 100 - 1,rot,eweapon.type));
	}
};
int enemy::x[5] = { 0 };
int enemy::y[5] = { 0 };
vector<enemy> enemybuffer;
void hitenemywithbat(shape bat) {
	if(!bat.vertices.empty())
	for (auto& enemy : enemybuffer) {
		if (enemy.playerinrange(250) && enemy.alive) {
			for (auto& i : enemy.hitboxes) {
				if (i.check(bat)) {
					enemy.alive = 0;
					enemy.drop();
				}
			}
		}
	}
}
GLuint dead_enemy_texture[2];
struct dead_enemy {
	int selected;
	float angle, posx, posy;
	static int x, y;
	dead_enemy(float x, float y, float angle) : posx(x), posy(y), angle(angle) {
		selected = rand() % 2;
	}
	static void init() {
		loadTexture(dead_enemy_texture[0], "..\\Textures\\Enemy1\\d1.png", x, y);
		loadTexture(dead_enemy_texture[1], "..\\Textures\\Enemy1\\d2.png", x, y);
	}
	void diplay() {
		glPushMatrix();
		//glGenTextures(1, &idk);
		glTranslatef(posx, posy, 0);
		glRotatef(angle, 0, 0, 1);
		tdisplay(dead_enemy_texture[selected], 3.5, x, y, 1, 0);
		glPopMatrix();
	}
};
vector<dead_enemy> dead_enemy_buffer;
int dead_enemy::x; int dead_enemy::y;
void tex_init() {
	pload();
	p1.init();
	cross.init();
	enemy::init();
	bullet::init();
	dead_enemy::init();
	weapondrop::init();
}

