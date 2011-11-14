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
#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__


#include "vector.h"

/// a structure, that holds all the info, which a Geometry::intersect() method
/// may need to save when an intersection is found.
class Geometry;
struct IntersectionInfo {
	Vector ip; //!< intersection point in the world-space
	double distance; //!< the distance to the intersection point along the ray
	Vector norm; //!< the normal of the geometry at the intersection point
	double u, v; //!< 2D UV coordinates for texturing, etc.
	Geometry* g;
};

inline bool operator < (const IntersectionInfo& a, const IntersectionInfo& b) 
{
	return a.distance < b.distance;
}

/// An abstract class, that describes a geometry in the scene.
class Geometry {
public:
	virtual ~Geometry() {}
	
	/// Intersect a geometry with a ray. Returns true if an intersection is found,
	/// in which case the info structure is filled with details about the intersection.
	virtual bool intersect(Ray ray, IntersectionInfo& info) = 0;
	
	virtual const char* name() const = 0; //!< a virtual function, which returns the name of a geometry
};

class Shader;

/// A Node, which holds a geometry, linked to a shader.
class Node {
public:
	Node(Geometry* g, Shader* s) { geometry = g; shader = s; }
	Geometry* geometry;
	Shader* shader;
};

/// A simple plane, parallel to the XZ plane (coinciding with XZ when y == 0)
class Plane: public Geometry {
	double y; /// the offset of the plane from the origin along the Y axis.
public:
	Plane(double _y) { y = _y; }
	bool intersect(Ray ray, IntersectionInfo& info);
	const char* name() const { return "Plane"; }
};

/// A sphere
class Sphere: public Geometry {
	Vector O; /// center of the sphere
	double R; /// the sphere's radius
public:
	Sphere(Vector _O, double _R) {O = _O; R = _R; }
	bool intersect(Ray ray, IntersectionInfo& info);
	const char* name() const { return "Sphere"; }
};

class Cube: public Geometry {
	Vector O; /// center of the cube
	double side; /// the cube's side
public:
	Cube(Vector _O, double _side) {O = _O; side = _side; }
	bool intersect(Ray ray, IntersectionInfo& info);
	const char* name() const { return "Cube"; }
};

class CsgOp: public Geometry {
	Geometry* left, *right;
	static const int MAX_INTERSECTIONS = 32;
	int findAllIntersections(Ray ray, Geometry* geom, IntersectionInfo infos[]);
public:
	CsgOp(Geometry* l, Geometry *r) { left = l; right = r; }
	virtual bool boolOp(bool insideL, bool insideR) = 0;
	bool intersect(Ray ray, IntersectionInfo& info);
};

class CsgUnion: public CsgOp {
public:
	CsgUnion(Geometry* l, Geometry *r): CsgOp(l, r) {}
	bool boolOp(bool insideL, bool insideR) { return insideL || insideR; }
	const char* name() const { return "CsgUnion"; }
};

class CsgInter: public CsgOp {
public:
	CsgInter(Geometry* l, Geometry *r): CsgOp(l, r) {}
	bool boolOp(bool insideL, bool insideR) { return insideL && insideR; }
	const char* name() const { return "CsgInter"; }
};

class CsgDiff: public CsgOp {
public:
	CsgDiff(Geometry* l, Geometry *r): CsgOp(l, r) {}
	bool boolOp(bool insideL, bool insideR) { return insideL && !insideR; }
	const char* name() const { return "CsgDiff"; }
};

#endif // __GEOMETRY_H__
