#pragma once
#include "entities.h"
extern float mousex, mousey;
extern bool left_click;
extern int currentscreen;
#include "Text.h"
extern stack<int> previousscreen;
class Textitem {
public:
	string text;
	float fontsize,sizex,sizey;
	float posx=0, posy=0;
	Textitem(string text, float fontsize, float sizex, float sizey, float posx, float posy) : text(text), fontsize(fontsize), sizex(sizex), sizey(sizey), posx(posx), posy(posy) {}
};
class Buttonitem {
	//Textitem label;
public:
	float posx, posy, sizex, sizey;
	bool ishover = 0;
	function<void()>Buttonfunc;
	vector<GLubyte> colors;
	string text,textcolor;
	Buttonitem(float sizex, float sizey, float posx, float posy, function<void()> Buttonfunc, string text = "", string color = "#FFFFFF", string textcolor = "#000000") :sizex(sizex), sizey(sizey), posx(posx), posy(posy), Buttonfunc(Buttonfunc), text(text), textcolor(textcolor) {
		colors = hextorgb(color);
	}
	void hover() {
		if ((mousex >= posx - sizex / 2 && mousex <= posx + sizex / 2) && (mousey >= posy - sizey / 2 && mousey <= posy + sizey / 2)) {
			ishover = 1;
		}
		else ishover = 0;
	}
	void onClick() {
		if (left_click && ishover) {
			Buttonfunc();
		}
	}
	void draw() {
		glPushMatrix();
		glColor3ubv(colors.data());
		glTranslatef(posx, posy, 0);
		glBegin(GL_QUADS);
		glVertex2f(-sizex / 2, -sizey / 2);
		glVertex2f(-sizex / 2, sizey / 2);
		glVertex2f(sizex / 2, sizey / 2);
		glVertex2f(sizex / 2, -sizey / 2);
		glEnd();
		glPopMatrix();
		renderTextCenter(text, posx, posy,64, textcolor);
	}
};
class Bar {
public:
	float posx, posy, sizex, sizey, indicator_width, slidervalue, indicatorxpos;
	float minvalue, maxvalue;
	bool isslider,isdragging;
	Bar(float posx, float posy, float sizex, float sizey, float iwidth = 0, float value = 0, float min = 0, float max = 1) : posx(posx), posy(posy), sizex(sizex), sizey(sizey), indicator_width(iwidth), slidervalue(value), minvalue(min), maxvalue(max) {
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
		if (isslider) {
			if ((mousex >= posx - sizex / 2 && mousex <= posx + sizex / 2) && (mousey >= posy - sizey / 2 && mousey <= posy + sizey / 2) || isdragging)
			if (left_click) {
				cout << "dragging these balls\n";
				float leftEdge = posx - sizex / 2;
				float x_offset = mousex - leftEdge;
				float adjusted_offset = x_offset - indicator_width / 2;
				float normalized = adjusted_offset / (sizex - indicator_width);
				if (normalized < 0) normalized = 0;
				if (normalized > 1) normalized = 1;
				indicatorxpos = normalized * (sizex - indicator_width);
				slidervalue = minvalue + normalized * (maxvalue - minvalue);
				cout << slidervalue << endl;
				isdragging = 1;
			}
			else isdragging = 0;
		}
	}
};
Buttonitem retrybutton(400, 200, 0, 0, [&]() {p1.reset(); bullet_buffer.clear(); enemybuffer.clear(); dropsbuffer.clear(); left_click = 0; }, "FFFFFF");
Buttonitem startbutton(400, 200, 300, 300, [&]() {currentscreen = 1; left_click = 0; previousscreen = stack<int>(); }, "Play");
Buttonitem settingsbutton(400, 200, 0, 0, [&]() {previousscreen.push(currentscreen); currentscreen = 2; left_click = 0; }, "FFFFFF");
Buttonitem exitbutton(400, 200, -300, -300, [&]() {glutDestroyWindow(glutGetWindow()); exit(0); }, "FFFFFF");
Buttonitem backbutton(200, 200, -1050, 1050, [&]() {
	if (!previousscreen.empty()) {
		currentscreen = previousscreen.top();
		previousscreen.pop();
	}},"#FFFFFF");
Bar testbar(0, 0, 400, 100, 100, 1.1);