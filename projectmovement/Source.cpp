#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include<string>
#include<unordered_map>
#include <mutex>
#include "entities.h"
#include "UI.h"
#include "Text.h"
#include "Audio.h"
#include "background.h"
using namespace std;
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2_mixer.lib")
#pragma comment(lib, "freetype.lib")
const int frames = 240;
const double target = 1.0 / frames;
std::chrono::steady_clock::time_point last, last_spawn = std::chrono::steady_clock::now();
float mousey = 0, mousex = 0; 
int score = 0,currentscreen=0, enemycount=0,wave=1;
int enemyfull = enemycount;
float slowmoframes = 1000,slowmomax=1000;
//w,a,s,d,space,e
bool keys[6] = { 0,0,0,0,0,0 },ispaused=0;
stack<int> previousscreen;
float masterVolume=1, musicVolume=1, sfxVolume=1;
void InitGraphics(int argc, char* argv[]);
void SetTransformations();
void OnDisplay();
void mainmenu();
void wavehandler();
int wavetransition();
void game();
void settings();
void cheatscreen();
void screenhandler();
void mouseclick(int, int, int, int);
void OnKey(unsigned char, int, int);
void OnKeyUp(unsigned char, int, int);
void OnSpecial(int, int, int);
void OnSpecialUp(int, int, int);
void mousefunc(int, int);
void quitter();
////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
	srand(time(0));

	if (!initFreeType()) {
		return -1;
	}
	initmixer();
	soundsloader();
	playMusic(0);
	InitGraphics(argc, argv);
	quitter();
	return 0;
}

void updateDeltaTime() {
	auto currentTime = std::chrono::steady_clock::now();
	auto elapsed = currentTime - last;
	if (elapsed < std::chrono::duration<double>(target)) {
		auto remaining = std::chrono::duration<double>(target) - elapsed;
		auto sleepDuration = remaining * 0.9;
		std::this_thread::sleep_for(sleepDuration);
		while (std::chrono::steady_clock::now() - last < std::chrono::duration<double>(target)) {
		}
	}
	last = std::chrono::steady_clock::now();
}
/**
Creates the main window, registers event handlers, and
initializes OpenGL stuff.
*/
void InitGraphics(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	//Create an 800x600 window with its top-left corner at pixel (100, 100)
	glutInitWindowPosition(100, 100); //pass (-1, -1) for Window-Manager defaults
	glutInitWindowSize(600, 600);
	glutCreateWindow("Prototype No.Ilostcount | Score: ");
	//OnDisplay will handle the paint event
	glutDisplayFunc(OnDisplay);
	// here is the setting of the idle function
	glutIdleFunc(screenhandler);
	// here is the setting of the key function
	glutKeyboardFunc(OnKey);
	glutKeyboardUpFunc(OnKeyUp);
	glutSpecialFunc(OnSpecial);
	glutSpecialUpFunc(OnSpecialUp);
	glutSetCursor(GLUT_CURSOR_NONE);
	SetTransformations();
	glutPassiveMotionFunc(mousefunc);
	glutMotionFunc(mousefunc);
	glutMouseFunc(mouseclick);
	tex_init();
	retry_init();
	init_bg();
	glutMainLoop();
}
/**
Sets the logical coordinate system we will use to specify
our drawings.
*/
void SetTransformations() {
	//set up the logical coordinate system of the window: [-100, 100] x [-100, 100]
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1200, 1200, -1200, 1200);
	glMatrixMode(GL_MODELVIEW);
}

