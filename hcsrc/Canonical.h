// helencore
// Copyright (C) 2019 Helen Ginn
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

#ifndef __helencore__Canonical__
#define __helencore__Canonical__

#include <vector>

class Canonical
{
public:
	Canonical(int m, int n);
	
	void sizeHint(int n);
	void addVecs(std::vector<double> &ms, std::vector<double> &ns);
	void run();
	double correlation();

	~Canonical();
private:
	typedef struct
	{
		double *vals;
		double **ptrs;
		int rows;
		int cols;
	} Matrix;

	typedef struct
	{
		Matrix u;
		Matrix v;
		double *w;
	} SVD;

	void transformVectors(std::vector<double> &vals, int total,
	                      int chosen, Matrix &basis);
	void setupMatrix(Matrix *mat, int x, int y = 0);
	void setupSVD(SVD *cc, int x, int y = 0);
	void printMatrix(Matrix *mat);
	void reorderSVD(SVD *cc);
	void freeMatrix(Matrix *m);
	void freeSVD(SVD *cc);

	bool runSVD(SVD *cc);
	bool invertSVD(SVD *cc);
	int _nSamples;
	int _m;
	int _n;
	int _dim;
	int _d;
	
	SVD _mmCC, _nnCC;
	Matrix _mBasis, _nBasis;
	Matrix _u, _v;
	bool _run;
	
	std::vector<double> _mVecs;
	std::vector<double> _nVecs;
};

#endif
