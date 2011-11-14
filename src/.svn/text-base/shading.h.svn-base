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
#ifndef __SHADING_H__
#define __SHADING_H__


#include "color.h"
#include "geometry.h"

/// An abstract class, representing a shader in our scene.
class Shader {
public:
	virtual ~Shader() {}
	
	virtual Color shade(const Ray& ray, const IntersectionInfo& info) = 0;
};

/// An abstract class, representing a (2D) texture
class Texture {
public:
	virtual ~Texture() {}
	virtual Color getTexColor(const IntersectionInfo& info) = 0;
};

/// A checker texture
class Checker: public Texture {
	Color col1, col2; /// the colors of the alternating squares
	double squareSize; /// the size of a square side, in world units
public:
	Checker(Color c1, Color c2, double size) { col1 = c1; col2 = c2; squareSize = size; }
	Color getTexColor(const IntersectionInfo& info);
};

class Bitmap;
class BitmapTexture: public Texture {
	Bitmap* map;
	double scaling;
public:
	BitmapTexture(const char* filename, double _scaling);
	~BitmapTexture();
	Color getTexColor(const IntersectionInfo& info);
};

/// A Lambert (flat) shader
class Lambert: public Shader {
	Color color; //!< This is the static color of the Lambert shader (to be used if a texture isn't present)
	Texture *texture; //!< a diffuse texture, if not NULL.
public:
	Lambert(Color _color, Texture* _texture = NULL ) { color = _color; texture = _texture; }
	Color shade(const Ray& ray, const IntersectionInfo& info);
};

/// A Lambert (flat) shader
class Phong: public Shader {
	Color color; //!< This is the static color of the Lambert shader (to be used if a texture isn't present)
	Texture *texture; //!< a diffuse texture, if not NULL.
	double exponent;
public:
	Phong(Color _color, double _exponent, Texture* _texture = NULL ) { color = _color; texture = _texture; exponent = _exponent; }
	Color shade(const Ray& ray, const IntersectionInfo& info);
};

#endif // __SHADING_H__
