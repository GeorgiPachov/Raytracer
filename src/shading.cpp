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

#include "shading.h"
#include "bitmap.h"
#include <math.h>
#include <stdio.h>

Vector lightPos;
Color ambient = Color(0.1f, 0.1f, 0.1f);
Color lightIntensity;

Color Checker::getTexColor(const IntersectionInfo& info)
{
	/*
	 * The checker texture works like that. Partition the whole 2D space
	 * in squares of squareSize. Use division and floor()ing to get the
	 * integral coordinates of the square, which our point happens to be. Then,
	 * use the parity of the sum of those coordinates to decide which color to return.
	 */
	int squareX = (int) floor(info.u / squareSize);
	int squareY = (int) floor(info.v / squareSize);
	if ((squareX + squareY) % 2 == 0) return col1;
	else return col2;
}

bool lightIsVisible(Vector p, Vector l);

Color Lambert::shade(const Ray& ray, const IntersectionInfo& info)
{
	// fetch the material color. This is ether the solid color, or a color
	// from the texture, if it's set up.
	Color materialColor;
	if (texture != NULL) materialColor = texture->getTexColor(info);
	else materialColor = color;
	
	// check if our point (info.ip) is visible from the light:
	
	Vector lightDir = lightPos - info.ip;
	
	double lightDist = lightDir.length();
	Color lightMultiplier = ambient;
	if (lightIsVisible(info.ip, lightPos)) {
		lightMultiplier += lightIntensity / float(sqr(lightDist));
	}
	
	lightDir.normalize();
	// get the Lambertian cosine of the angle between the geometry's normal and
	// the direction to the light. This will scale the lighting:
	double normDotL = lightDir * info.norm;
	if (normDotL < 0) normDotL = 0; // light can be "below" the surface.
	// multiply all that together and apply the quadratic light attenuation law.
	return materialColor * lightMultiplier * (float) (normDotL);
}

Color Phong::shade(const Ray& ray, const IntersectionInfo& info)
{
	// fetch the material color. This is ether the solid color, or a color
	// from the texture, if it's set up.
	Color materialColor;
	if (texture != NULL) materialColor = texture->getTexColor(info);
	else materialColor = color;
	
	bool lightVisible = lightIsVisible(info.ip, lightPos);
	Vector nrm = faceforward(info.norm, ray.dir);
	
	Vector lightDir = lightPos - info.ip;
	
	double lightDist = lightDir.length();
	Color lightMultiplier = ambient;
	if (lightVisible) {
		lightMultiplier += lightIntensity / float(sqr(lightDist));
	}

	lightDir.normalize();
	// get the Lambertian cosine of the angle between the geometry's normal and
	// the direction to the light. This will scale the lighting:
	double normDotL = lightDir * nrm;
	if (normDotL < 0) normDotL = 0; // light can be "below" the surface.
	// multiply all that together and apply the quadratic light attenuation law.
	Color lambertResult = materialColor * lightMultiplier * (float) (normDotL);
	
	if (lightVisible) {
		Vector fromLight = info.ip - lightPos;
		fromLight.normalize();
		
		Vector r = reflect(fromLight, nrm);
		Vector toCamera = ray.start - info.ip;
		toCamera.normalize();
		double cosGamma = dot(toCamera, r);
		Color specularResult = lightIntensity * float(pow(cosGamma, exponent) / sqr(lightDist));
		return lambertResult + specularResult;
	} else return lambertResult;
}


BitmapTexture::BitmapTexture(const char* filename, double _scaling)
{
	scaling = _scaling;
	map = new Bitmap;
	if (!map->loadBMP(filename)) {
		printf("Could not load `%s'\n", filename);
		delete map;
		map = NULL;
	}
}

BitmapTexture::~BitmapTexture(){ if (map) delete map; }

Color BitmapTexture::getTexColor(const IntersectionInfo& info)
{
	if (!map) return Color(0, 0, 0);
	double u = info.u / scaling;
	double v = info.v / scaling;
	double fracu = u - floor(u);
	double fracv = v - floor(v);
	return map->getPixel((int) floor(fracu * map->getWidth()), (int) floor(fracv * map->getHeight()));
}
