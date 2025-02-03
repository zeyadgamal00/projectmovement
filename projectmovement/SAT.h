#ifndef SAT_H
#define SAT_H
#pragma once
// SAT algorithm for collision detection
#include <vector>
#include <cmath>
#include <set>
using namespace std;
class vect
{
public:
	float x, y;
	vect(float x = 0, float y = 0) :x(x), y(y) {}
	vect(float *arr) :x(arr[0]), y(arr[1]) {}
	vect operator-(const vect& v2) {
		return vect(x - v2.x, y - v2.y);
	}
	float dot(const vect& v2) {
		return x * v2.x + y * v2.y;
	}
	vect perp() {
		return vect(-y, x);
	}
	vect oppostite() {
		return vect(-x, -y);
	}
	vect normalize() {
		float i = sqrt(x * x + y * y);
		return vect(x / i, y / i);
	}
};
class shape {
public:
	vector<vect> vertices;
	shape(vector<vect>& vertices) : vertices(vertices) {}
	pair<float, float> project(vect& axis) {

		float min = axis.dot(vertices[0]);
		float max = min;
		for (const auto& vertex : vertices) {
			float projection = axis.dot(vertex);
			if (projection < min) min = projection;
			if (projection > max) max = projection;
		}
		return{ min,max };
	}
	bool check(shape& s2) {
		set<pair<float, float>> prev_axis;
		for (int i = 0; i < vertices.size(); ++i) {
			vect edge1 = vertices[(i + 1) % vertices.size()] - vertices[i];
			vect axis1 = edge1.perp();
			pair<float, float> value = { axis1.x,axis1.y };
			pair<float, float> op = { -axis1.x, -axis1.y };
			if (!prev_axis.count(value) && !prev_axis.count(op)) {
				prev_axis.insert(value);
				pair<float, float> projection1 = project(axis1);
				pair<float, float> projection2 = s2.project(axis1);

				if (projection1.second < projection2.first || projection2.second < projection1.first)
					return false;
			}
		}
		for (int i = 0; i < s2.vertices.size(); ++i) {
			vect edge2 = s2.vertices[(i + 1) % s2.vertices.size()] - s2.vertices[i];
			vect axis2 = edge2.perp();
			pair<float, float> value = { axis2.x,axis2.y };
			pair<float, float> op = { -axis2.x, -axis2.y };
			if (!prev_axis.count(value) && !prev_axis.count(op)) {
				prev_axis.insert(value);
				pair<float, float> projection1 = project(axis2);
				pair<float, float> projection2 = s2.project(axis2);

				if (projection1.second < projection2.first || projection2.second < projection1.first)
					return false;
			}
		}
		return true;
	}
};
#endif
