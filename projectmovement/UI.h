#pragma once
#include "entities.h"
extern float mousex, mousey;
extern bool left_click;

vector<GLubyte> hextorgb(string hex) {
	if (hex[0] == '#') {
		hex = hex.substr(1);
	}
	transform(hex.begin(), hex.end(), hex.begin(), ::toupper);
	map<char, int> hexadec = { {'0',0}, {'1',1},{'2',2},{'3',3},{'4',4},{'5',5},{'6',6},{'7',7},{'8',8},{'9',9},{'A',10},{'B',11},{'C',12},{'D',13},{'E',14},{'F',15} };
	GLubyte red = hexadec[hex[0]] * 16 + hexadec[hex[1]], green = hexadec[hex[2]] * 16+ hexadec[hex[3]], blue = hexadec[hex[4]] * 16 + hexadec[hex[5]];
	return vector<GLubyte>({ red,green,blue });
}
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
	Buttonitem(float sizex, float sizey, float posx, float posy, function<void()> Buttonfunc, string color = "#000000") :sizex(sizex), sizey(sizey), posx(posx), posy(posy), Buttonfunc(Buttonfunc) {
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
	}
};
Buttonitem retrybutton(400, 200, 0, 0, [&]() {p1.reset(); bullet_buffer.clear(); enemybuffer.clear(); },"00FFFF");