void OnDisplay() {
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	p1.draw();
	glutSwapBuffers();
}
void spawnEnemy() {
	int weapontype = rand() % 101;
	if (weapontype <= 70) weapontype = 1;
	else if (weapontype <= 85) weapontype = 2;
	else if (weapontype <= 95) weapontype = 3;
	else if (weapontype <= 100) weapontype = 4;
	float angle = (float(rand()%101)/100.f)*2*PI;
	float distance = 1200+(float(rand() % 101) / 100.f)*1200;
	float xpos = p1.posx + cos(angle) * distance;
	float ypos = p1.posy + sin(angle) * distance;
	enemybuffer.push_back(enemy(xpos, ypos, 0,weapontype));
}
void updatetitle() {
	
	char buffer[100] = { 0 };
	string str;
	_itoa_s(score, buffer, 10);
	str = "Prototype No.Ilostcount | Score: ";
	str.append(buffer);
	glutSetWindowTitle(str.c_str());
}
void screenhandler() {
	switch (currentscreen) {
	case 0:
		mainmenu();
		break;
	case 1:
		game();
		break;
	case 2:
		settings();
		break;
	case 3:
		cheatscreen();
		break;
	default:
		break;
	}
}
void mainmenu() {
	glClearColor(0.1, 0.1, 0.11,1);
	glClear(GL_COLOR_BUFFER_BIT);
	startbutton.draw();
	startbutton.hover();
	startbutton.onClick();
	settingsbutton.draw();
	settingsbutton.hover();
	settingsbutton.onClick();
	exitbutton.draw();
	exitbutton.hover();
	exitbutton.onClick();
	cross.draw();
	glutSwapBuffers();
}
void settings() {
	glClearColor(0.1, 0.1, 0.11, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	backbutton.draw();
	backbutton.hover();
	backbutton.onClick();
	renderCenteredText("Master Volume:", -580, 370, 64, "#FFFFFF");
	masterAudioBar.drag();
	masterAudioBar.draw();
	renderCenteredText("Sfx Volume:", -500, 220, 64, "#FFFFFF");
	sfxBar.drag();
	sfxBar.draw();
	renderCenteredText("Music Volume:", -530, 70, 64, "#FFFFFF");
	musicBar.drag();
	musicBar.draw();
	renderCenteredText(to_string(int(masterVolume * 100)) + "%", 550, 370, 64, "#FFFFFF");
	renderCenteredText(to_string(int(sfxVolume * 100)) + "%", 550, 220, 64, "#FFFFFF");
	renderCenteredText(to_string(int(musicVolume * 100)) + "%", 550, 70, 64, "#FFFFFF");
	cheatsbutton.draw();
	cheatsbutton.hover();
	cheatsbutton.onClick();
	cross.draw();
	glutSwapBuffers();
}
float xdir = 0.2, ydir = 0.2, xdis = 0, ydis = 0;

void camera_hover() {

	//cout << xdir << " " << ydir<<"\n";

	if (abs(xdis) > 450)xdir = -xdir;

	if (abs(ydis) > 350)ydir = -ydir;

	xdis += xdir;

	ydis += ydir;



}
void resetgame() {
	p1.reset();
	enemybuffer.clear();
	bullet_buffer.clear();
	dropsbuffer.clear();
	score = 0;
	dead_enemy_buffer.clear();
	enemycount = enemyfull;
	updatetitle();
	xdir = 0.2, ydir = 0.2, xdis = 0, ydis = 0;
}
void paused() {
	glPushMatrix();
	camera_hover();
	glTranslatef(-p1.posx + xdis, -p1.posy + ydis, 0.0f);
	p1.draw();
	for (auto& i : enemybuffer)i.draw();

	for (auto& i : dead_enemy_buffer)i.diplay();

	for (auto& i : bullet_buffer)i.draw();

	glPopMatrix();
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(0, 0, 0, 0.5);

	glBegin(GL_QUADS);
	glVertex2f(-1200, -1200);
	glVertex2f(-1200, 1200);
	glVertex2f(1200, 1200);
	glVertex2f(1200, -1200);
	glEnd();

	glDisable(GL_BLEND);
}
void game() {
	if (p1.gameover) {
		glClearColor(0.5, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		background(p1.posx - xdis, p1.posy - ydis);
		paused();
		retrybutton.draw();
		retrybutton.hover();
		retrybutton.onClick();
		if (keys[4]) {
			resetgame();
		}
		renderCenteredText("WASTED", 0, 20, 128, "94241c");

	}
	else if(!ispaused){
		if(cross.hover) cross.hover = false;
		last = std::chrono::steady_clock::now();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		updateDeltaTime();


		// Apply camera translation
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		int valu = 5 / (1 + score % 5);
	
		p1.move();
		background(p1.posx, p1.posy);
		wavehandler();
		int impact_x = 0, impact_y = 0;
		//mimic camera by moving everything else
		if (impact>0) {
			impact_x = impact * ((rand() % 2)==0? 1: -1) * (rand() % 3);
			impact_y = impact * ((rand() % 2) == 0 ? 1 : -1) * (rand() % 3);
			impact--;
		}
		glTranslatef(-p1.posx+impact_x, -p1.posy+impact_y, 0.0f);
		//p1

		p1.shoot();
		//bullets
		vector<bullet> next;
	for (auto &i : bullet_buffer) {
		i.move();
		i.draw();
		if (!i.unneeded(p1)) {
			next.push_back(i);

			if (i.type == 1 && !invulnerability) {
				if (p1.collision(i)) {
					p1.rot = i.rot - 90;
					p1.gameover = 1;
					dselected = rand() % 9;
				}
				continue;
			}
			for (int j = 0; j < enemybuffer.size(); j++) {
				if (i.type == 0 && enemybuffer[j].alive) {
					if (enemybuffer[j].collision(i)) {
						enemybuffer[j].alive = false;
						score++;
						enemybuffer[j].drop();
						if (!next.empty()) {
							next.pop_back();
						}
						updatetitle();
						continue;
					}
				}
			}
		}
	}
	
	bullet_buffer.clear();
	bullet_buffer = next;
	
	vector<enemy> enemy_next;
		for (auto& enemy : enemybuffer) {
			if (enemy.alive) {
				enemy.move();
				enemy.shoot();
				enemy_next.push_back(enemy);
			}
			else {

				dead_enemy_buffer.push_back(dead_enemy(enemy.posx, enemy.posy, enemy.rot));
			}
		}
			enemybuffer.clear();
			enemybuffer = enemy_next;
			for (auto& i : dead_enemy_buffer)i.diplay();
			for (auto& i : enemybuffer)i.draw();
		p1.pickup();
		for (auto& i : dropsbuffer) {
			i.move();
			i.draw();
		}
		p1.draw();
		glPopMatrix();
		if(!nocooldown)
		cooldownBar.specialdraw(1.f - p1.pweapon.cooldown / p1.pweapon.basecooldown, 0.192, 0.208, 0.231);
		if(!infslowmo)
		slowmoBar.specialdraw(static_cast<float>(slowmoframes/slowmomax), 0.145, 0.51, 0.871);
	}
	else if(ispaused){
		
		background(p1.posx, p1.posy);
		paused();
		settingsbutton.draw();
		settingsbutton.hover();
		settingsbutton.onClick();
		renderCenteredText(" Paused..", 0, 20, 128, "#FFFFFF");
	}
	cross.draw();
	glutSwapBuffers();

}
void OnKey(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'W':
	case 'w': keys[0] = 1; break;
	case 'A':
	case 'a':keys[1] = 1; break;
	case 'S':
	case 's':keys[2] = 1; break;
	case 'D':
	case 'd':keys[3] = 1; break;
	case 32: keys[4] = 1; break;
	case 'E':
	case 'e': keys[5] = 1; break;

	default:
		break;
	}
}
void OnKeyUp(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'W':
	case 'w': keys[0] = 0; break;
	case 'A':
	case 'a':keys[1] = 0; break;
	case 'S':
	case 's':keys[2] = 0; break;
	case 'D':
	case 'd':keys[3] = 0; break;
	case 32: keys[4] = 0; break;
	case 'E':
	case 'e': keys[5] = 0; break;
	case 27: if (currentscreen == 1)ispaused = !ispaused; else if (!previousscreen.empty()) { currentscreen = previousscreen.top(); previousscreen.pop(); } break;
	default:
		break;
	}
}
void OnSpecial(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_UP: keys[0] = 1; break;
	case GLUT_KEY_LEFT:keys[1] = 1; break;
	case GLUT_KEY_DOWN:keys[2] = 1; break;
	case GLUT_KEY_RIGHT:keys[3] = 1; break;
	default:
		break;
	}
}
void OnSpecialUp( int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_UP: keys[0] = 0; break;
	case GLUT_KEY_LEFT:keys[1] = 0; break;
	case GLUT_KEY_DOWN:keys[2] = 0; break;
	case GLUT_KEY_RIGHT:keys[3] = 0; break;
	default:
		break;
	}
}
void mousefunc(int x, int y) {
	float nx = x / 600.0f, ny = y / 600.0f;
	mousex = ((nx * 2400) - 1200);
	mousey = 1200 - (ny * 2400);
}
void mouseclick(int button, int state, int x, int y) {
	float nx = x / 600.0f, ny = y / 600.0f;
	mousex = ((nx * 2400) - 1200);
	mousey = 1200 - (ny * 2400);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		left_click = 1;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		left_click = 0;
	}
}

