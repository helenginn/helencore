// Vagabond
// Copyright (C) 2017-2018 Helen Ginn
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 
// Please email: vagabond @ hginn.co.uk for more details.

#include "Fibonacci.h"
#include <iostream>

Fibonacci::Fibonacci()
{

}

Fibonacci::~Fibonacci()
{

}

void Fibonacci::generateLattice(int num, double radius)
{
	if (num % 2 == 0)
	{
		num += 1;
	}

	_points.clear();
	_points.reserve(num);
	
	double offset = 2. / (double)num;
	double increment = M_PI * (3.0 - sqrt(5));

	for (int i = 0; i < num; i++)
	{
		/* z = roughly from just above -1 to just below +1 */
		double z = (double)i * offset - 1 + offset / 2;
		z = sin(z * M_PI / 2);
		/* how far out do the rest of the coordinates need to stick */
		double r = sqrt(1 - z * z);

		double phi = (double)((i + 1) % num) * increment;

		double x = cos(phi) * r;
		double y = sin(phi) * r;

		vec3 point = make_vec3(x * radius, y * radius, z * radius);

		_points.push_back(point);
	}
}

void Fibonacci::prepareHyperpoints(int dims, int num)
{
	_hyperpoints.clear();
	_hyperpoints.resize(num);
	
	for (size_t i = 0; i < _hyperpoints.size(); i++)
	{
		_hyperpoints[i].resize(dims);
	}
}

void Fibonacci::startHyperpoints(int num, double shift)
{
	double increment = M_PI * (3.0 - sqrt(5));
	int extra = shift * num;

	for (int i = 0; i < num; i++)
	{
		double phi = (double)((i + 1 + extra) % num) * increment;

		double x = cos(phi);
		double y = sin(phi);

		_hyperpoints[i][0] = x;
		_hyperpoints[i][1] = y;
	}
}

void Fibonacci::nextHyperpoints(int dim, int num)
{
	double offset = 2. / (double)num;

	for (int i = 0; i < num; i++)
	{
		/* z = roughly from just above -1 to just below +1 */
		double prop = (double)i * offset - 1 + offset / 2;
		double z = sin(prop * M_PI / 2);

		/* how far out do the rest of the coordinates need to stick */
		double r = sqrt(1 - z * z);

		for (size_t j = 0; j < dim - 1; j++)
		{
			_hyperpoints[i][j] *= r;
		}

		_hyperpoints[i][dim - 1] = z;
	}
}

void Fibonacci::hyperLattice(int dims, int used, int num, 
                             double radius, double shift)
{
	if (num % 2 == 0)
	{
		num += 1;
	}
	
	if (dims < 3) dims = 3;
	
	prepareHyperpoints(dims, num);
	startHyperpoints(num, shift);
	
	for (size_t i = 3; i <= used; i++)
	{
		nextHyperpoints(i, num);
	}
	
	for (size_t i = 0; i < _hyperpoints.size(); i++)
	{
		for (size_t j = 0; j < dims; j++)
		{
			_hyperpoints[i][j] *= radius;
		}
	}
}

std::vector<std::vector<double> > 
	Fibonacci::hyperVolume(int dims, int used, int num, double radius)
{
	std::vector<std::vector<double> > fullPoints;

	/* shortcut if num == 1 */
	if (num <= 1)
	{
		std::vector<double> points;
		for (size_t i = 0; i < dims; i++)
		{
			points.push_back(0);
		}
		fullPoints.push_back(points);
		_hyperpoints = fullPoints;
		return fullPoints;
	}

	/* work out how many layers from how many total samples. */

	double factor = pow(num, 1./3.);
	int layers = lrint(factor);
	
	if (layers % 2 == 0)
	{
		layers++;
	}
	
	std::vector<double> layerSurfaces;
	double totalSurfaces = 0;
	
	/* Work out relative ratios of the surfaces on which points
	 * will be generated. */
	for (int i = 1; i <= layers; i++)
	{
		layerSurfaces.push_back(i * i);
		totalSurfaces += i * i;
	}

	double scale = num / (double)totalSurfaces;

	double addTotal = 0;

	/*
	Fibonacci fib;
	fib.generateLattice(layers, 1);
	std::vector<vec3> directions = fib.getPoints();
	*/
	fullPoints.reserve(num);
	
//	vec3 yAxis = make_vec3(0, 1, 0);

	for (int j = 0; j < layers; j++)
	{
		double frac = (double)(j + 1) / (double)layers;
		double m = radius * frac;

		int samples = layerSurfaces[j] * scale + 1;
		double offset = 2. / (double)samples;
		
		hyperLattice(dims, used, samples, m, frac);
		fullPoints.insert(fullPoints.end(), _hyperpoints.begin(),
		                  _hyperpoints.end());
	}

	_hyperpoints = fullPoints;
	return fullPoints;
}

