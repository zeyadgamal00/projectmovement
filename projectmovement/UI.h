#pragma once
#include "entities.h"
extern float mousex, mousey;
extern bool left_click;
extern int currentscreen;
#include "Text.h"
#include "Audio.h"
extern stack<int> previousscreen;
extern void quitter(),resetgame();
float* volumes[] = { &masterVolume,&sfxVolume,&musicVolume };
int alreadydragging=0;
GLuint retry[2];

int retryx, retryy;

void retry_init() {

	loadTexture(retry[0], "..\\Textures\\reset1.1.png", retryx, retryy);

	loadTexture(retry[1], "..\\Textures\\reset2.1.png", retryx, retryy);



}
class Buttonitem {
	//Textitem label;
public:
	float posx, posy, sizex, sizey;
	bool ishover = 0;
	function<void()>Buttonfunc;
	vector<GLubyte> colors;
	string text,textcolor;
	int fontsize;
	bool toggleable=0,toggled=0;
	Buttonitem(float sizex, float sizey, float posx, float posy,bool toggleable, function<void()> Buttonfunc, string text = "", int fontsize=64, string color = "#FFFFFF", string textcolor = "#000000") :sizex(sizex), sizey(sizey), posx(posx), posy(posy), Buttonfunc(Buttonfunc), text(text), textcolor(textcolor),fontsize(fontsize) {
		colors = hextorgb(color);
		this->toggleable = toggleable;
	}
	void hover() {
		if ((mousex >= posx - sizex / 2 && mousex <= posx + sizex / 2) && (mousey >= posy - sizey / 2 && mousey <= posy + sizey / 2)) {
			ishover = 1;
			cross.hover = true;
		}
		else if (ishover) {
			ishover = 0;
			cross.hover = false;
		}
	}
	void onClick() {
		if (left_click && ishover) {
			Buttonfunc();
			if (toggleable) toggled = !toggled; 
			left_click = 0;
			cross.hover = false;

		}
	}
	void draw() {
		if (text == "retry") {
			glPushMatrix();
			glColor3ubv(colors.data());
			glTranslatef(posx, posy, 0);
			tdisplay(retry[ishover], 6, retryx, retryy, 1, 0);
			glPopMatrix();
			return;
		}
		if (toggleable) {
			glPushMatrix();
			glTranslatef(posx, posy, 0);
			if (!toggled) { glColor3f(1, 1, 1); }
			else glColor3f(0.929, 0.929, 0);
			glBegin(GL_QUADS);
			glVertex2d(-sizex / 2, -sizey / 2);
			glVertex2d(-sizex / 2, sizey / 2);
			glVertex2d(sizex / 2, sizey / 2);
			glVertex2d(sizex / 2, -sizey / 2);
			glEnd();
			renderText(text + (toggled ? " On" : " Off"), sizex / 2, -20, fontsize, "FFFFFF");
			glPopMatrix();
		}
		else {
			glPushMatrix();
			glColor3ubv(colors.data());
			glTranslatef(posx, posy, 0);
			glBegin(GL_QUADS);
			glVertex2d(-sizex / 2, -sizey / 2);
			glVertex2d(-sizex / 2, sizey / 2);
			glVertex2d(sizex / 2, sizey / 2);
			glVertex2d(sizex / 2, -sizey / 2);
			glEnd();
			renderCenteredText(text, 0, -20, fontsize, textcolor);
			glPopMatrix();
		}

	}
};
class Bar {
public:
	float posx, posy, sizex, sizey, indicator_width, slidervalue, indicatorxpos;
	float minvalue=0, maxvalue=1;
	bool isslider=0,isdragging=0,ishover=false;
	int sliderid;
	Bar(int sliderid,float posx, float posy, float sizex, float sizey, float iwidth = 0, float value = 0, float min = 0, float max = 1) :sliderid(sliderid), posx(posx), posy(posy), sizex(sizex), sizey(sizey), indicator_width(iwidth), slidervalue(value), minvalue(min), maxvalue(max) {

		value = max(minvalue, value);
		value = min(maxvalue, value);
		indicatorxpos = ((value - minvalue) / (maxvalue - minvalue)) * (sizex-indicator_width);
		isslider = (iwidth != 0);
	}
	void draw() {
		glPushMatrix();
		glTranslatef(posx, posy, 0);
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		glVertex2f(-sizex / 2, -sizey / 2);
		glVertex2f(-sizex / 2, sizey / 2);
		glVertex2f(sizex / 2, sizey / 2);
		glVertex2f(sizex / 2, -sizey / 2);
		glEnd();
		glPopMatrix();
		if (isslider) {
			glPushMatrix();
			glColor3f(0, 0, 0);
			glTranslatef(posx-sizex/2 + indicatorxpos+indicator_width/2, posy, 0);
			glBegin(GL_QUADS);
			glVertex2f(-indicator_width / 2, -sizey / 2);
			glVertex2f(-indicator_width / 2, sizey / 2);
			glVertex2f(indicator_width / 2, sizey / 2);
			glVertex2f(indicator_width / 2, -sizey / 2);
			glEnd();
			glPopMatrix();
		}
	}
	void drag() {
		if (alreadydragging && !isdragging)
			return;
		if (isslider) {
			if ((mousex >= posx - sizex / 2 && mousex <= posx + sizex / 2) && (mousey >= posy - sizey / 2 && mousey <= posy + sizey / 2)) {
				ishover = 1;
				cross.hover = true;
			}
			else if (ishover) {
				ishover = 0;
				cross.hover = false;
			}

			if (ishover || isdragging)
				if (left_click) {
					if (!isdragging) {
						isdragging = 1;
						alreadydragging = true;
					}
					float leftEdge = posx - sizex / 2;
					float x_offset = mousex - leftEdge;
					float adjusted_offset = x_offset - indicator_width / 2;
					float normalized = adjusted_offset / (sizex - indicator_width);
					if (normalized < 0) normalized = 0;
					if (normalized > 1) normalized = 1;
					indicatorxpos = normalized * (sizex - indicator_width);
					slidervalue = minvalue + normalized * (maxvalue - minvalue);
					*volumes[sliderid] = slidervalue;
					changeVolume();
					cross.hover = true;
				}
				else if(isdragging) {
					if (sliderid == 1) {
						playSound(2);
					}
					isdragging = 0;
					alreadydragging = 0;
				}
		}

	}
	void specialdraw(float ratio,float r,float g,float b) {
		glPushMatrix();
		glTranslatef(posx, posy, 0);
		glColor3f(r, g, b);
		glBegin(GL_QUADS);
		glVertex2f(0, sizey / 2);
		glVertex2f(0, -sizey / 2);
		glVertex2f(sizex*ratio, -sizey / 2);
		glVertex2f(sizex*ratio, sizey / 2);
		glEnd();
		glPopMatrix();
	}
};
Buttonitem retrybutton(400, 200, 1000, -1000, 0, [&]() {resetgame(); }, "retry");
Buttonitem startbutton(500, 200, 0, 150, 0, [&]() {currentscreen = 1; previousscreen = stack<int>();  playMusic(1); resetgame(); }, "Play");
Buttonitem settingsbutton(500, 200, 0, -150,0, [&]() {previousscreen.push(currentscreen); currentscreen = 2; }, "Settings");
Buttonitem exitbutton(500, 200, 0, -450,0, [&]() { quitter(); }, "Exit");
Buttonitem backbutton(200, 200, -1050, 1050,0, [&]() {
	if (!previousscreen.empty()) {
		currentscreen = previousscreen.top();
		previousscreen.pop();
	}},"<");
Buttonitem cheatsbutton(500, 200, 850, -1000,0, [&]() {previousscreen.push(currentscreen); currentscreen = 3; },"Cheats");
Buttonitem invulnerabilitybutton(100, 100, -800, 600, 1, [&] {invulnerability = !invulnerability; }, "Invulnerability:");
Buttonitem infslowmobutton(100, 100, -800, 400, 1, [&] {infslowmo = !infslowmo; }, "Infinite slow motion:");
Buttonitem nocooldownbutton(100, 100, -800, 200, 1, [&] {nocooldown = !nocooldown; }, "No Cooldown:");
Buttonitem weapon5button(100, 100, -800, 0, 1, [&] {weapon5 = !weapon5; }, "Secret Weapon:");

Bar masterAudioBar(0,100, 400, 600, 100, 100, 1.1);
Bar sfxBar(1,100, 250, 600, 100, 100, 1.1);
Bar musicBar(2,100, 100, 600, 100, 100, 1.1);

Bar slowmoBar(-1, 600, -1100, 500, 50);
Bar cooldownBar(-1, -1100, -1100, 500, 50);