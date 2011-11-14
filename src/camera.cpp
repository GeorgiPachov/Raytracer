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

#include "camera.h"
#include "matrix.h"
#include "sdl.h"

void Camera::beginRender()
{
	double x, y;
	x = -aspect;
	y = 1;
	
	double curLength = sqrt(x*x + y*y);
	double L = tan(toRadians(fov / 2));
	double factor = L / curLength;
	x *= factor;
	y *= factor;
	
	upLeft = Vector(x, y, 1);
	upRight = Vector(-x, y, 1);
	downLeft = Vector(x, -y, 1);
	
	Matrix mroll = rotationAroundZ(toRadians(roll));
	Matrix mpitch = rotationAroundX(toRadians(pitch));
	Matrix myaw = rotationAroundY(toRadians(yaw));
	
	Matrix all = mroll * mpitch * myaw;
	
	upLeft *= all;
	upRight *= all;
	downLeft *= all;
	
	upLeft = upLeft + pos;
	upRight = upRight + pos;
	downLeft = downLeft + pos;
}

Ray Camera::getScreenRay(double x, double y)
{
	Vector dest = upLeft + (upRight - upLeft) * (x / frameWidth()) 
	                     + (downLeft - upLeft) * (y / frameHeight());
	Ray result;
	result.start = pos;
	result.dir = dest - pos;
	result.dir.normalize();
	return result;
}

