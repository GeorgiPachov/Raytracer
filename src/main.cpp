/***************************************************************************
 *   Copyright (C) 2009-2012 by Veselin Georgiev, Slavomir Kaslev et al    *
 *   admin@raytracing-bg.net                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <SDL/SDL.h>
#include "sdl.h"
#include "matrix.h"
#include "camera.h"
#include "geometry.h"
#include "shading.h"

Color vfb[VFB_MAX_SIZE][VFB_MAX_SIZE]; //!< virtual framebuffer
bool needsAA[VFB_MAX_SIZE][VFB_MAX_SIZE];
const float AA_THRESH = 0.1f;
Camera camera;

Geometry* geometries[100];
Shader* shaders[100];
Node* nodes[100];
Texture* textures[100];
int nGeom = 0, nShaders = 0, nNodes = 0, nTextures = 0;

/// traces a ray in the scene and returns the visible light that comes from that direction
Color raytrace(Ray ray)
{
	if (ray.debug) {
		printf("  Raytrace[start = ");
		ray.start.print();
		printf(", dir = ");
		ray.dir.print();
		printf("]\n");
	}
	IntersectionInfo closestInfo;
	closestInfo.distance = INF;
	Node* closestNode = NULL;
	for (int i = 0; i < nNodes; i++) {
		IntersectionInfo info;
		if (nodes[i]->geometry->intersect(ray, info)) {
			if (info.distance < closestInfo.distance) {
				closestInfo = info;
				closestNode = nodes[i];
			}
		}
	}
	if (!closestNode) return Color(0, 0, 0);
	else {
		if (ray.debug) {
			printf("    Closest node is a %s at distance %.3lf\n", closestNode->geometry->name(), closestInfo.distance);
			printf("      ip   = "); closestInfo.ip.println();
			printf("      norm = "); closestInfo.norm.println();
		}
		return closestNode->shader->shade(ray, closestInfo);
	}
}

static bool tooDifferent(Color a, Color b)
{
	float diff = fabs(a.r - b.r) + fabs(a.g - b.g) + fabs(a.b - b.b);
	return (diff > AA_THRESH);
}

void renderScene(void)
{
	const double offsets[5][2] = {
		{ 0, 0 }, {0.3, 0.3}, {0.6, 0}, {0, 0.6}, {0.6, 0.6}
	};

	//trace rays
	for (int y = 0; y < frameHeight(); y++){
		for (int x = 0; x < frameWidth(); x++) {
			Ray ray = camera.getScreenRay(x, y);
			vfb[y][x] = raytrace(ray);
		}
	}

	//check for AA
	for (int y = 0; y < frameHeight(); y++) {
		for (int x = 0; x < frameWidth(); x++) {
			Color neighs[4];
			neighs[0] = y > 0 ? vfb[y-1][x] : vfb[y][x];
			neighs[1] = y < frameHeight() - 1 ? vfb[y+1][x] : vfb[y][x];
			neighs[2] = x > 0 ? vfb[y][x-1] : vfb[y][x];
			neighs[3] = x < frameWidth() - 1 ? vfb[y][x+1] : vfb[y][x];
			Color average = (vfb[y][x] + neighs[0] + neighs[1] + neighs[2] + neighs[3]) / 5;
			for (int i = 0; i < 4; i++) {
				if (tooDifferent(neighs[i], average)) needsAA[y][x] = true;
			}
		}
	}

	//draw AA
	for (int y = 0; y < frameHeight(); y++)
		for (int x = 0; x < frameWidth(); x++) {
			if (needsAA[y][x]) {
				Color accum = Color(0, 0, 0);
				for (int samples = 0; samples < 5; samples++) {
					Ray ray = camera.getScreenRay(x + offsets[samples][0], y + offsets[samples][1]);
					accum += raytrace(ray);
				}
				vfb[y][x] = accum / 5;
			}
		}
}

/// checks if light (situated at point l) is visible at point p. This works
/// by tracing a ray along the two points and testing whether it is unobstructed.
bool lightIsVisible(Vector p, Vector l)
{
	Vector LP = p - l;
	double len = LP.length();
	Ray ray;
	ray.start = l;
	ray.dir = LP;
	ray.dir.normalize(); // save the length of the LP
	for (int i = 0; i < nNodes; i++) {
		IntersectionInfo info;
		if (nodes[i]->geometry->intersect(ray, info)
		    && info.distance < len - 1e-6) return false; // a hit point was found and it's closer
		                                                 // to the light than length(LP); we're in shadow.
	}
	return true;
}

/// generates a scene directly, using hardcoded coordinates
void generateScene(void)
{
	geometries[0] = new Plane(0);
	nGeom = 1;
	shaders[0] = new Lambert(Color(0, 0.9f, 0));
	nShaders = 1;
	nodes[0] = new Node(geometries[0], shaders[0]);
	nNodes = 1;
	camera.pos = Vector(-10, 100, 0);
	camera.aspect = 4.0/3.0;
	camera.yaw = -10;
	camera.pitch = -25;
	camera.roll = 0;
	camera.fov = 90;
	camera.beginRender();

	// lightPos and lightIntensity are defined in shading.cpp
	extern Vector lightPos;
	extern Color lightIntensity;
	lightPos = Vector(0, 1000, 1600);
	lightIntensity = Color(10000, 10000, 10000) * 150;
}


/// automatically frees all scene resources
void freeScene(void)
{
	for (int i = 0; i < nNodes; i++) delete nodes[i];
	for (int i = 0; i < nShaders; i++) delete shaders[i];
	for (int i = 0; i < nTextures; i++) delete textures[i];
	for (int i = 0; i < nGeom; i++) delete geometries[i];
}

void handleMouse(SDL_MouseButtonEvent *mev)
{
	printf("Mouse click from %d %d\n", (int) mev->x, (int) mev->y);
	Ray ray = camera.getScreenRay(mev->x, mev->y);
	ray.debug = true;
	raytrace(ray);
	printf("Raytracing completed!\n");
}

int main(int argc, char** argv)
{
	if (!initGraphics(RESX, RESY)) return -1;
	generateScene();
	Uint32 ticks = SDL_GetTicks();
	renderScene();
	Uint32 diff = SDL_GetTicks() - ticks;
	printf("Render time: %0.2lf seconds\n", diff / 1000.0);
	displayVFB(vfb);
	waitForUserExit();
	freeScene();
	closeGraphics();
	return 0;
}
