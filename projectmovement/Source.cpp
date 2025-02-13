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
using namespace std;
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")
const int frames = 240;
const double target = 1.0 / frames;
std::chrono::steady_clock::time_point last, last_spawn = std::chrono::steady_clock::now();
float mousey = 0, mousex = 0; int score = 0,currentscreen=0;
//w,a,s,d,space
bool keys[6] = { 0,0,0,0,0,0 };
//Collision using SAT so sadly we need to implement vector algebra D:

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
int main(int argc, char* argv[]) {
	srand(time(0));
	InitGraphics(argc, argv);
	return 0;
}
void updateDeltaTime() {
	auto currentTime = std::chrono::steady_clock::now();
	chrono::duration<double> elapsed = currentTime - last;
	if (elapsed.count() < target) {
		this_thread::sleep_for(std::chrono::duration<double>(target - elapsed.count()));
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
	left_click = 0;
}
void settings() {
	currentscreen = 1;
	cout << "setting'd" << endl;
}
void game() {
	if (p1.gameover) {
		glClearColor(0.5, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		retrybutton.draw();
		retrybutton.hover();
		retrybutton.onClick();
		if (keys[4]) {
			p1.reset();
			enemybuffer.clear();
			bullet_buffer.clear();
			dropsbuffer.clear();
			score = 0;
			updatetitle();
		}
		dead_enemy_buffer.clear();
	}
	else {

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
				if (p1.collision(i))
					p1.gameover = 1;
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
