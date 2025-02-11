#pragma once
#include "entities.h"
class Textitem {
	string text;
	float fontsize,sizex,sizey;
	float posx=0, posy=0;
};
class Buttonitem {
	Textitem label;
	float posx, posy, sizex, sizey;
};