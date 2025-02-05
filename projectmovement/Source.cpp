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
using namespace std;
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")
const int frames = 240;
const double target = 1.0 / frames;
std::chrono::steady_clock::time_point last, last_spawn = std::chrono::steady_clock::now();
Wall wall;
float mousey = 0, mousex = 0; int score = 0;
//w,a,s,d,space
bool keys[5] = { 0,0,0,0,0 };
//Collision using SAT so sadly we need to implement vector algebra D:

void InitGraphics(int argc, char* argv[]);
void SetTransformations();
void OnDisplay();
void game();
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
	glutIdleFunc(game);
	// here is the setting of the key function
	glutKeyboardFunc(OnKey);
	glutKeyboardUpFunc(OnKeyUp);
	glutSetCursor(GLUT_CURSOR_NONE);
	SetTransformations();
	glutPassiveMotionFunc(mousefunc);
	glutMotionFunc(mousefunc);
	glutMouseFunc(mouseclick);
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
	enemy3.push_back(enemy_whooshwhoosh(xpos, ypos, 0));
}
void updatetitle() {
	score++;
	char buffer[100] = { 0 };
	string str;
	_itoa_s(score, buffer, 10);
	str = "Prototype No.Ilostcount | Score: ";
	str.append(buffer);
	glutSetWindowTitle(str.c_str());
}
void collision_check(vector<bullet>& bullet_buffer) {
	vector<bullet> next;
	for (auto i : bullet_buffer) {
		i.move();
		i.draw();
		if (i.type == 1) {
			if (p1.collision(i))
				p1.gameover = 1;
			continue;
		}
		if (i.type == 0 && enemy1.alive) {
			if (enemy1.collision(i)) {
				enemy1.alive = false;
				updatetitle();
				continue;
			}
		}
		for (int j = 0; j < enemy3.size(); j++) {
			if (i.type == 0 && enemy3[j].alive) {
				if (enemy3[j].collision(i)) {
					enemy3[j].alive = false;
					updatetitle();
					continue;
				}
			}
		}
		if (!i.unneeded(p1)) {
			next.push_back(i);
		}
	}
	bullet_buffer.clear();
	bullet_buffer = next;
}
void threadedcollisions() {
	const unsigned int number = thread::hardware_concurrency();
	const unsigned int chunksize = bullet_buffer.size() / number;
	vector<thread> threads;
	std::vector<std::vector<bullet>> result_buffers(number);

	for (int t = 0; t < number; t++) {
		unsigned int start = t * chunksize;
		unsigned int end = (t == number - 1) ? bullet_buffer.size() : (t + 1) * chunksize;
		threads.push_back(thread([start, end]() {
			vector<bullet> subvector(bullet_buffer.begin() + start, bullet_buffer.begin() + end);
		collision_check(subvector);
			
			}));
	}
	for (auto& t : threads) {
		t.join();
	}
}
void game() {
	if (p1.gameover) {
		glClearColor(0.5, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	else {

		last = std::chrono::steady_clock::now();
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		updateDeltaTime();


		// Apply camera translation
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		int valu = 5 / (1 + score % 5);
		auto spawn_interval = chrono::seconds(2);
		if (last - last_spawn >= spawn_interval) {
			spawnEnemy();
			last_spawn = last;
		}
		p1.move();
		cross.draw();
		//mimic camera by moving everything else
		glTranslatef(-p1.posx, -p1.posy, 0.0f);
		//p1

		p1.shoot();
		//bullets
		vector<bullet> next;
	for (auto i : bullet_buffer) {
		i.move();
		i.draw();
		if (!i.unneeded(p1)) {
			next.push_back(i);

			if (i.type == 1) {
				if (p1.collision(i))
					p1.gameover = 1;
				continue;
			}
			if (i.type == 0 && enemy1.alive) {
				if (enemy1.collision(i)) {
					enemy1.alive = false;
					updatetitle();
					continue;
				}
			}
			for (int j = 0; j < enemy3.size(); j++) {
				if (i.type == 0 && enemy3[j].alive) {
					if (enemy3[j].collision(i)) {
						enemy3[j].alive = false;
						updatetitle();
						continue;
					}
				}
			}
		}
	}
	bullet_buffer.clear();
	bullet_buffer = next;

		p1.draw();
		if (p1.collision(enemy2) && enemy2.alive) {
			p1.gameover = 1;
		}
		enemy1.draw();
		if (enemy1.alive) {
			enemy1.move();
			//enemy1.shoot();
		}
		for (int i = 0; i < enemy3.size(); i++) {
			enemy3[i].draw();
			enemy3[i].shoot();
			enemy3[i].drawbat();
			if (enemy3[i].alive) {
				enemy3[i].move();
				/*if (p1.collision(enemy3[i])) {
					p1.gameover = 1;
				}*/

			}
		}
	}
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