void wavehandler() {
	if (enemycount != 0) {
		auto spawn_interval = chrono::seconds(2);
		if (last - last_spawn >= spawn_interval) {
			for (int i = 0; i < (wave / 10)+1; i++) {
				spawnEnemy();
			}
			last_spawn = last;
			enemycount--;
		}
	}
	else {
		if (dead_enemy_buffer.size() == enemyfull) {
			if (wavetransition()) {
				enemycount = 5 + (5 * log(wave + 1));
				enemyfull = enemycount;
				bullet_buffer.clear();
				enemybuffer.clear();
				dead_enemy_buffer.clear();
				dropsbuffer.clear();
				wave++;
			}
		}
	}
}
int textpos=-1500,animframes=0;
int wavetransition() {
	string s = "Wave " + to_string(wave);
	renderTextCenter(s, textpos, 600, 64, "#FFFFFF");
	if (textpos < 0) {
		textpos += 10;
	}
	else {
		if (textpos < 1200 && animframes < 100) {
			animframes++;
		}
		else {
			textpos++;
		}
		if (textpos >= 300) {
			textpos += 10;
		}
	}
	if (textpos > 1500) {
		textpos = -1500;
		return 1;
	}
	return 0;

}
void quitter() {
	quitmixer();
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	exit(0);
}
void cheatscreen() {
	glClearColor(0.1, 0.1, 0.11, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	renderCenteredText(" Game too hard?", 0, 1000, 80, "#FFFFFF");
	backbutton.draw();
	backbutton.hover();
	backbutton.onClick();
	invulnerabilitybutton.draw();
	invulnerabilitybutton.hover();
	invulnerabilitybutton.onClick();
	infslowmobutton.draw();
	infslowmobutton.hover();
	infslowmobutton.onClick();
	nocooldownbutton.draw();
	nocooldownbutton.hover();
	nocooldownbutton.onClick();
	weapon5button.draw();
	weapon5button.hover();
	weapon5button.onClick();

	cross.draw();
	glutSwapBuffers();
}