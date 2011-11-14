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

#include <assert.h>
#include "geometry.h"
#include "util.h"
#include <algorithm>
#include <stdio.h>
using std::sort;


bool Plane::intersect(Ray ray, IntersectionInfo& info)
{
	// intersect a ray with a XZ plane:
	// if the ray is (almost) parallel to the XZ plane, consider no intersection:
	if (fabs(ray.dir.y) < 1e-9) return false;
	
	// calculate how much distance to the target height we need to cover from the start
	double toCover = this->y - ray.start.y;
	// calculate how much we should scale ray.dir so that we reach the target height (this may be negative) ...
	double scaling = toCover / ray.dir.y;
	if (scaling < 0) return false; // ... and if it is, then the intersection point is behind us; bail out
	
	// calculate the intersection
	info.ip = ray.start + ray.dir * scaling;
	info.distance = scaling;
	info.norm = Vector(0, 1, 0);
	info.u = info.ip.x;
	info.v = info.ip.z;
	info.g = this;
	return true;
}


bool Sphere::intersect(Ray ray, IntersectionInfo& info)
{
	// compute the sphere intersection using a quadratic equation:
	Vector H = ray.start - O;
	double A = ray.dir.lengthSqr();
	double B = 2 * dot(H, ray.dir);
	double C = H.lengthSqr() - R*R;
	double Dscr = B*B - 4*A*C;
	if (Dscr < 0) return false; // no solutions to the quadratic equation - then we don't have an intersection.
	double x1, x2;
	x1 = (-B + sqrt(Dscr)) / (2*A);
	x2 = (-B - sqrt(Dscr)) / (2*A);
	double sol = x2; // get the closer of the two solutions...
	if (sol < 0) sol = x1; // ... but if it's behind us, opt for the other one
	if (sol < 0) return false; // ... still behind? Then the whole sphere is behind us - no intersection.
	
	info.distance = sol;
	info.ip = ray.start + ray.dir * sol;
	info.norm = info.ip - O; // generate the normal by getting the direction from the center to the ip
	info.norm.normalize();
	info.g = this;
	info.u = (PI + atan2(info.ip.z - O.z, info.ip.x - O.x))/(2*PI);
	info.v = 1.0 - (PI/2 + asin((info.ip.y - O.y)/R)) / PI;
	return true;
}

static void testIntersect(Ray ray, IntersectionInfo& info, Vector faceCenter, double c3, double start, double dir, Vector normal, double side)
{
	if (fabs(dot(ray.dir, normal)) < 1e-9) return;
	double toCover = c3 - start;
	double scaling = toCover / dir;
	if (scaling < 0) return;
	Vector ip = ray.start + ray.dir * scaling;
	double distanceFromCenter = fabs(faceCenter.x - ip.x);
	distanceFromCenter = max(distanceFromCenter, fabs(faceCenter.y - ip.y));
	distanceFromCenter = max(distanceFromCenter, fabs(faceCenter.z - ip.z));
	if (distanceFromCenter > side/2) return;
	if (scaling < info.distance) {
		info.distance = scaling;
		info.ip = ip;
		info.norm = normal;
		info.u = ip.x + ip.y;
		info.v = ip.z;
	}
}

bool Cube::intersect(Ray ray, IntersectionInfo& info)
{
	IntersectionInfo closest;
	closest.distance = INF;
	
	testIntersect(ray, closest, Vector(O.x - side/2, O.y, O.z), O.x - side/2, ray.start.x, ray.dir.x, Vector(-1, 0, 0), side);
	testIntersect(ray, closest, Vector(O.x + side/2, O.y, O.z), O.x + side/2, ray.start.x, ray.dir.x, Vector(+1, 0, 0), side);
	
	testIntersect(ray, closest, Vector(O.x, O.y - side/2, O.z), O.y - side/2, ray.start.y, ray.dir.y, Vector(0, -1, 0), side);
	testIntersect(ray, closest, Vector(O.x, O.y + side/2, O.z), O.y + side/2, ray.start.y, ray.dir.y, Vector(0, +1, 0), side);
	
	testIntersect(ray, closest, Vector(O.x, O.y, O.z - side/2), O.z - side/2, ray.start.z, ray.dir.z, Vector(0, 0, -1), side);
	testIntersect(ray, closest, Vector(O.x, O.y, O.z + side/2), O.z + side/2, ray.start.z, ray.dir.z, Vector(0, 0, +1), side);
	
	if (closest.distance < INF) {
		info = closest;
		info.g = this;
		return true;
	} else return false;
}
 
int CsgOp::findAllIntersections(Ray ray, Geometry* geom, IntersectionInfo infos[])
{
	int c = 0;
	double totalLength = 0;
	while (true) {
		bool ok = geom->intersect(ray, infos[c]);
		if (!ok) break;
		double l = infos[c].distance;
		infos[c].distance += totalLength;
		totalLength += l + 1e-6;
		Ray newRay;
		newRay.start = infos[c].ip + ray.dir * 1e-6;
		newRay.dir = ray.dir;
		ray = newRay;
		c++;
		assert(c < MAX_INTERSECTIONS);
	}
	return c;
}


bool CsgOp::intersect(Ray ray, IntersectionInfo& ret)
{
	IntersectionInfo li[MAX_INTERSECTIONS], ri[MAX_INTERSECTIONS];
	IntersectionInfo all[MAX_INTERSECTIONS];
	int nR = 0, nL = 0;
	//
	nL = findAllIntersections(ray, left, li);
	nR = findAllIntersections(ray, right, ri);
	bool insideL = nL % 2 != 0;
	bool insideR = nR % 2 != 0;
	for (int i = 0; i < nL; i++) all[i] = li[i];
	for (int i = 0; i < nR; i++) all[i+nL] = ri[i];
	int n = nL + nR;
	sort(all, all + n);
	
	for (int i = 0; i < n; i++) {
		IntersectionInfo& info = all[i];
		if (info.g == left) insideL = !insideL;
		else insideR = !insideR;
		if (boolOp(insideL, insideR)) {
			ret = info;
			ret.g = this;
			return true;
		}
	}
	
	return false;
}










