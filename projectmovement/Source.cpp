#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include<unordered_map>
#include <mutex>
#include "entities.h"
#include "UI.h"
#include "Text.h"
#include "Audio.h"
using namespace std;
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "freetype.lib")
const int frames = 240;
const double target = 1.0 / frames;
std::chrono::steady_clock::time_point last, last_spawn = std::chrono::steady_clock::now();
float mousey = 0, mousex = 0; int score = 0,currentscreen=0;
//w,a,s,d,space,e
bool keys[6] = { 0,0,0,0,0,0 };
stack<int> previousscreen;
float masterVolume=1, musicVolume=1, sfxVolume=1;
void InitGraphics(int argc, char* argv[]);
void SetTransformations();
void OnDisplay();
void mainmenu();
void game();
void settings();
void screenhandler();
void mouseclick(int, int, int, int);
void OnKey(unsigned char, int, int);
void OnKeyUp(unsigned char, int, int);
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
	glutSetCursor(GLUT_CURSOR_NONE);
	SetTransformations();
	glutPassiveMotionFunc(mousefunc);
	glutMotionFunc(mousefunc);
	glutMouseFunc(mouseclick);
	tex_init();
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
	srand(time(0));
	int sign = rand() % 2 *2 - 1;
	float xpos = (((rand() % 2400) * 2 - 2400)) - p1.posx*sign;
	float ypos = (((rand() % 2400) * 2 - 2400)) - p1.posy*sign;
	enemybuffer.push_back(enemy(xpos, ypos, 0,1));
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
	masterAudioBar.drag();
	masterAudioBar.draw();
	sfxBar.drag();
	sfxBar.draw();
	musicBar.drag();
	musicBar.draw();
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

	glTranslatef(-p1.posx + xdis, -p1.posy + ydis, 0.0f);

}
void resetgame() {
	p1.reset();
	enemybuffer.clear();
	bullet_buffer.clear();
	dropsbuffer.clear();
	score = 0;
	dead_enemy_buffer.clear();
	updatetitle();
}
void game() {
	if (p1.gameover) {
		glClearColor(0.5, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		retrybutton.draw();
		retrybutton.hover();
		retrybutton.onClick();
		glPushMatrix();
		if (keys[4]) {
			p1.reset();
			enemybuffer.clear();
			bullet_buffer.clear();
			dropsbuffer.clear();
			score = 0;
			dead_enemy_buffer.clear();
			updatetitle();
		}
		camera_hover();

		p1.draw();

		for (auto& i : enemybuffer)i.draw();

		for (auto& i : dead_enemy_buffer)i.diplay();

		for (auto& i : bullet_buffer)i.draw();
		
		glPopMatrix();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0, 0, 0, 0.5);
		glBegin(GL_QUADS);
		glVertex2f(-1200 - p1.posx, -1200 - p1.posy);
		glVertex2f(-1200 - p1.posx, 1200 - p1.posy);
		glVertex2f(1200 - p1.posx, 1200 - p1.posy);
		glVertex2f(1200 - p1.posx, -1200 - p1.posy);
		glEnd();
		glDisable(GL_BLEND);
	}
	else {
		cross.hover = false;
		last = std::chrono::steady_clock::now();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		updateDeltaTime();


		// Apply camera translation
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		int valu = 5 / (1 + score % 5);
		auto spawn_interval = chrono::seconds(2);
		if (last - last_spawn >= spawn_interval) {
			spawnEnemy();
			last_spawn = last;
		}
		p1.move();


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

			if (i.type == 1) {
				if (p1.collision(i)) {
					p1.rot = i.rot - 90;
					p1.gameover = 1;
					p1.dselected = rand() % 9;
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
			enemy.draw();
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
	}
	cross.draw();
	glutSwapBuffers();

}
void OnKey(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'w': keys[0] = 1; break;
	case 'a':keys[1] = 1; break;
	case 's':keys[2] = 1; break;
	case 'd':keys[3] = 1; break;
	case 32: keys[4] = 1; break;
	case 'e': keys[5] = 1; break;
	default:
		break;
	}
}
void OnKeyUp(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'w': keys[0] = 0; break;
	case 'a':keys[1] = 0; break;
	case 's':keys[2] = 0; break;
	case 'd':keys[3] = 0; break;
	case 32: keys[4] = 0; break;
	case 'e': keys[5] = 0; break;
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
void quitter() {
	quitmixer();
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	exit(0);
}